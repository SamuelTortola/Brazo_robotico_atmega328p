/*
 * UART.h
 *
 * Created: 4/05/2024 23:12:07
 *  Author: samue
 */ 


#include <avr/io.h>
#include <stdint.h>

#ifndef UART_H_
#define UART_H_

void initUART9600();
void writeTextUART(char * Texto);
void writeUART(char Caracter);


#endif /* UART_H_ */