//******************************************************************************
// Universidad Del Valle De Guatemala
// IE2023: Programación de Microcontroladores
// Autor: Samuel Tortola - 22094
// Proyecto: Proyecto brazo robotico 
// Hardware: Atmega238P
// Creado: 26/04/2024
//Última modificación: 10/5/2024
//******************************************************************************



#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include "PWM1/PWM1.h"
#include "ADC/ADC.h"
#include "PWM2/PWM2.h"
#include "UART/UART.h"


void setup(void);

int caso = 0, activa = 0,  activa2 = 0, estado = 0, activa3 = 0, sumaa = 0;
int garra = 0, garra1 = 0, brazo = 0, brazo1 = 0, codo = 0, codo1 = 0, rota = 0, rota1 = 0;
volatile char receivedChar = 0, mover = 0;
char anterior[]= {};


void setup(void){
	cli();  //Apagar interrupciones
	
	DDRB |= (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB3); //PB1, PB2, PB3 como salida de servos
	DDRC = 0;  //Puerto C como entrada
	DDRD |= (1 << DDD3) | (1 << DDD4) | (1 << DDD5) | (1 << DDD6);   //PD3 como salida de servo, PD4 - PD6 salida de LEDS

	
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
	{
		if (receivedChar == 'Q'){
			estado = 0;
			activa2 = 0;
			receivedChar = 0;
		}
		else if (receivedChar == 'W'){
			estado = 1;
			activa2 = 0;
			receivedChar = 0;
		}
		else if (receivedChar == 'E'){
			estado = 2;
			activa2 = 1;
			receivedChar = 0;
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
					if(receivedChar != 0){      //Si la variable que hay en USART es diferente de cero
						
						
						if (garra >= 255){
							garra = 254;
						}
						else if (garra <= 1){
							garra = 0;
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
					if(receivedChar != 0){      
						
						
						if (garra1 >= 255){
							garra1 = 254;
						}
						else if (garra1 <= 1){
							garra1 = 0;
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
				if(receivedChar != 0){      //Si la variable que hay en USART es diferente de cero
					
					
					if (brazo >= 255){
						brazo = 254;
					}
					else if (brazo <= 1){
						brazo = 0;
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
			if(receivedChar != 0){      //Si la variable que hay en USART es diferente de cero
			
			
				if (brazo1 >= 255){
					brazo1 = 254;
				}
				else if (brazo1 <= 1){
					brazo1 = 0;
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
			if(receivedChar != 0){      //Si la variable que hay en USART es diferente de cero
			
			
				if (codo >= 255){
					codo = 254;
				}
				else if (codo <= 1){
					codo = 0;
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
			if(receivedChar != 0){      //Si la variable que hay en USART es diferente de cero
				
				
				if (codo1 >= 255){
					codo1 = 254;
				}
				else if (codo1 <= 1){
					codo1 = 0;
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
			if(receivedChar != 0){      //Si la variable que hay en USART es diferente de cero
				
				
				if (rota >= 255){
					rota = 254;
				}
				else if (rota <= 1){
					rota = 0;
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
			if(receivedChar != 0){      //Si la variable que hay en USART es diferente de cero
				
				
				if (rota1 >= 255){
					rota1 = 254;
				}
				else if (rota1 <= 1){
					rota1 = 0;
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
			
			if(receivedChar != 0){ 
				activa2 = 0;
				activa = 0;
			 }
				
			         //INICIO
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
			
			
			
			
		break;
		
		case 2: //Si se desea manipular al robot desde adafruit
			PORTD |=  (1 << DDD4) | (1 << DDD5) | (1 << DDD6);  //encender los led correspondientes
			activa3 = 1;
			
	
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
		
		if (estado == 2)
		{
			anterior[sumaa] = receivedChar;
			
			if (sumaa == 2)
			{
				sumaa = 0;
				UDR0 = anterior[0];
				UDR0 = anterior[1];
				UDR0 = anterior[2];
			}
			sumaa ++;
		}
	
	}
	
	while(!(UCSR0A & (1<<UDRE0)));    //Mientras haya caracteres
	
	
	
	

}

