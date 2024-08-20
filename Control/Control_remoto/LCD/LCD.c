#include "LCD.h"

void Lcd_Command(unsigned char cmnd) {
	PORTD = (PORTD & 0x0F) | (cmnd & 0xF0); // Enviar parte alta del comando
	PORTD &= ~(1 << RS); // RS = 0, para comando
	PORTD |= (1 << E); // E = 1
	_delay_us(1);
	PORTD &= ~(1 << E); // E = 0
	_delay_us(200);

	PORTD = (PORTD & 0x0F) | (cmnd << 4); // Enviar parte baja del comando
	PORTD |= (1 << E); // E = 1
	_delay_us(1);
	PORTD &= ~(1 << E); // E = 0
	_delay_ms(2);
}

void Lcd_Char(unsigned char data) {
	PORTD = (PORTD & 0x0F) | (data & 0xF0); // Enviar parte alta del dato
	PORTD |= (1 << RS); // RS = 1, para datos
	PORTD |= (1 << E); // E = 1
	_delay_us(1);
	PORTD &= ~(1 << E); // E = 0
	_delay_us(200);

	PORTD = (PORTD & 0x0F) | (data << 4); // Enviar parte baja del dato
	PORTD |= (1 << E); // E = 1
	_delay_us(1);
	PORTD &= ~(1 << E); // E = 0
	_delay_ms(2);
}

void Lcd_Init_4bits() {
	_delay_ms(20); // Esperar a que el LCD inicie
	Lcd_Command(0x02); // Iniciar en modo de 4 bits
	Lcd_Command(0x28); // Configurar LCD en modo de 4 bits, 2 líneas y formato de 5x7
	Lcd_Command(0x0C); // Encender LCD, cursor apagado
	Lcd_Command(0x06); // Modo de incremento automático
	Lcd_Command(0x01); // Limpiar LCD
	_delay_ms(2);
}

void Lcd_Set_Cursor(unsigned char row, unsigned char col) {
	unsigned char pos = 0x80;  // Inicializar con un valor por defecto
	if (row == 1) {
		pos = 0x80 + col;
		} else if (row == 2) {
		pos = 0xC0 + col;
	}
	Lcd_Command(pos);
}

void Lcd_Clear() {
	Lcd_Command(0x01); // Limpiar pantalla
	_delay_ms(2);
}

void Lcd_Write_String(char *str) {
	while (*str) {
		Lcd_Char(*str++);
	}
}

void LCD_number(int num) {
	char buffer[16];
	itoa(num, buffer, 10);  // Convertir el número entero a cadena de caracteres (base 10)
	 if (num < 10) {
		 Lcd_Write_String(" ");  // Sobrescribir el carácter adicional con un espacio en blanco
	 }
	Lcd_Write_String(buffer);     // Enviar la cadena de caracteres al LCD
}