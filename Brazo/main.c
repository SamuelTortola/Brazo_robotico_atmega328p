//******************************************************************************
// Universidad Del Valle De Guatemala
// IE2023: Programación de Microcontroladores
// Autor: Samuel Tortola - 22094
// Proyecto: Proyecto brazo robotico 
// Hardware: Atmega238P
// Creado: 26/04/2024
//Última modificación: 18/5/2024
//******************************************************************************



#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/eeprom.h>

#include "PWM1/PWM1.h"
#include "ADC/ADC.h"
#include "PWM2/PWM2.h"
#include "UART/UART.h"
#include "EEPROM/EEPROM.h"


void setup(void);

int caso = 0, activa = 0,  activa2 = 0, estado = 0, activa3 = 0, samtf=0, activa4 = 0, activa5 = 0, moverr = 0;      //Variables a utilizar para el procesamiento 
int garra = 0, garra1 = 0, brazo = 0, brazo1 = 0, codo = 0, codo1 = 0, rota = 0, rota1 = 0, sumar = 0;
volatile char receivedChar = 0, mover = 0;
int anterior1 = 0;

uint8_t pos1[4]={};    //Guardar posicion 1 de servomotores en EEPROM
uint8_t pos2[4]={};    //Guardar posicion 2 de servomotores en EEPROM
uint8_t pos3[4]={};    //Guardar posicion 3 de servomotores en EEPROM
uint8_t pos4[4]={};    //Guardar posicion 4 de servomotores en EEPROM



void setup(void){
	cli();  //Apagar interrupciones
	
	DDRB |= (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB3); //PB1, PB2, PB3 como salida de servos
	DDRC =0;  //Puerto C como entrada
	DDRD |= (1 << DDD3) | (1 << DDD4) | (1 << DDD5) | (1 << DDD6);   //PD3 como salida de servo, PD4 - PD6 salida de LEDS
	DDRB &= 0b00001110;   //Activar entrada de pulsador 1 de estado, 2 de guardar/confirmar. 
	PORTB = (1 << PORTB4) | (1 << PORTB0);  //Activar PULLUP 
	DDRD = 0b01111000;
	
	initFastPWM1(8);  //Iniciar funcion de FASTPWM en timer1
	channel(channelB, modo);
	channel(channelA, modo);
	topValue(29999);   //Valor alto de desbordamiento para 1 a 2 ms que se necesita de servo
	initADC();   //Iniciar la funcion de ADC
	initFastPWM2();  //Iniciar funcion de FASTPWM en timer2
	initUART9600();
	PORTD |=  (1 << DDD4);
	
    sei();   //Activar interrupciones
}


