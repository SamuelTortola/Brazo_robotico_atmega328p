
#include <avr/io.h>
#include <stdint.h>


#ifndef EEPROM_H_
#define EEPROM_H_

void EEPROM_write(unsigned int uiAddress, unsigned char ucData);

unsigned char EEPROM_read(unsigned int uiAddress);



#endif 