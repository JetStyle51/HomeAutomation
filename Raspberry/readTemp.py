import os
import time
import sys
import adafruit_dht
from board import *
import paho.mqtt.client as mqtt
import json

# GPIO4
SENSOR_PIN = D4

SERVEUR = '192.168.1.17'

INTERVAL=10

next_reading = time.time()

client = mqtt.Client()

# Set access token
# client.username_pw_set(ACCESS_TOKEN)

# Connect to ThingsBoard using default MQTT port and 60 seconds keepalive interval
client.connect(SERVEUR, 1883, 60)

sensor_data = {'temperature': 0, 'humidity': 0}

client.loop_start()

dht11 = adafruit_dht.DHT11(SENSOR_PIN, use_pulseio=False)


try:
    while True:
        #humidity,temperature = dht.read_retry(dht.DHT11, 4)
        temperature = dht11.temperature
        humidity = dht11.humidity
        print(u"Temperature: {:g}\u00b0C, Humidity: {:g}%".format(temperature, humidity))
        sensor_data['temperature'] = temperature
        sensor_data['humidity'] = humidity

        # Sending humidity and temperature data to ThingsBoard
        client.publish('iot', json.dumps(sensor_data), 1)

        next_reading += INTERVAL
        sleep_time = next_reading-time.time()
        if sleep_time > 0:
            time.sleep(sleep_time)
except KeyboardInterrupt:
    pass

client.loop_stop()
client.disconnect()