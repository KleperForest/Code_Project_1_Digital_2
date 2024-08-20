#include "RTC.h"

#define DS3231_ADDRESS 0x68  // Dirección I2C del DS3231

#define SLA_R(address) ((address << 1) | 1)  // Define la macro SLA_R
#define SLA_W(address) (address << 1)

// Función para leer la fecha y hora del DS3231
void leer_fecha_hora(uint8_t *segundos, uint8_t *minutos, uint8_t *horas, uint8_t *dia, uint8_t *fecha, uint8_t *mes, uint8_t *anio) {
	I2C_inicio();
	I2C_EscByte(SLA_W(0x68));  // Dirección del esclavo con bit de escritura
	I2C_EscByte(0x00);         // Dirección del primer registro (segundos)
	I2C_inicio();
	I2C_EscByte(SLA_R(0x68));  // Dirección del esclavo con bit de lectura

	*segundos = BCD_to_Decimal(I2C_leerACK());
	*minutos = BCD_to_Decimal(I2C_leerACK());
	*horas = BCD_to_Decimal(I2C_leerACK());
	*dia = BCD_to_Decimal(I2C_leerACK());
	*fecha = BCD_to_Decimal(I2C_leerACK());
	*mes = BCD_to_Decimal(I2C_leerACK());
	*anio = BCD_to_Decimal(I2C_leerNACK());

	I2C_STOP();
}

uint8_t BCD_to_Decimal(uint8_t bcd) {
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}


uint8_t I2C_leerACK() {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

uint8_t I2C_leerNACK() {
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

