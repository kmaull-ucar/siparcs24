import paho.mqtt.client as mqtt
import pandas as pd
from datetime import datetime

# Initialize an empty DataFrame with the necessary columns
columns = ['device_id', 'sensor_type', 'measurement', 'timestamp', 'rssi', 'record_time']
data = pd.DataFrame(columns=columns)

# MQTT connection settings
broker = "iotwx.ucar.edu"  # Replace with your MQTT broker address
port = 1883
topic = "/test/topic"  # Replace with your topic

# Callback function to handle incoming messages
def on_message(client, userdata, msg):
    global data
    payload = msg.payload.decode()
    parsed_data = parse_payload(payload)
    if parsed_data:
        # Create a new DataFrame from the parsed data and concatenate it with the existing DataFrame
        new_data = pd.DataFrame([parsed_data])
        data = pd.concat([data, new_data], ignore_index=True)
        print(data)

def parse_payload(payload):
    lines = payload.split('\n')
    parsed_data = {}
    for line in lines:
        if line.startswith('device:'):
            parsed_data['device_id'] = line.split(':')[1].strip()
        elif line.startswith('sensor:'):
            parsed_data['sensor_type'] = line.split(':')[1].strip()
        elif line.startswith('m:'):
            parsed_data['measurement'] = float(line.split(':')[1].strip())
        elif line.startswith('t:'):
            parsed_data['timestamp'] = int(line.split(':')[1].strip())
        elif line.startswith('rssi:'):
            parsed_data['rssi'] = int(line.split(':')[1].strip())
        elif '/' in line:
            parsed_data['record_time'] = datetime.strptime(line, '%m/%d/%Y %I:%M:%S %p')
    return parsed_data

# Set up MQTT client
client = mqtt.Client()
client.on_message = on_message

client.connect(broker, port, 60)
client.subscribe(topic)

# Start the MQTT client
client.loop_forever()
