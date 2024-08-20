
#ifndef RTC_H_
#define RTC_H_

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <util/twi.h>

void leer_fecha_hora(uint8_t *segundos, uint8_t *minutos, uint8_t *horas, uint8_t *dia, uint8_t *fecha, uint8_t *mes, uint8_t *anio);
uint8_t I2C_leerACK();
uint8_t I2C_leerNACK();
uint8_t BCD_to_Decimal(uint8_t bcd);




#endif