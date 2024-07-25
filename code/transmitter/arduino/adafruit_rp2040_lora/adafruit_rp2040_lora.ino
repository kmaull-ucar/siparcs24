// Feather9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_RX

#include <SPI.h>
#include <RH_RF95.h>
#include "RTClib.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <Adafruit_TMP117.h>
#include "Adafruit_Si7021.h"
#include <ArduinoUniqueID.h> // https://github.com/ricaun/ArduinoUniqueID
#include "Adafruit_LTR390.h"
#include "Adafruit_PM25AQI.h"
#include <SparkFun_I2C_GPS_Arduino_Library.h> // https://github.com/sparkfun/SparkFun_I2C_GPS_Arduino_Library
#include <TinyGPSPlus.h>

#if defined(ARDUINO_ADAFRUIT_FEATHER_RP2040_RFM)  // Feather RP2040 w/Radio
  #define RFM95_CS   16
  #define RFM95_INT  21
  #define RFM95_RST  17
#endif

#define RF95_FREQ 915.0
#define SEALEVELPRESSURE_HPA (1013.25)

RH_RF95          rf95(RFM95_CS, RFM95_INT);
RTC_PCF8523      pcf8523_rtc;
Adafruit_Si7021  si7021     = Adafruit_Si7021();
Adafruit_BME680  bme680; 
Adafruit_TMP117  tmp117;
Adafruit_LTR390  ltr390     = Adafruit_LTR390();
Adafruit_PM25AQI pm25aqi    = Adafruit_PM25AQI();
I2CGPS           sparkfun_GPS; 
TinyGPSPlus      gps; 

char            device_id[64]       = {0};
int16_t         packetnum           = 0;  // packet counter, we increment per xmission
int             led                 = LED_BUILTIN;
bool            si7021_enableHeater = false;
bool            si7021_connected    = false;
bool            bme680_connected    = false;
bool            tmp117_connected    = false;
bool            pcf8523_connected   = false;
bool            ltr390_connected    = false;
bool            pm25aqi_connected   = false;
bool            sf_xa1110_connected = false;

//      generic struct for storing data; updated based on sensors
struct sensor_data {
  float temperature;
  float humidity;
  float pressure;
  long  tm = 0;
  long  uv = -1;
};


bool sf_xa1110_gps_init() {
  if (sparkfun_GPS.begin() == false)
  {
    Serial.println("[warn] Sparkfun XA1110 GPS Module failed to respond or was not found. Please check wiring.");
    return false;
  }
  Serial.println("[info] Sparkfun XA1110 GPS module found");
  return true;  
}


bool pm25aqi_init() { 

  if (!pm25aqi.begin_I2C()) {
    Serial.println("[warn]: couldn't find Adafruit PMSA003I AQ sensor. Check your connections and verify the address 0x12 is correct.");
    return false;
  } 
 
  Serial.println("[info]: Adafruit PM25AQI found ... OK");

  return true;
}


