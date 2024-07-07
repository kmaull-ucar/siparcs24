"""
	LoRa Bonnet implemetation for Pi

	authors: A. Hossain, K.E. Maull
	(c) 2024
"""

import board
import busio
import digitalio
from digitalio import DigitalInOut, Direction, Pull
import adafruit_rfm9x
import adafruit_ssd1306
import paho.mqtt.client as mqtt
import json
import yaml
import time


def initialize_led(i2c):
	"""Description of Function

	Parameters:
		config (dict): ...

	Returns:
	mqtt.Client object

	Thanks Adafruit! [https://learn.adafruit.com/lora-and-lorawan-for-raspberry-pi]
	"""
	# 128x32 OLED Display
	reset_pin = DigitalInOut(board.D4)
	display = adafruit_ssd1306.SSD1306_I2C(128, 32, i2c, reset=reset_pin)

	# Clear the display
	display.fill(0)
	display.show()
	width = display.width
	height = display.height

	return display

def initialize_radio(freq=915, power=23):
	"""Description of Function

	Parameters:
		config (dict): ...

	Returns:
	mqtt.Client object

	Thanks Adafruit! [https://learn.adafruit.com/lora-and-lorawan-for-raspberry-pi]
	"""
	# Define pins connected to the chip
	CS = digitalio.DigitalInOut(board.CE1)
	RESET = digitalio.DigitalInOut(board.D25)

	# Initialize SPI bus.
	spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)

	# Initialize RFM radio and power dB
	rfm9x = adafruit_rfm9x.RFM9x(spi, CS, RESET, freq)
	rfm9x.tx_power = power
	
	return rfm9x


def initialize_mqtt_client(broker=None, port=1883):
	"""Description of Function

	Parameters:
		config (dict): ...

	Returns:
	mqtt.Client object

	"""

	if broker and port:
		try: 
			# MQTT callback 
			def on_connect(client, userdata, flags, rc):
				print("Connected with result code " + str(rc))

			# MQTT Initialization
			mqtt_client = mqtt.Client()
			mqtt_client.on_connect = on_connect
			mqtt_client.connect(broker, port, 60)
			mqtt_client.loop_start()

			return mqtt_client
		except Exception as e: 
			print(f"[halt]: missing/improperly formatted configuration file or mqtt connect failure: {e}")
	else:
		print("[halt]: missing configuration > no broker and/or port")





def main():
	# Create the I2C interface.
	i2c = busio.I2C(board.SCL, board.SDA)

	with open("config.yml") as f: # load config
		config = yaml.safe_load(f)
		
		broker = config['mqtt']['broker']
		port = config['mqtt']['port']
		topic = config['mqtt']['topic']

		rcv_timeout = config['radio']['rcv_timeout']

	mqtt_client = initialize_mqtt_client(broker, port)
	display = initialize_led(i2c)

	print("[info]: waiting for LoRa radio packets...")

	# Draw a black filled box to clear the image.
	display.fill(0)

	try:
		radio = initialize_radio()
		display.text('RFM9x: Detected', 0, 0, 1)

		display.show()
		time.sleep(5)

		display.fill(0)
		display.show()

		display.text('RFM9x: Waiting to RX', 0, 0, 1)
		display.show()
	except RuntimeError as error:
		# Thrown on version mismatch
		display.text('RFM9x: ERROR', 0, 0, 1)
		print('[error]: RFM9x Error: ', error)
		return

	while True:
		packet = radio.receive(timeout=rcv_timeout)

		if packet is not None:
			try: 
				msg = str(packet, "utf8")
				rssi = radio.last_rssi 

				print(f"[info]: RX: {msg}")
				display.fill(0)
				display.text(f"[RX]:\n{msg}\nrssi: {rssi}", 0, 0, 1)
				display.show()

				# Publish data to MQTT
				if mqtt_client:
					mqtt_client.publish(topic, f"{msg}\nrssi:{rssi}")
			except:
				pass # TODO: occassionally there are encoding errors, please fix
	
	mqtt_client.loop_stop()
	mqtt_client.disconnect()


if __name__ == "__main__":
	main()
