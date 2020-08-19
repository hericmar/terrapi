import datetime
import time
import sys
import Adafruit_DHT

while True:
    prev = datetime.datetime.now()
    humidity, temperature = Adafruit_DHT.read_retry(11, 4)
    now = datetime.datetime.now()

    print('Temp: {0:0.1f} C  Humidity: {1:0.1f} %'.format(temperature, humidity))

    elapsed = now - prev
    print(f"Took {elapsed.seconds}:{elapsed.microseconds} ns.")

    time.sleep(2)
