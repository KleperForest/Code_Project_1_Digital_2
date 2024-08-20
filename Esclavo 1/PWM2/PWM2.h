
#ifndef PWM2_H_
#define PWM2_H_

#include <avr/io.h>
#include <stdint.h>



#define channel2A 1
#define channel2B 2

void initFastPWM2();


void convertServo2(uint16_t analogIn2, uint8_t selChannel2);



#endif 