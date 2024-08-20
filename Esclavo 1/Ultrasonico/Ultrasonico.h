
#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#ifndef ULTRASONICO_H_
#define ULTRASONICO_H_

void initUltrasonic();
uint16_t measureDistance();




#endif 