//******************************************************************************
// Universidad Del Valle De Guatemala
// IE2023: Programación de Microcontroladores
// Autor: Samuel Tortola - 22094
// Proyecto: Proyecto brazo robotico 
// Hardware: Atmega238P
// Creado: 26/04/2024
//Última modificación: 26/04/2024
//******************************************************************************

/*Conexiones de servos:
-Servo de garra a pin D11 de Arduino
- 
*/
#define F_CPU 12000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>



#include "PWM1/PWM1.h"
#include "ADC/ADC.h"
#include "PWM2/PWM2.h"


void setup(void);

int caso = 0;

void setup(void){
	cli();  //Apagar interrupciones
	
	DDRB |= (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB3); //PB1, PB2, PB3 como salida de servos
	DDRC = 0;  //Puerto C como entrada
	DDRD |= (1 << DDD3);   //PD3 como salida de servo

	
	initFastPWM1(8);  //Iniciar funcion de FASTPWM en timer1
	channel(channelB, modo);
	channel(channelA, modo);
	topValue(29999);   //Valor alto de desbordamiento para 1 a 2 ms que se necesita de servo
	initADC();   //Iniciar la funcion de ADC
	initFastPWM2();  //Iniciar funcion de FASTPWM en timer1
	
	sei();   //Activar interrupciones

}


int main(void)
{
	setup();
	while (1)
	{
		
		ADCSRA |= (1 << ADSC);   //Leer puerto de ADC
		_delay_ms(10);   //Velocidad de servo
		
	
		
	}
}


ISR (ADC_vect){
	
	switch (caso){
		case 0:
			ADMUX &= ~((1<<MUX2)|(1<<MUX1)|(1<<MUX0));   //Borrar configuracion actual y poner ADC0
			caso = 1;
			convertServo(ADCH, channelB);   //enviar señal a canal B de timer1
			break;
		case 1:
			ADMUX &= ~((1<<MUX2)|(1<<MUX1)|(1<<MUX0));   //Borrar configuracion actual y poner ADC1
			ADMUX |= (1<<MUX0);
			caso = 2; 
			convertServo(ADCH, channelA);       //enviar señal a canal A de timer1
			break;
			
		case 2:
			ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2));
			ADMUX|=(1<<MUX1);
			caso = 3; 
			convertServo2(ADCH, channel2A);
			break;
			
		case 3:
			ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2));
			ADMUX|=(1<<MUX0)|(1<<MUX1);
			caso = 0;
			convertServo2(ADCH, channel2B);
			break;
		
	}
	
	ADCSRA |= (1 << ADIF);	//Apagar bandera de interrupcion
}