bool pcf8523_rtc_init() { 
  if (!pcf8523_rtc.begin()) {
    Serial.println("[warn]: couldn't find PCF8523 RTC");
    Serial.flush();
    return false;
  }

  Serial.println("[info]: Adafruit PCF8523 qwiic sensor found ... OK");

  if (! pcf8523_rtc.initialized() || pcf8523_rtc.lostPower()) {
    Serial.println("[warn]: PCF8523 RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    pcf8523_rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    //
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  // When the RTC was stopped and stays connected to the battery, it has
  // to be restarted by clearing the STOP bit. Let's do this to ensure
  // the RTC is running.
  pcf8523_rtc.start();

   // The PCF8523 can be calibrated for:
  //        - Aging adjustment
  //        - Temperature compensation
  //        - Accuracy tuning
  // The offset mode to use, once every two hours or once every minute.
  // The offset Offset value from -64 to +63. See the Application Note for calculation of offset values.
  // https://www.nxp.com/docs/en/application-note/AN11247.pdf
  // The deviation in parts per million can be calculated over a period of observation. Both the drift (which can be negative)
  // and the observation period must be in seconds. For accuracy the variation should be observed over about 1 week.
  // Note: any previous calibration should cancelled prior to any new observation period.
  // Example - RTC gaining 43 seconds in 1 week
  float drift = 43; // seconds plus or minus over oservation period - set to 0 to cancel previous calibration.
  float period_sec = (7 * 86400);  // total obsevation period in seconds (86400 = seconds in 1 day:  7 days = (7 * 86400) seconds )
  float deviation_ppm = (drift / period_sec * 1000000); //  deviation in parts per million (μs)
  float drift_unit = 4.34; // use with offset mode PCF8523_TwoHours
  // float drift_unit = 4.069; //For corrections every min the drift_unit is 4.069 ppm (use with offset mode PCF8523_OneMinute)
  int offset = round(deviation_ppm / drift_unit);
  // rtc.calibrate(PCF8523_TwoHours, offset); // Un-comment to perform calibration once drift (seconds) and observation period (seconds) are correct
  // rtc.calibrate(PCF8523_TwoHours, 0); // Un-comment to cancel previous calibration

  Serial.print("[info]: PCF8523 RTC Offset is "); Serial.println(offset); // Print to control offset
  
  return true;
}


/***
 * 
 */
bool bme680_init() {  
  delay(500);
  if (!bme680.begin()) {
    Serial.println("[warn]: Couldn't find BME680 sensor");
    return false;
  }

  bme680.setTemperatureOversampling(BME680_OS_8X);
  bme680.setHumidityOversampling(BME680_OS_2X);
  bme680.setPressureOversampling(BME680_OS_4X);
  bme680.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme680.setGasHeater(320, 150); // 320*C for 150 ms

  Serial.println("[info]: Adafruit BME680 qwiic sensor found ... OK");

  return true;
}


/***
 * 
 */
bool tmp117_init() {
  delay(500);
  if (!tmp117.begin()) {
    Serial.println("[error]: Adafruit TMP117 qwiic sensor not found");
    return false;
  }
  
  Serial.println("[info]: Adafruit TMP117 qwiic sensor found ... OK");
  return true;
}


/***
 * 
 */
bool si7021_init() {
  delay(500);
  if (!si7021.begin()) {
    Serial.println("[error]: Adafruit si7021 qwiic sensor not found");
	  return false;
  }

  Serial.print("[info]: Adafruit si7021 qwiic sensor found ... OK: model >>");
  switch(si7021.getModel()) {
    case SI_Engineering_Samples:
      Serial.print("SI engineering samples"); break;
    case SI_7013:
      Serial.print("Si7013"); break;
    case SI_7020:
      Serial.print("Si7020"); break;
    case SI_7021:
      Serial.print("Si7021"); break;
    case SI_UNKNOWN:
    default:
      Serial.print("Unknown");
  }
  Serial.print(" Rev(");
  Serial.print(si7021.getRevision());
  Serial.print(")");
  Serial.print(" Serial #"); Serial.print(si7021.sernum_a, HEX); Serial.println(si7021.sernum_b, HEX);

  return true;
}


bool ltr390_init() {

  if (!ltr390.begin()) {
    Serial.println("[error]: Adafruit LTR390 qwiic sensor not found");
    return false;
  }
  
  Serial.println("[info]: Adafruit LTR390 qwiic sensor found ... OK");

  ltr390.setMode(LTR390_MODE_UVS);
  ltr390.setGain(LTR390_GAIN_3);
 
//    case LTR390_GAIN_1
//    case LTR390_GAIN_3
//    case LTR390_GAIN_6
//    case LTR390_GAIN_9
//    case LTR390_GAIN_18
 
  ltr390.setResolution(LTR390_RESOLUTION_16BIT);

//    case LTR390_RESOLUTION_13BIT
//    case LTR390_RESOLUTION_16BIT
//    case LTR390_RESOLUTION_17BIT
//    case LTR390_RESOLUTION_18BIT
//    case LTR390_RESOLUTION_19BIT
//    case LTR390_RESOLUTION_20BIT

  ltr390.setThresholds(100, 1000);
  ltr390.configInterrupt(true, LTR390_MODE_UVS);

  return true;
}


/***
 * 
 */
void blink_led() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                // wait for a half second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(500);
}


/***
 * 
 */
void rfm95_start() {
  pinMode(RFM95_RST, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
}


/***
 * 
 */
void rfm95_reset() {
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
}


/***
 * 
 */
bool rfm95_init() {
  while (!rf95.init()) {
      Serial.println("[warn]: LoRa radio init failed");
      Serial.println("[warn]: Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
      return false;
  }

  Serial.println("[info] Adafruit RP2040-LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("[error] radio setFrequency() failed");
    while (1);
  }
  Serial.print("[info] radio frequency set to: "); Serial.println(RF95_FREQ);

  rf95.setTxPower(23, false); // 23 is this highest power

  return true;
}


/***
 * 
 */
void rfm95_send(char* packet) {  
    Serial.print("[info]: sending packet >> "); Serial.println(packet);  
    Serial.println("Sending...");
    delay(10);
    
    rf95.send((uint8_t *)packet, strlen(packet));
  
    Serial.println("Waiting for packet to complete...");
    delay(10);
    rf95.waitPacketSent();
}


/***
 * 
 */
bool pm25aqi_measure_transmit() {
  struct sensor_data measurement;
  char               radiopacket[254] = {0}; // max packet is 254 bytes, alloc it all now

  PM25_AQI_Data data;

  
  if (!pm25aqi.read(&data)) {
    Serial.println("[warn] could not read from AQI");
    delay(500);
    return false;
  }

  if (pcf8523_connected) {
    measurement.tm = pcf8523_rtc.now().unixtime();
  }

  Serial.println("[info]: rfm95 transmitting packet ..."); // Send a message to rf95_server
  
  delay(50);
  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/pmsa003i/pm10standard\nm:%d\nt:%lu", device_id, data.pm10_standard, measurement.tm);  
  rfm95_send(radiopacket);
  delay(50);

  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/pmsa003i/pm25standard\nm:%d\nt:%lu", device_id, data.pm25_standard, measurement.tm);  
  rfm95_send(radiopacket);
  delay(50);

  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/pmsa003i/pm100standard\nm:%d\nt:%lu", device_id, data.pm100_standard, measurement.tm);  
  rfm95_send(radiopacket);
  delay(50);

  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/pmsa003i/pm10env\nm:%d\nt:%lu", device_id, data.pm10_env, measurement.tm);  
  rfm95_send(radiopacket);
  delay(50);

  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/pmsa003i/pm25env\nm:%d\nt:%lu", device_id, data.pm25_env, measurement.tm);  
  rfm95_send(radiopacket);
  delay(50);

  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/pmsa003i/pm100env\nm:%d\nt:%lu", device_id, data.pm100_env, measurement.tm);  
  rfm95_send(radiopacket);
  delay(50);

  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/pmsa003i/partcount03um\nm:%d\nt:%lu", device_id, data.particles_05um, measurement.tm);  
  rfm95_send(radiopacket);
  delay(50);

  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/pmsa003i/partcount05um\nm:%d\nt:%lu", device_id, data.particles_05um, measurement.tm);  
  rfm95_send(radiopacket);
  delay(50);

  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/pmsa003i/partcount10um\nm:%d\nt:%lu", device_id, data.particles_10um, measurement.tm);  
  rfm95_send(radiopacket);
  delay(50);

  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/pmsa003i/partcount25um\nm:%d\nt:%lu", device_id, data.particles_25um, measurement.tm);  
  rfm95_send(radiopacket);
  delay(50);

  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/pmsa003i/partcount50um\nm:%d\nt:%lu", device_id, data.particles_50um, measurement.tm);  
  rfm95_send(radiopacket);
  delay(50);

  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/pmsa003i/partcount100um\nm:%d\nt:%lu", device_id, data.particles_100um, measurement.tm);  
  rfm95_send(radiopacket);
  delay(50);

  return true;
}


/***
 * 
 */
bool ltr390_measure_transmit() {
  struct sensor_data measurement;
  char               radiopacket[254] = {0}; // max packet is 254 bytes, alloc it all now

  if (pcf8523_connected) {
    measurement.tm = pcf8523_rtc.now().unixtime();
  }

  if (!ltr390.newDataAvailable()) {
    return false;
  }
  
  measurement.uv = ltr390.readUVS();

  Serial.print("[info]: ltr390 ");

  Serial.print("\tuv: ");
  Serial.println(measurement.uv);

  Serial.println("[info]: rfm95 transmitting packet ..."); // Send a message to rf95_server
  
  delay(50);
  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/ltr390/uv\nm:%.2f\nt:%lu", device_id, measurement.uv, measurement.tm);  
  rfm95_send(radiopacket);
  
  return true;
}

/***
 * 
 */
bool tmp117_measure_transmit() {
  struct sensor_data measurement;
  char               radiopacket[254] = {0}; // max packet is 254 bytes, alloc it all now

  if (pcf8523_connected) {
    measurement.tm = pcf8523_rtc.now().unixtime();
  }
  
  sensors_event_t temp; 
  tmp117.getEvent(&temp); 

  Serial.print("[info]: tmp117 humidity:    ");

  Serial.print("\ttemperature: ");
  measurement.temperature = temp.temperature;
  Serial.println(measurement.temperature, 2);

  Serial.println("[info]: rfm95 transmitting packet ..."); // Send a message to rf95_server

  delay(50);
  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/tmp117/temperature\nm:%.2f\nt:%lu", device_id, measurement.temperature, measurement.tm);  
  rfm95_send(radiopacket);
  
  return true;
}
/***
 * 
 */
bool si7021_measure_transmit() {
  struct sensor_data measurement;
  char               radiopacket[254] = {0}; // max packet is 254 bytes, alloc it all now
  
  if (pcf8523_connected) {
    measurement.tm = pcf8523_rtc.now().unixtime();
  }
  
  Serial.print("[info]: si7021 humidity:    ");
  measurement.humidity = si7021.readHumidity();
  Serial.print(measurement.humidity, 2);

  Serial.print("\ttemperature: ");
  measurement.temperature = si7021.readTemperature();
  Serial.println(measurement.temperature, 2);

  Serial.println("[info]: rfm95 transmitting packet ..."); // Send a message to rf95_server

  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/si7021/temperature\nm:%.2f\nt:%lu", device_id, measurement.temperature, measurement.tm);  
  rfm95_send(radiopacket);
  
  delay(50);
  
  sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor:i2c/si7021/humidity\nm:%.2f\nt:%lu", device_id, measurement.humidity, measurement.tm);
  rfm95_send(radiopacket);

  return true;
}


/***
 * 
 */
bool bme680_measure_transmit() {
  struct sensor_data measurement;
  char               radiopacket[254] = {0}; // max packet is 254 bytes, alloc it all now

  if (pcf8523_connected) {
    measurement.tm = pcf8523_rtc.now().unixtime();
  }
  
  if (!bme680.performReading()) {
    Serial.println("[error]: Failed to perform reading :(");
    return false;
  }
  
  Serial.print("[info]: bme680 humidity:    ");
  measurement.humidity = bme680.humidity;
  Serial.print(measurement.humidity, 2);

  Serial.print("\ttemperature: ");
  measurement.temperature = bme680.temperature;

  
  Serial.print("\tpressure: ");
  measurement.pressure = bme680.pressure / 100.0;
  Serial.println(measurement.pressure, 2);

  Serial.println("[info]: rfm95 transmitting packet ..."); // Send a message to rf95_server

  sprintf(radiopacket, "device: adafruit/rp2040/%s\nsensor: i2c/bme680/temperature\nm: %.2f\nt: %lu", device_id, measurement.temperature, measurement.tm);
  rfm95_send(radiopacket);
  
  delay(50);
  
  sprintf(radiopacket, "device: adafruit/rp2040/%s\nsensor: i2c/bme680/humidity\nm: %.2f\nt: %lu", device_id, measurement.humidity, measurement.tm);
  rfm95_send(radiopacket);

  sprintf(radiopacket, "device: adafruit/rp2040/%s\nsensor: i2c/bme680/pressure\nm: %.2f\nt: %lu", device_id, measurement.pressure, measurement.tm);
  rfm95_send(radiopacket);

  return true;
}

bool sf_xa1110_measure_transmit() {
  struct sensor_data measurement;
  char               radiopacket[254] = {0}; // max packet is 254 bytes, alloc it all now

  if (pcf8523_connected) {
    measurement.tm = pcf8523_rtc.now().unixtime();
  } // TODO: implement fall back to GPS time

  
  while (sparkfun_GPS.available()) //available() returns the number of new bytes available from the GPS module
  {
    gps.encode(sparkfun_GPS.read()); //Feed the GPS parser
  }

  if (gps.location.isValid())
  {
    double lat = gps.location.lat();
    double lng = gps.location.lng();
  
    Serial.print("[info] location: "); 
    Serial.print(lat, 6); Serial.print(F(", "));
    Serial.println(lng, 6);
  
    Serial.println("[info]: rfm95 transmitting packet ..."); // Send a message to rf95_server

    delay(50);
    sprintf(radiopacket, "device:adafruit/rp2040/%s\nsensor: i2c/sfxa1110/gps\nm: %.6f,%.6f\nt: %lu", device_id, lat, lng, measurement.tm);
    rfm95_send(radiopacket);
  }
  else
  {
    Serial.println(F("Location not yet valid"));
    return false;
  }

  return true;
}


void setup() {
  delay(5000);
  Serial.begin(115200);
  delay(100);
  Serial.println("*** This is IoTwx-LoRa-RP2040 v0.1 ...");

  // setup radio
  rfm95_start();
  rfm95_reset(); // bounce the radio once more
  rfm95_init();
     
  for(size_t i = 0; i < UniqueIDsize; i++)
    sprintf(&device_id[i], "%.2X", UniqueID[i]);

  Serial.print("[info]: device_id ==> ");  Serial.println(device_id);
  
  // start up sensors
  si7021_connected  = si7021_init();
  bme680_connected  = bme680_init();
  tmp117_connected  = tmp117_init();
  pcf8523_connected = pcf8523_rtc_init();
  ltr390_connected  = ltr390_init();
  pm25aqi_connected = pm25aqi_init();
  sf_xa1110_connected = sf_xa1110_gps_init();
}


void loop() {
  struct sensor_data measurements;
  bool   transmit_ok; 
  
  if (si7021_connected) {
    transmit_ok = si7021_measure_transmit();
    if (transmit_ok) {
      blink_led();
    }
  }

  if (bme680_connected) {
    transmit_ok = bme680_measure_transmit();
    if (transmit_ok) {
      blink_led();
    }
  }

  if (tmp117_connected) {
    transmit_ok = tmp117_measure_transmit();
    if (transmit_ok) {
      blink_led();
    }
  }

  if (tmp117_connected) {
    transmit_ok = ltr390_measure_transmit();
    if (transmit_ok) {
      blink_led();
    }
  }

  if (pm25aqi_connected) {
    transmit_ok = pm25aqi_measure_transmit();
    if (transmit_ok) {
      blink_led();
    }
  }

  if (sf_xa1110_connected) {
    transmit_ok = sf_xa1110_measure_transmit();
    if (transmit_ok) {
      blink_led();
    }
  }
  
  delay(1000); // Wait 1 second between transmits, could also 'sleep' here!  
}
