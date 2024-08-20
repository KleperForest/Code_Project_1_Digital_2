#include "UART.h"

void initUART9600()
{
	//Paso 1: RX y TX como salida
	DDRD &= ~(1<<DDD0);
	DDRD |= (1<<DDD1);
	
	//Paso 2:Configurar UCSR0A
	UCSR0A = 0;
	
	//Paso 3: configurar UCSR0B donde se habilita ISR de recepcion y se habilita RX y TX
	UCSR0B = 0;
	UCSR0B |= (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0);
	
	//Paso 4: Configurar UCSR0C, asincrono, paridad: none, 1 bit stop, data bits de 8 bits
	UCSR0C = 0;
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);
	
	//Paso 5: Configurar velocidad de baudrate a 9600
	UBRR0 = 103;
	
}


//Funcion de escritura de una cadena de caracteres
void writeTextUART(char * Texto){
	uint8_t o;
	for(o = 0; Texto[o]!= '\0'; o++){
		while(!(UCSR0A & (1<<UDRE0)));
		UDR0 = Texto[o];
	}
}


//Funcion de escritura de un caracter
void writeUART(char Caracter)
{
	while(!(UCSR0A & (1<<UDRE0)));  //hasta que la bandera este en 1
	UDR0 = Caracter;
	
}

void UART_PrintNum(uint16_t num) {
	char buffer[6];
	itoa(num, buffer, 10);
	writeTextUART(buffer);
}

void USART_SendString(char* str) {
	while (*str) {
		writeUART(*str++);
	}
}

