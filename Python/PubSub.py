# Import standard python modules.
import sys
import time
import random
import serial


#Uses the MQTTClient 
from Adafruit_IO import MQTTClient


ADAFRUIT_IO_USERNAME = "poner su username"
ADAFRUIT_IO_KEY      = "poner su clave"

# Set to the ID of the feed to subscribe to for updates.
feedsecuenciap = 'secuenciap'
feedsecuenciao = 'secuenciao'
feedestado = 'estado'
feedconfirmar = 'confirmar'
feedEEPROM = 'EEPROM'
feedbrazo = 'brazo'
feedcodo = 'codo'
feedgarra = 'garra'
feedrota = 'rota'
feedes = 'es'

mensaje = ''

# Define callback functions which will be called when certain events happen.
def connected(client):

    # Subscribe to changes on a feed 
    client.subscribe(feedsecuenciap)
    client.subscribe(feedsecuenciao)
    client.subscribe(feedestado)
    client.subscribe(feedconfirmar)
    client.subscribe(feedEEPROM)
    client.subscribe(feedbrazo)
    client.subscribe(feedcodo)
    client.subscribe(feedgarra)
    client.subscribe(feedrota)
    client.subscribe(feedes)
    client.publish(feedes, 1)


def disconnected(client):
    """Disconnected function will be called when the client disconnects."""
    sys.exit(1)

def message(client, feed_id, payload):
    """Message function will be called when a subscribed feed has a new value.
    The feed_id parameter identifies the feed, and the payload parameter has
    the new value.
    """
    print('Feed {0} received new value: {1}'.format(feed_id, payload))
    
    if(feed_id == feedsecuenciap):
        if(payload == '1'):
            arduino.write(bytes('A\n', 'utf-8'))
    
    if(feed_id == feedsecuenciao):
            if(payload == '2'):
                arduino.write(bytes('B\n', 'utf-8'))
                
    if(feed_id == feedestado):
            if(payload == '3'):
                arduino.write(bytes('Q\n', 'utf-8'))
                client.publish(feedes, 1)
            
            if(payload == '4'):
                arduino.write(bytes('W\n', 'utf-8'))
                client.publish(feedes, 2)
            
            if(payload == '5'):
                arduino.write(bytes('E\n', 'utf-8'))
                client.publish(feedes, 3)
            
    if(feed_id == feedconfirmar):
        if(payload == '1'):
                arduino.write(bytes('R\n', 'utf-8'))
                
    if(feed_id == feedgarra):
        if (payload == '1'):
            arduino.write(bytes(10, 'utf-8'))

try:
    client = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

    # Setup the callback functions defined above.
    client.on_connect = connected
    client.on_disconnect = disconnected
    client.on_message = message

    # Connect to the Adafruit IO server.
    client.connect()
    client.loop_background()
              
    arduino = serial.Serial(port='COM6', baudrate =9600, timeout = 0.1)
    
    while True:    
        mensaje = arduino.readline().decode('utf-8')
        print(mensaje)
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