
#ifndef PWM1_H_
#define PWM1_H_

#include <avr/io.h>
#include <stdint.h>

//Configuraciones
#define modo 0  //Enviar 1 para señal invertida, 0 para señal no invertida
#define channelA 1
#define channelB 2


void initFastPWM1( uint16_t prescaler);


void channel(uint8_t setChannel, uint8_t inverted);


void topValue(uint16_t top);


void convertServo(uint16_t analogIn, uint8_t selChannel);


#endif