int main(void)
{
	setup();
	while (1)
	{                                //Cambiar estado por medio de Adafruit 
		if (receivedChar == 'Q'){
			estado = 0;
			activa2 = 0;
			samtf = 0;
			receivedChar = 0;
		}
		else if (receivedChar == 'W'){
			estado = 1;
			activa2 = 0;
			samtf = 1;
			receivedChar = 0;
			
		}
		else if (receivedChar == 'E'){
			estado = 2;
			activa2 = 1;
			samtf = 2;
			UDR0 = 'A';
			UDR0 = '\n';
			receivedChar = 0;
		
		}
		
		
	 if ((PINB & (1 << PINB4)) == 0)   //Cuando se presiona el boton de cambio de estado, cambiar estado
		{
			_delay_ms(30);    //antirrebote
			samtf ++;
			
			if (samtf == 1)
			{
				estado = 1;
				activa2 = 0;
				 UDR0 = 'Z';
				 UDR0 = '\n';
			}
			
			else if (samtf == 2)
			{
				estado = 2;
				activa2 = 1;
				 UDR0 = 'A';
				 UDR0 = '\n';
			}
			
			
			else if (samtf >= 3)
			{
				samtf = 0;
				estado = 0;
				activa2 = 0;
				 UDR0 = 'B';
				 UDR0 = '\n';
			}
			
				while ((PINB & (1 << PINB4)) == 0)   //While para evitar sumas indebidas
				{
					_delay_ms(30);    //antirrebote
				}
		}

		switch (estado){
			
			case 0: //Si se desea manipular al robot con UART desde adafruit con secuencias
			
				PORTD &=  (0 << DDD4);
				PORTD |=  (1 << DDD5);
				PORTD |=  (0 << DDD6);
				activa3 = 0;
								
				if(activa == 0){
				activa ++;   //Salir del menu 
				}
		
				else if (activa == 1){
					if(receivedChar != 0 || (PINC & (1 << PINC4)) || (PINC & (1 << PINC5)) || activa5 == 1){      //Si la variable que hay en USART es diferente de cero o pulsadores se presionan 
						
						activa5 = 1;
						
						if (garra >= 255){
							garra = 254;
						}
						else if (garra <= 1){
							garra = 0;
						}
						
						if ((PINC & (1 << PINC4)))
						{
							_delay_ms(15);
							receivedChar = 'A';
						}
						
						else if ((PINC & (1 << PINC5)))
						{
							_delay_ms(15);
							receivedChar = 'B';
						}
						
						else if ((PINB & (1 << PINB0))==0)
						{
							
							receivedChar = 'R';
						}
						
						while ((PINB & (1 << PINB0)) == 0)   //While para evitar sumas indebidas
						{
							_delay_ms(30);    //antirrebote
						}
						
						switch(receivedChar)   //Switch para elegir que se desea
						{
							case 'A':
								activa2 = 1;   //Desactivar el ADC
								garra = garra + 10;
								convertServo2(garra, channel2A);
								receivedChar = 0;
					          
								break;
							
							case 'B':
								activa2 = 1;   //Desactivar el ADC
								garra = garra - 10;
								convertServo2(garra, channel2A);
								receivedChar = 0;
								
								break;
							
							case 'R':
								garra1 = garra;
								activa ++;
								receivedChar = 0;
								break;
							
						}
						
						
					}
					
				}
				
				
				else if (activa == 2){
					activa5 = 0;
					if(receivedChar != 0 || (PINC & (1 << PINC4)) || (PINC & (1 << PINC5)) || (PINB & (1 << PINB0)) == 0){      
					
						
						if (garra1 >= 255){
							garra1 = 254;
						}
						else if (garra1 <= 1){
							garra1 = 0;
						}
						
						if ((PINC & (1 << PINC4)))
						{
							receivedChar = 'A';
							_delay_ms(15);
						}
						
						else if ((PINC & (1 << PINC5)))
						{
							receivedChar = 'B';
							_delay_ms(15);
						}
						
						else if ((PINB & (1 << PINB0))==0)
						{
							receivedChar = 'R';
						}
						
							while ((PINB & (1 << PINB0)) == 0)   //While para evitar sumas indebidas
							{
								_delay_ms(30);    //antirrebote
							}
						
						
						switch(receivedChar)   //Switch para elegir que se desea
						{
							case 'A':
								garra1 = garra1 + 10;
								convertServo2(garra1, channel2A);
								receivedChar = 0;
					
								break;
							
							case 'B':

								garra1 = garra1 - 10;
								convertServo2(garra1, channel2A);
								receivedChar = 0;
								
								break;
							
							case 'R':
								activa ++;
								receivedChar = 0;
								break;
							
						}
						
						
					}
					
				}
				
			else if (activa == 3){
				if(receivedChar != 0 || (PINC & (1 << PINC4)) || (PINC & (1 << PINC5)) || (PINB & (1 << PINB0)) == 0){      //Si la variable que hay en USART es diferente de cero
					
					
					if (brazo >= 255){
						brazo = 254;
					}
					else if (brazo <= 1){
						brazo = 0;
					}
					
					if ((PINC & (1 << PINC4)))
					{
						receivedChar = 'A';
						_delay_ms(25);
					}
					
					else if ((PINC & (1 << PINC5)))
					{
						receivedChar = 'B';
						_delay_ms(25);
					}
					
					else if ((PINB & (1 << PINB0))==0)
					{
						receivedChar = 'R';
					}
					
						while ((PINB & (1 << PINB0)) == 0)   //While para evitar sumas indebidas
						{
							_delay_ms(30);    //antirrebote
						}
					
					switch(receivedChar)   //Switch para elegir que se desea
					{
						case 'B':
						brazo = brazo + 10;
						convertServo(brazo, channelA);
						receivedChar = 0;
						
						break;
						
						case 'A':
						brazo = brazo - 10;
						convertServo(brazo, channelA);
						receivedChar = 0;
						
						break;
						
						case 'R':
						brazo1 = brazo;
						activa ++;
						receivedChar = 0;
						break;
						
					}
					
					
				}
				
			}
			
		else if (activa == 4){
			if(receivedChar != 0 || (PINC & (1 << PINC4)) || (PINC & (1 << PINC5)) || (PINB & (1 << PINB0)) == 0){      //Si la variable que hay en USART es diferente de cero
			
			
				if (brazo1 >= 255){
					brazo1 = 254;
				}
				else if (brazo1 <= 1){
					brazo1 = 0;
				}
				
				if ((PINC & (1 << PINC4)))
				{
					receivedChar = 'A';
					_delay_ms(25);
				}
				
				else if ((PINC & (1 << PINC5)))
				{
					receivedChar = 'B';
					_delay_ms(25);
				}
				
				else if ((PINB & (1 << PINB0))==0)
				{
					receivedChar = 'R';
				}
			
				while ((PINB & (1 << PINB0)) == 0)   //While para evitar sumas indebidas
				{
					_delay_ms(30);    //antirrebote
				}
			
				switch(receivedChar)   //Switch para elegir que se desea
				{
					case 'B':
					brazo1 = brazo1 + 10;
					convertServo(brazo1, channelA);
					receivedChar = 0;
				
					break;
				
					case 'A':
					brazo1 = brazo1 - 10;
					convertServo(brazo1, channelA);
					receivedChar = 0;
				
					break;
				
					case 'R':
					activa ++;
					receivedChar = 0;
					break;
				
				}
			
			
			}
		
		}
		
		else if (activa == 5){
			if(receivedChar != 0 || (PINC & (1 << PINC4)) || (PINC & (1 << PINC5)) || (PINB & (1 << PINB0)) == 0){      //Si la variable que hay en USART es diferente de cero
			
			
				if (codo >= 255){
					codo = 254;
				}
				else if (codo <= 1){
					codo = 0;
				}
				
				if ((PINC & (1 << PINC4)))
				{
					receivedChar = 'A';
					_delay_ms(15);
				}
				
				else if ((PINC & (1 << PINC5)))
				{
					receivedChar = 'B';
					_delay_ms(15);
				}
				
				else if ((PINB & (1 << PINB0))==0)
				{
					receivedChar = 'R';
				}
			
				while ((PINB & (1 << PINB0)) == 0)   //While para evitar sumas indebidas
				{
					_delay_ms(30);    //antirrebote
				}
			
				switch(receivedChar)   //Switch para elegir que se desea
				{
					case 'B':
					codo = codo + 10;
					convertServo(codo, channelB);
					receivedChar = 0;
				
					break;
				
					case 'A':
					codo = codo - 10;
					convertServo(codo, channelB);
					receivedChar = 0;
				
					break;
				
					case 'R':
					activa ++;
					codo1 = codo;
					receivedChar = 0;
					break;
				
				}
			
			
			}
		
		}
		
		else if (activa == 6){
			if(receivedChar != 0 || (PINC & (1 << PINC4)) || (PINC & (1 << PINC5)) || (PINB & (1 << PINB0)) == 0){      //Si la variable que hay en USART es diferente de cero
				
				
				if (codo1 >= 255){
					codo1 = 254;
				}
				else if (codo1 <= 1){
					codo1 = 0;
				}
				
				if ((PINC & (1 << PINC4)))
				{
					receivedChar = 'A';
					_delay_ms(15);
				}
				
				else if ((PINC & (1 << PINC5)))
				{
					receivedChar = 'B';
					_delay_ms(15);
				}
				
				else if ((PINB & (1 << PINB0))==0)
				{
					receivedChar = 'R';
				}
				
					while ((PINB & (1 << PINB0)) == 0)   //While para evitar sumas indebidas
					{
						_delay_ms(30);    //antirrebote
					}
					
				switch(receivedChar)   //Switch para elegir que se desea
				{
					case 'B':
					codo1 = codo1 + 10;
					convertServo(codo1, channelB);
					receivedChar = 0;
					
					break;
					
					case 'A':
					codo1 = codo1 - 10;
					convertServo(codo1, channelB);
					receivedChar = 0;
					
					break;
					
					case 'R':
					activa ++;
					receivedChar = 0;
					break;
					
				}
				
				
			}
			
		}
		
		else if (activa == 7){
			if(receivedChar != 0 || (PINC & (1 << PINC4)) || (PINC & (1 << PINC5)) || (PINB & (1 << PINB0)) == 0){      //Si la variable que hay en USART es diferente de cero
				
				
				if (rota >= 255){
					rota = 254;
				}
				else if (rota <= 1){
					rota = 0;
				}
				
				if ((PINC & (1 << PINC4)))
				{
					receivedChar = 'A';
					_delay_ms(25);
				}
				
				else if ((PINC & (1 << PINC5)))
				{
					receivedChar = 'B';
					_delay_ms(25);
				}
				
				else if ((PINB & (1 << PINB0))==0)
				{
					receivedChar = 'R';
				}
				
					while ((PINB & (1 << PINB0)) == 0)   //While para evitar sumas indebidas
					{
						_delay_ms(30);    //antirrebote
					}
				
				switch(receivedChar)   //Switch para elegir que se desea
				{
					case 'B':
					rota = rota + 5;
					convertServo2(rota, channel2B);
					receivedChar = 0;
					
					break;
					
					case 'A':
					rota = rota - 5;
					convertServo2(rota, channel2B);
					receivedChar = 0;
					
					break;
					
					case 'R':
					activa ++;
					rota1 = rota;
					receivedChar = 0;
					break;
					
				}
				
				
			}
			
		}
		
		else if (activa == 8){
			if(receivedChar != 0 || (PINC & (1 << PINC4)) || (PINC & (1 << PINC5)) || (PINB & (1 << PINB0)) == 0){      //Si la variable que hay en USART es diferente de cero
				
				
				if (rota1 >= 255){
					rota1 = 254;
				}
				else if (rota1 <= 1){
					rota1 = 0;
				}
				
				if ((PINC & (1 << PINC4)))
				{
					receivedChar = 'A';
					_delay_ms(25);
				}
				
				else if ((PINC & (1 << PINC5)))
				{
					receivedChar = 'B';
					_delay_ms(25);
				}
				
				else if ((PINB & (1 << PINB0))==0)
				{
					receivedChar = 'R';
				}
				
					while ((PINB & (1 << PINB0)) == 0)   //While para evitar sumas indebidas
					{
						_delay_ms(30);    //antirrebote
					}
				
				switch(receivedChar)   //Switch para elegir que se desea
				{
					case 'B':
					rota1 = rota1 + 5;
					convertServo2(rota1, channel2B);
					receivedChar = 0;
					
					break;
					
					case 'A':
					rota1 = rota1 - 5;
					convertServo2(rota1, channel2B);
					receivedChar = 0;
					
					break;
					
					case 'R':
					activa ++;
					receivedChar = 0;
					break;
					
				}
				
				
			}
			
		}
		
		else if (activa == 9){
			
			if(receivedChar != 0 || (PINB & (1 << PINB0)) == 0){ 
				activa2 = 0;   //Reiniciar todas las variables 
				activa = 0;
				garra = 0;
				garra1 = 0;
				codo = 0;
				codo1 = 0;
				brazo = 0;
				brazo1 = 0;
				rota1 = 0;
				rota = 0;
				
			 }
				
			         //INICIO DE SECUENCIA AUTOMATICA
			if (garra > garra1){
			for (int w = garra1; w<=garra; w++){
				convertServo2(w, channel2A);
				_delay_ms(10);
			}
			}
			
			else{
				for (int w = garra1; w>=garra; w--){
					convertServo2(w, channel2A);
					_delay_ms(10);
				}
			}
			
			
			if (brazo > brazo1){
				for (int w = brazo1; w<=brazo; w++){
					convertServo(w, channelA);
					_delay_ms(10);
				}
			}
			
			else{
				for (int w = brazo1; w>=brazo; w--){
					convertServo(w, channelA);
					_delay_ms(10);
				}
			}
			
			
		
			if (codo > codo1){
				for (int w = codo1; w<=codo; w++){
					convertServo(w, channelB);
					_delay_ms(10);
				}
			}
			
			else{
				for (int w = codo1; w>=codo; w--){
					convertServo(w, channelB);
					_delay_ms(10);
				}
			}
			
			if (rota > rota1){
				for (int w = rota1; w<=rota; w++){
					convertServo2(w, channel2B);
					_delay_ms(20);
				}
			}
			
			else{
				for (int w = rota1; w>=rota; w--){
					convertServo2(w, channel2B);
					_delay_ms(20);
				}
			}
			
			
			
			//FIN
			if (garra > garra1){
			for (int w = garra; w>=garra1; w--){
				convertServo2(w, channel2A);
				_delay_ms(10);
			}
			}
			else{
				for (int w = garra; w<=garra1; w++){
					convertServo2(w, channel2A);
					_delay_ms(10);
				}
			}
		
			
			if (brazo > brazo1){
				for (int w = brazo; w>=brazo1; w--){
					convertServo(w, channelA);
					_delay_ms(10);
				}
			}
			else{
				for (int w = brazo; w<=brazo1; w++){
					convertServo(w, channelA);
					_delay_ms(10);
				}
			}
			
		
			if (codo > codo1){
				for (int w = codo; w>=codo1; w--){
					convertServo(w, channelB);
					_delay_ms(10);
				}
			}
			else{
				for (int w = codo; w<=codo1; w++){
					convertServo(w, channelB);
					_delay_ms(10);
				}
			}
			
			if (rota > rota1){
				for (int w = rota; w>=rota1; w--){
					convertServo2(w, channel2B);
					_delay_ms(20);
				}
			}
			else{
				for (int w = rota; w<=rota1; w++){
					convertServo2(w, channel2B);
					_delay_ms(20);
				}
			}
			
			
			
		
		}
			break;
	
		case 1: //Si se desea manipular al robot con EEPROM
			PORTD |=  (1 << DDD4) | (1 << DDD5);  //encender los led correspondientes
			if(activa3 == 1){
				PORTD &=  (1 << DDD4) | (1 << DDD5);
			}
			else{
				activa3 = 0;
			}
			
			
			
			
			if ((PINB & (1 << PINB0)) == 0  || receivedChar == 'l')    //Si se presiona el segundo pulsador guardar posicion en EEPROM
			{
				_delay_ms(20);
				sumar ++;
				receivedChar = 0;
				
				      //Verificar que valor tiene la posicion actual y almacenar los valores
				      if (sumar == 1)
				      {
					      EEPROM_write(0,garra);          //Almacenar cada valor de cada servomotor
						  EEPROM_write(1,brazo);
						  EEPROM_write(2,codo);
						  EEPROM_write(3,rota);
				      }
				      
				      else if (sumar == 2)
				      {
						  EEPROM_write(4,garra);
						  EEPROM_write(5,brazo);
						  EEPROM_write(6,codo);
						  EEPROM_write(7,rota);
					      
				      }
				      
				      else if (sumar == 3)
				      {
					      EEPROM_write(8,garra);
					      EEPROM_write(9,brazo);
					      EEPROM_write(10,codo);
					      EEPROM_write(11,rota);
					      
				      }
				      
				      else if (sumar == 4)
				      {
					      sumar = 0;
						   EEPROM_write(12,garra);
						   EEPROM_write(13,brazo);
						   EEPROM_write(14,codo);
						   EEPROM_write(15,rota);
					     
				      }
			}
					     //Verificar que pulsador se presionó y mostrar la posicion grabada en EEPROM
			if(PINC & (1 << PINC4) || receivedChar == 'A'){
				_delay_ms(15);
				 activa2 = 1;
				 int  datoeeprom = EEPROM_read(0);
				 convertServo2(datoeeprom, channel2A);  //garra
				 
				 datoeeprom = EEPROM_read(1);  //brazo
				 convertServo(datoeeprom, channelA);
				 
				  datoeeprom = EEPROM_read(2);  //codo
				  convertServo(datoeeprom, channelB);
				  
				   datoeeprom = EEPROM_read(3);  //rota
				   convertServo2(datoeeprom, channel2B);
				   receivedChar = 0;
			}
			
			if(PINC & (1 << PINC5) || receivedChar == 'B'){
				_delay_ms(15);
				 activa2 = 1;
				 int  datoeeprom = EEPROM_read(4);
				 convertServo2(datoeeprom, channel2A); //garra
				 
				 datoeeprom = EEPROM_read(5);  //brazo
				 convertServo(datoeeprom, channelA);
				 
				 datoeeprom = EEPROM_read(6);  //codo
				 convertServo(datoeeprom, channelB);
				 
				 datoeeprom = EEPROM_read(7);  //rota
				 convertServo2(datoeeprom, channel2B);
				 receivedChar = 0;
			}
			
			if(PINB & (1 << PINB5) || receivedChar == 'C'){
				_delay_ms(15);
				 activa2 = 1;
				 int  datoeeprom = EEPROM_read(8);
				 convertServo2(datoeeprom, channel2A); //garra
				 
				 datoeeprom = EEPROM_read(9);  //brazo
				 convertServo(datoeeprom, channelA);
				 
				 datoeeprom = EEPROM_read(10);  //codo
				 convertServo(datoeeprom, channelB);
				 
				 datoeeprom = EEPROM_read(11);  //rota
				 convertServo2(datoeeprom, channel2B);
				 receivedChar = 0;
			}
			
			if(PIND & (1 << PIND7) || receivedChar == 'D'){
				_delay_ms(15);
				 activa2 = 1;
				 int  datoeeprom = EEPROM_read(12);
				 convertServo2(datoeeprom, channel2A); //garra
				 
				 datoeeprom = EEPROM_read(13);  //brazo
				 convertServo(datoeeprom, channelA);
				 
				 datoeeprom = EEPROM_read(14);  //codo
				 convertServo(datoeeprom, channelB);
				 
				 datoeeprom = EEPROM_read(15);  //rota
				 convertServo2(datoeeprom, channel2B);
				 receivedChar = 0;
			}
	
			
			
			
			
			
			
			while ((PINB & (1 << PINB0)) == 0)   //While para evitar sumas indebidas
			{
				_delay_ms(30);    //antirrebote
			}
			
			
		break;
		
		
		
		
		case 2: //Si se desea manipular al robot desde adafruit
			PORTD |=  (1 << DDD4) | (1 << DDD5) | (1 << DDD6);  //encender los led correspondientes
			activa3 = 1;
			
			if (receivedChar == 'g')      //Observar que servomotor se desea mover
			{
				moverr = 1;
			}
			
		   if (receivedChar == 'a')
			{
				moverr = 2;
			}
			
			if (receivedChar == 'b')
			{
				moverr = 3;
			}
			
			if (receivedChar == 'c')
			{
				moverr = 4;
			}
			
			
			if (moverr == 1)      //Ver que servo se desea mover
			{
				switch(receivedChar){         //Enviar al servo el respectivo angulo 
					case '0':
					convertServo2(0, channel2A);
					break;
					
					case '1':
					convertServo2(10, channel2A);
					break;
					
					case '2':
					convertServo2(20, channel2A);
					break;
					
					case '3':
					convertServo2(30, channel2A);
					break;
					
					case '4':
					convertServo2(40, channel2A);
					break;
					
					case '5':
					convertServo2(50, channel2A);
					break;
					
					case '6':
					convertServo2(60, channel2A);
					break;
					
					case '7':
					convertServo2(70, channel2A);
					break;
					
					case '8':
					convertServo2(80, channel2A);
					break;
					
					case '9':
					convertServo2(90, channel2A);
					break;
					
					case 'A':
					convertServo2(100, channel2A);
					break;
					
					case 'B':
					convertServo2(110, channel2A);
					break;
					
					case 'C':
					convertServo2(120, channel2A);
					break;
					
					case 'D':
					convertServo2(130, channel2A);
					break;
					
					case 'e':
					convertServo2(140, channel2A);
					break;
					
					case 'F':
					convertServo2(150, channel2A);
					break;
					
					case 'G':
					convertServo2(160, channel2A);
					break;
					
					case 'H':
					convertServo2(170, channel2A);
					break;
					
					case 'I':
					convertServo2(180, channel2A);
					break;
					
					case 'J':
					convertServo2(190, channel2A);
					break;
					
					case 'K':
					convertServo2(200, channel2A);
					break;
					
					case 'L':
					convertServo2(210, channel2A);
					break;
					
					case 'M':
					convertServo2(220, channel2A);
					break;
					
					case 'N':
					convertServo2(230, channel2A);
					break;
					
					case 'O':
					convertServo2(240, channel2A);
					break;
					
					case 'P':
					convertServo2(250, channel2A);
					break;
					
					case 'q':
					convertServo2(255, channel2A);
					break;
					
				}
			}
			
		 if (moverr == 2)
			{
				switch(receivedChar){
					case '0':
					convertServo(0, channelB);
					break;
					
					case '1':
					convertServo(10, channelB);
					break;
					
					case '2':
					convertServo(20, channelB);
					break;
					
					case '3':
					convertServo(30, channelB);
					break;
					
					case '4':
					convertServo(40, channelB);
					break;
					
					case '5':
					convertServo(50, channelB);
					break;
					
					case '6':
					convertServo(60, channelB);
					break;
					
					case '7':
					convertServo(70, channelB);
					break;
					
					case '8':
					convertServo(80, channelB);
					break;
					
					case '9':
					convertServo(90, channelB);
					break;
					
					case 'A':
					convertServo(100, channelB);
					break;
					
					case 'B':
					convertServo(110, channelB);
					break;
					
					case 'C':
					convertServo(120, channelB);
					break;
					
					case 'D':
					convertServo(130, channelB);
					break;
					
					case 'e':
					convertServo(140, channelB);
					break;
					
					case 'F':
					convertServo(150, channelB);
					break;
					
					case 'G':
					convertServo(160, channelB);
					break;
					
					case 'H':
					convertServo(170, channelB);
					break;
					
					case 'I':
					convertServo(180, channelB);
					break;
					
					case 'J':
					convertServo(190, channelB);
					break;
					
					case 'K':
					convertServo(200, channelB);
					break;
					
					case 'L':
					convertServo(210, channelB);
					break;
					
					case 'M':
					convertServo(220, channelB);
					break;
					
					case 'N':
					convertServo(230, channelB);
					break;
					
					case 'O':
					convertServo(240, channelB);
					break;
					
					case 'P':
					convertServo(250, channelB);
					break;
					
					case 'q':
					convertServo(255, channelB);
					break;
					
				}
			}
			
		   if (moverr == 3)
			{
				switch(receivedChar){
					case '0':
					convertServo(0, channelA);
					break;
					
					case '1':
					convertServo(10, channelA);
					break;
					
					case '2':
					convertServo(20, channelA);
					break;
					
					case '3':
					convertServo(30, channelA);
					break;
					
					case '4':
					convertServo(40, channelA);
					break;
					
					case '5':
					convertServo(50, channelA);
					break;
					
					case '6':
					convertServo(60, channelA);
					break;
					
					case '7':
					convertServo(70, channelA);
					break;
					
					case '8':
					convertServo(80, channelA);
					break;
					
					case '9':
					convertServo(90, channelA);
					break;
					
					case 'A':
					convertServo(100, channelA);
					break;
					
					case 'B':
					convertServo(110, channelA);
					break;
					
					case 'C':
					convertServo(120, channelA);
					break;
					
					case 'D':
					convertServo(130, channelA);
					break;
					
					case 'e':
					convertServo(140, channelA);
					break;
					
					case 'F':
					convertServo(150, channelA);
					break;
					
					case 'G':
					convertServo(160, channelA);
					break;
					
					case 'H':
					convertServo(170, channelA);
					break;
					
					case 'I':
					convertServo(180, channelA);
					break;
					
					case 'J':
					convertServo(190, channelA);
					break;
					
					case 'K':
					convertServo(200, channelA);
					break;
					
					case 'L':
					convertServo(210, channelA);
					break;
					
					case 'M':
					convertServo(220, channelA);
					break;
					
					case 'N':
					convertServo(230, channelA);
					break;
					
					case 'O':
					convertServo(240, channelA);
					break;
					
					
				}
			}
			
			if (moverr == 4)
			{
				switch(receivedChar){
					case '0':
					convertServo2(0, channel2B);
					break;
					
					case '1':
					convertServo2(10, channel2B);
					break;
					
					case '2':
					convertServo2(20, channel2B);
					break;
					
					case '3':
					convertServo2(30, channel2B);
					break;
					
					case '4':
					convertServo2(40, channel2B);
					break;
					
					case '5':
					convertServo2(50, channel2B);
					break;
					
					case '6':
					convertServo2(60, channel2B);
					break;
					
					case '7':
					convertServo2(70, channel2B);
					break;
					
					case '8':
					convertServo2(80, channel2B);
					break;
					
					case '9':
					convertServo2(90, channel2B);
					break;
					
					case 'A':
					convertServo2(100, channel2B);
					break;
					
					case 'B':
					convertServo2(110, channel2B);
					break;
					
					case 'C':
					convertServo2(120, channel2B);
					break;
					
					case 'D':
					convertServo2(130, channel2B);
					break;
					
					case 'e':
					convertServo2(140, channel2B);
					break;
					
					case 'F':
					convertServo2(150, channel2B);
					break;
					
					case 'G':
					convertServo2(160, channel2B);
					break;
					
					case 'H':
					convertServo2(170, channel2B);
					break;
					
					case 'I':
					convertServo2(180, channel2B);
					break;
					
					case 'J':
					convertServo2(190, channel2B);
					break;
					
					case 'K':
					convertServo2(200, channel2B);
					break;
					
					case 'L':
					convertServo2(210, channel2B);
					break;
					
					case 'M':
					convertServo2(220, channel2B);
					break;
					
					case 'N':
					convertServo2(230, channel2B);
					break;
					
					case 'O':
					convertServo2(240, channel2B);
					break;
					
					case 'P':
					convertServo2(250, channel2B);
					break;
					
					case 'q':
					convertServo2(255, channel2B);
					break;
					
				}
			}
			 
			receivedChar = 0;
	
		break;
		}
	
	
	if (activa2 == 0){
		ADCSRA |= (1 << ADSC);   //Leer puerto de ADC
	}
		_delay_ms(15);   //Velocidad de servo
		
	
		
	}
}


