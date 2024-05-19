# Import standard python modules.
import sys
import time
import random
import serial


#Uses the MQTTClient 
from Adafruit_IO import MQTTClient

ADAFRUIT_IO_USERNAME = "USUARIO"
ADAFRUIT_IO_KEY      = "CONTRASEÑA"
 

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
                
        if(payload == 'A'):
                arduino.write(bytes('A\n', 'utf-8'))
                
        if(payload == 'B'):
                arduino.write(bytes('B\n', 'utf-8'))
                
        if(payload == 'C'):
                arduino.write(bytes('C\n', 'utf-8'))
                
        if(payload == 'D'):
                arduino.write(bytes('D\n', 'utf-8'))
                
                
                
    if(feed_id == feedEEPROM):
        if(payload == '1'):
                arduino.write(bytes('l\n', 'utf-8'))
                
                
    if(feed_id == feedgarra or feed_id == feedcodo or feed_id == feedbrazo or feed_id == feedrota):
        
        if(feed_id == feedgarra):         #Ver que servomotor se desea mover
            arduino.write(bytes('g\n', 'utf-8'))
            time.sleep(0.1)   #tiempo paara que el atmega328p pueda procesar los datos
            
        if(feed_id == feedcodo):
            arduino.write(bytes('a\n', 'utf-8'))
            time.sleep(0.1)
            
        if(feed_id == feedbrazo):
            arduino.write(bytes('b\n', 'utf-8'))
            time.sleep(0.1)
            
        if(feed_id == feedrota):
            arduino.write(bytes('c\n', 'utf-8'))
            time.sleep(0.1)
        
        if (payload == '0'):  #Enviar el angulo deseado, segun estan los slider de adafruit
            arduino.write(bytes('0\n', 'utf-8'))
            
        if (payload == '10'):
            arduino.write(bytes('1\n', 'utf-8'))
           
            
        if (payload == '20'):
            arduino.write(bytes('2\n', 'utf-8'))
          
        
        if (payload == '30'):
            arduino.write(bytes('3\n', 'utf-8'))
           
            
        if (payload == '40'):
            arduino.write(bytes('4\n', 'utf-8'))
           
            
        if (payload == '50'):
            arduino.write(bytes('5\n', 'utf-8'))
           
        
        if (payload == '60'):
            arduino.write(bytes('6\n', 'utf-8'))
            
            
        if (payload == '70'):
            arduino.write(bytes('7\n', 'utf-8'))
            
            
        if (payload == '80'):
            arduino.write(bytes('8\n', 'utf-8'))
            
        
        if (payload == '90'):
            arduino.write(bytes('9\n', 'utf-8'))
            
            
        if (payload == '100'):
            arduino.write(bytes('A\n', 'utf-8'))
          
            
        if (payload == '110'):
            arduino.write(bytes('B\n', 'utf-8'))
           
        
        if (payload == '120'):
            arduino.write(bytes('C\n', 'utf-8'))
           
            
        if (payload == '130'):
             arduino.write(bytes('D\n', 'utf-8'))
            
            
        if (payload == '140'):
            arduino.write(bytes('e\n', 'utf-8'))
         
        
        if (payload == '150'):
            arduino.write(bytes('F\n', 'utf-8'))
          
            
        if (payload == '160'):
            arduino.write(bytes('G\n', 'utf-8'))
       
        
        if (payload == '170'):
            arduino.write(bytes('H\n', 'utf-8'))
      
            
        if (payload == '180'):
             arduino.write(bytes('I\n', 'utf-8'))
 
            
        if (payload == '190'):
            arduino.write(bytes('J\n', 'utf-8'))

        
        if (payload == '200'):
            arduino.write(bytes('K\n', 'utf-8'))

            
        if (payload == '210'):
            arduino.write(bytes('L\n', 'utf-8'))
  
        
        if (payload == '220'):
            arduino.write(bytes('M\n', 'utf-8'))
     
            
        if (payload == '230'):
             arduino.write(bytes('N\n', 'utf-8'))
      
            
        if (payload == '240'):
            arduino.write(bytes('O\n', 'utf-8'))

        
        if (payload == '250'):
            arduino.write(bytes('P\n', 'utf-8'))
        
        if (payload == '255'):
            arduino.write(bytes('q\n', 'utf-8'))
 

    
        
        
           

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
        mensaje = arduino.readline().decode('utf-8')     #Si se recibe un mensaje del atmega328p
        print(mensaje)
        
        if(mensaje == 'Z\n'):
            print('Entrando a estado EEPROM\n')
            client.publish(feedes, 2)   #Encender LEDs correspondientes en Adafruit
            
        if(mensaje == 'A\n'):
            print('Entrando a estado en linea\n')
            client.publish(feedes, 3)
            
        if(mensaje == 'B\n'):
            print('Entrando a estado Manual\n')
            client.publish(feedes, 1)
       
        time.sleep(3)
        
        
except KeyboardInterrupt:
    print("Salimos del programa")
    if arduino.is_open:
        arduino.close()
    sys.exit(1)