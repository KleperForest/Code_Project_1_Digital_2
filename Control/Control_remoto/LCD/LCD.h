
#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>
#include <stdint.h>
#define F_CPU 16000000
#include <util/delay.h>


#define RS PD2
#define E  PD3
#define D4 PD4
#define D5 PD5
#define D6 PD6
#define D7 PD7

void Lcd_Command(unsigned char cmnd);
void Lcd_Char(unsigned char data);
void Lcd_Init_4bits();
void Lcd_Set_Cursor(unsigned char row, unsigned char col);
void Lcd_Clear();
void Lcd_Write_String(char *str);
void LCD_number(int num);


#endif 