ISR (ADC_vect){
	
	if (activa2 == 0){
		switch (caso){
			case 0:
				ADMUX &= ~((1<<MUX2)|(1<<MUX1)|(1<<MUX0));   //Borrar configuracion actual y poner ADC0
				caso = 1;
				convertServo(ADCH, channelB);   //enviar señal a canal B de timer1
				codo = ADCH;
				break;
				
			case 1:
				ADMUX &= ~((1<<MUX2)|(1<<MUX1)|(1<<MUX0));   //Borrar configuracion actual y poner ADC1
				ADMUX |= (1<<MUX0);
				caso = 2; 
				convertServo(ADCH, channelA);       //enviar señal a canal A de timer1
				brazo = ADCH;
				break;
			
			case 2:
				ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2));
				ADMUX|=(1<<MUX1);
				caso = 3; 
				convertServo2(ADCH, channel2A);
				garra = ADCH;
				break;
			
			case 3:
				ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2));
				ADMUX|=(1<<MUX0)|(1<<MUX1);
				caso = 0;
				convertServo2(ADCH, channel2B);
				rota = ADCH;
				break;
		
		}
	
	}
	ADCSRA |= (1 << ADIF);	//Apagar bandera de interrupcion
}


ISR(USART_RX_vect)
{
	char temp = UDR0;

	
	if (temp != '\n'){  // Si es diferente que enter 
		receivedChar = temp;  // Almacena el carácter recibido
	}
	
	while(!(UCSR0A & (1<<UDRE0)));    //Mientras haya caracteres
	
}

