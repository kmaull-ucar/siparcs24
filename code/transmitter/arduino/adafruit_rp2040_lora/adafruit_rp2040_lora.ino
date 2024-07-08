// Feather9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_RX

#include <SPI.h>
#include <RH_RF95.h>
#include "Adafruit_Si7021.h"



// First 3 here are boards w/radio BUILT-IN. Boards using FeatherWing follow.
#if defined (__AVR_ATmega32U4__)  // Feather 32u4 w/Radio
  #define RFM95_CS    8
  #define RFM95_INT   7
  #define RFM95_RST   4

#elif defined(ADAFRUIT_FEATHER_M0) || defined(ADAFRUIT_FEATHER_M0_EXPRESS) || defined(ARDUINO_SAMD_FEATHER_M0)  // Feather M0 w/Radio
  #define RFM95_CS    8
  #define RFM95_INT   3
  #define RFM95_RST   4

#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040_RFM)  // Feather RP2040 w/Radio
  #define RFM95_CS   16
  #define RFM95_INT  21
  #define RFM95_RST  17

#elif defined (__AVR_ATmega328P__)  // Feather 328P w/wing
  #define RFM95_CS    4  //
  #define RFM95_INT   3  //
  #define RFM95_RST   2  // "A"

#elif defined(ESP8266)  // ESP8266 feather w/wing
  #define RFM95_CS    2  // "E"
  #define RFM95_INT  15  // "B"
  #define RFM95_RST  16  // "D"

#elif defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) || defined(ARDUINO_NRF52840_FEATHER) || defined(ARDUINO_NRF52840_FEATHER_SENSE)
  #define RFM95_CS   10  // "B"
  #define RFM95_INT   9  // "A"
  #define RFM95_RST  11  // "C"

#elif defined(ESP32)  // ESP32 feather w/wing
  #define RFM95_CS   33  // "B"
  #define RFM95_INT  27  // "A"
  #define RFM95_RST  13

#elif defined(ARDUINO_NRF52832_FEATHER)  // nRF52832 feather w/wing
  #define RFM95_CS   11  // "B"
  #define RFM95_INT  31  // "C"
  #define RFM95_RST   7  // "A"

#endif

#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);
Adafruit_Si7021 si7021   = Adafruit_Si7021();
char    device_id[64]    = {0};
bool si7021_enableHeater = false;
int16_t packetnum        = 0;  // packet counter, we increment per xmission
int led                  = LED_BUILTIN;

bool si7021_connected    = false;

// generic struct for storing data; updated based on sensors
struct sensor_data {
  float temperature;
  float humidity;
};


/***
 * 
 */
bool si7021_init() {
  
  if (!si7021.begin()) {
    Serial.println("[error]: Adafruit si7021 qwiic sensor not found");
	return false;
  }

  Serial.print("[info]: Adafruit si7021 qwiic sensor found: model >>");
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

  // set device id sent with packet
  sprintf(device_id, "%.2X%.2X", si7021.sernum_a, si7021.sernum_b);
  
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
struct sensor_data si7021_get_measurements() {
  struct sensor_data measurement;

  Serial.print("[info]: si7021 humidity:    ");
  measurement.humidity = si7021.readHumidity();
  Serial.print(measurement.humidity, 2);

  Serial.print("\ttemperature: ");
  measurement.temperature = si7021.readTemperature();
  Serial.println(measurement.temperature, 2);

  Serial.println("[info]: rfm95 transmitting packet ..."); // Send a message to rf95_server
  return measurement;
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

  // start up sensors
  si7021_connected = si7021_init();
}


void loop() {
  struct sensor_data measurements;
  char               radiopacket[254] = {0}; // max packet is 254 bytes, alloc it all now

  if (si7021_connected) {
    measurements = si7021_get_measurements();
  
    sprintf(radiopacket, "did: %s\nt:%.2f\nh:%.2f ", device_id, measurements.temperature, measurements.humidity);
  
    Serial.print("[info]: sending packet >> "); Serial.println(radiopacket);  
    Serial.println("Sending...");
    delay(10);
    rf95.send((uint8_t *)radiopacket, 25);
  
    Serial.println("Waiting for packet to complete...");
    delay(10);
    rf95.waitPacketSent();
  
    blink_led();
  }
  
  delay(1000); // Wait 1 second between transmits, could also 'sleep' here!  
}
