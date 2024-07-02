import board
import busio
import digitalio
import adafruit_rfm9x
import paho.mqtt.client as mqtt
import json

# Define radio parameters.
RADIO_FREQ_MHZ = 915.0  # Frequency of the radio in Mhz. Must match your module!
# Can be a value like 915.0, 433.0, etc.

# Define pins connected to the chip
CS = digitalio.DigitalInOut(board.CE1)
RESET = digitalio.DigitalInOut(board.D25)

# Initialize SPI bus.
spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)

# Initialize RFM radio
rfm9x = adafruit_rfm9x.RFM9x(spi, CS, RESET, RADIO_FREQ_MHZ)
rfm9x.tx_power = 23  # Set transmit power (in dB)

# Define MQTT parameters
MQTT_BROKER = "iotwx.ucar.edu"  # Local MQTT broker
MQTT_PORT = 1883
MQTT_TOPIC = "test/topic"  # Example topic

# MQTT callback functions
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

# Initialize MQTT client
mqtt_client = mqtt.Client()
mqtt_client.on_connect = on_connect
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
mqtt_client.loop_start()

print("Waiting for packets...")

while True:
    packet = rfm9x.receive(timeout=5.0)
    if packet is None:
        print("Received nothing! Listening again...")
    else:
        print("Received (raw bytes): {0}".format(packet))
        packet_text = str(packet, "ascii")
        print("Received (ASCII): {0}".format(packet_text))
        rssi = rfm9x.last_rssi
        print("Received signal strength: {0} dB".format(rssi))

        # Prepare data for MQTT
        data = {
            "packet": packet_text,
            "rssi": rssi
        }

        # Publish data to MQTT
        mqtt_client.publish(MQTT_TOPIC, json.dumps(data))

mqtt_client.loop_stop()
mqtt_client.disconnect()
