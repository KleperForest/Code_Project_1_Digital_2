
#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>
#include <stdbool.h>
#define F_CPU 16000000


void I2C_Config_SLAVE(uint8_t address);


#endif