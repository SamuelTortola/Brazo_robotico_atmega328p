# Import standard python modules.
import sys
import time
import random
import serial


# This example uses the MQTTClient instead of the REST client
from Adafruit_IO import MQTTClient



# holds the count for the feed
global run_count

ADAFRUIT_IO_USERNAME = "samuetor"
ADAFRUIT_IO_KEY = "aio_xtjq36avTguBE0qVFQO24TNqPOWM"

# Set to the ID of the feed to subscribe to for updates.
#feedContador = 'contador'
feedLed1 = 'Led1'
feedLed2 = 'Led2'
feedtemperatura = 'temperatura'

mensaje = ''

# Define callback functions which will be called when certain events happen.
def connected(client):
    """Connected function will be called when the client is connected to
    Adafruit IO.This is a good place to subscribe to feed changes.  The client
    parameter passed to this function is the Adafruit IO MQTT client so you
    can make calls against it easily.
    """
    # Subscribe to changes on a feed named Counter.
    #print('Subscribing to Feed {0} and {1}'.format(feedLed, feedContador))
    client.subscribe(feedLed1)
    client.subscribe(feedLed2)
    client.subscribe(feedtemperatura)
    print('Waiting for feed data...')

def disconnected(client):
    """Disconnected function will be called when the client disconnects."""
    sys.exit(1)

def message(client, feed_id, payload):
    """Message function will be called when a subscribed feed has a new value.
    The feed_id parameter identifies the feed, and the payload parameter has
    the new value.
    """
    print('Feed {0} received new value: {1}'.format(feed_id, payload))
    
    if(feed_id == feedLed1):
        if(payload == '0'):
            print('led1: OFF')
            arduino.write(bytes('0\n', 'utf-8'))
        if(payload == '1'):
            print('led1: ON')
            arduino.write(bytes('1\n', 'utf-8'))
    if(feed_id == feedLed2):
        if(payload == '0'):
            print('led2: OFF')
            arduino.write(bytes('0\n', 'utf-8'))
        if(payload == '2'):
            print('led2: ON')
            arduino.write(bytes('2\n', 'utf-8'))


    
    


try:
    client = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

    # Setup the callback functions defined above.
    client.on_connect = connected
    client.on_disconnect = disconnected
    client.on_message = message

    # Connect to the Adafruit IO server.
    client.connect()
    client.loop_background()
              
    arduino = serial.Serial(port='/dev/cu.usbserial-4', baudrate =9600, timeout = 0.1)
    
    while True:    
        mensaje = arduino.readline().decode('utf-8')
        if(mensaje == 'LED1\n'):
            print('LED 1\n')
            client.publish(feedLed1, 1)
        if(mensaje == 'LED2\n'):
            print('LED 2\n')
            client.publish(feedLed2, 2)
        time.sleep(3)
        
        
except KeyboardInterrupt:
    print("Salimos del programa")
    if arduino.is_open:
        arduino.close()
    sys.exit(1)