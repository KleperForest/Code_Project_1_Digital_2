//******************************************************************************
// Universidad Del Valle De Guatemala
// IE2023: Electrónica digital 2
// Autor: Samuel Tortola - 22094, Alan Gomez - 22115
// Proyecto: Proyecto 1: Red de sensores
// Hardware: Atmega328p
// Creado: 4/08/2024
// Última modificación: 11/08/2024
//******************************************************************************
  //CODIGO DEL MAESTRO, controla a los esclavos  y sensores I2C

#define F_CPU 16000000
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <util/twi.h>

#include "I2C/I2C.h"  // Incluir librería de I2C
#include "UART/UART.h"   //Incluir libreria de UART
#include "RTC/RTC.h"


#define ESCLAVO1 0x03  // Dirección del esclavo 1
#define ESCLAVO2 0x02  // Dirección del esclavo 2
#define ESCLAVO3 0x04  //Dirección del ESP32

// Sensores mendiante I2C Presion BMP280: 0x76 y Temp: 0x38

volatile char receivedChar = 0;    //Variable que almacena el valor del UART
uint8_t segundos, minutos, horas, dia, fecha, mes, anio, brazo = 0;
int  horaencendido = 9, minutoencendido = 2, horaapagado = 0, minutoapagado = 0;
int dato1;
 char buffer[16];
 
 
void setup(void);
void setup(void){
	cli();  //Apagar interrupciones
	DDRD = 0xFF;  //Puerto D como salida
	DDRC = 0x03;  //Puerto C como salida
	
	DDRB = 0b00000000; //Entrada sensores infrarrojos
	PORTB = 0b00000011;		//pull up encendido en PB0 y PB1 
	
	PCMSK0 |= (1 << 0)|(1 << 1); //PCINT0, PCINT1
	PCICR |= (1 << 0); //Mascara de interrupción
	
	initUART9600();  //Iniciar UART
    I2C_Config_MASTER(4, 200000);  //Configurar  el atmega328P como maestro de I2C
	

	sei(); //Activar interrupciones
}


int main(void)
{
    setup();
    while (1) 
    {
		
		// Leer la fecha y hora del RTC DS3231
		leer_fecha_hora(&segundos, &minutos, &horas, &dia, &fecha, &mes, &anio);
		
		  //Verificar la hora de encendido y apagado del sistema 
		if (horaencendido == horas)
		{
			if (minutoencendido == minutos)
			{
				I2C_esclavo(10,ESCLAVO1);
			}
		}
		
		if (horaapagado == horas)
		{
			if (minutoapagado == minutos)
			{
				I2C_esclavo(11, ESCLAVO1);
			}
		}
		
		
		
		
		 
		if (receivedChar != 0)   //Cada vez que se tiene una entrada por UART
		{
			if (receivedChar == 'A')  //Si se requiere ver la hora y fecha actual del sistema
			{
				// Imprimir la hora y la fecha
				//HORA:
				UART_PrintNum(horas);
				writeUART(':');
				UART_PrintNum(minutos);
				writeUART(':');
				UART_PrintNum(segundos);
				writeTextUART(" ");
				writeTextUART("_");    //Separador de fecha y hora
				//FECHA
				UART_PrintNum(fecha);
				writeUART('/');
				UART_PrintNum(mes);
				writeUART('/');
				UART_PrintNum(anio + 2000);  // Asumiendo que el año es en formato 2 dígitos y se quiere imprimir en formato 4 dígitos
				writeTextUART("\n");
			}
			
			if (receivedChar == 'B')  //Si se requiere manejar el brazo robotico individual
			{
				brazo = 1;
			}
			
			if (brazo == 1)    //Si se activa la función de manejar el brazo con los jostick
			{
				if (receivedChar == 'C')
				{
					I2C_esclavo(1,ESCLAVO1);
				
					
				}
				
				if (receivedChar == 'D')
				{
					I2C_esclavo(2,ESCLAVO1);
					
					
				}
				
				if (receivedChar == 'E')
				{
					I2C_esclavo(3,ESCLAVO1);
				}
				
				if (receivedChar == 'F')
				{
					
					I2C_esclavo(4,ESCLAVO1);
				}
				
				
				if (receivedChar == 'G')
				{
					I2C_esclavo(5,ESCLAVO1);
				}
				
				
				if (receivedChar == 'H')
				{
					I2C_esclavo(6,ESCLAVO1);
				}
				
				if (receivedChar == 'I')
				{
					I2C_esclavo(7,ESCLAVO1);
				}
				
				if (receivedChar == 'J')
				{
					I2C_esclavo(8,ESCLAVO1);
				}
			}
			
			if (receivedChar == 'R') //Si se desea manipular las llantas de la máquina
			{
				brazo = 2;     //Desactivar manejar el brazo de manera individual y colocar las llantas
				
			}
			
			if (brazo == 2)
			{
				if (receivedChar == 'E')
				{
					I2C_esclavo(1,ESCLAVO2);
				}
				
				if (receivedChar == 'F')
				{
					
					I2C_esclavo(2,ESCLAVO2);
				}
				
				
				if (receivedChar == 'G')
				{
					I2C_esclavo(3,ESCLAVO2);
				}
				
				
				if (receivedChar == 'H')
				{
					I2C_esclavo(4,ESCLAVO2);
				}
			}
			
			if (receivedChar == 'T') //Si se desea cambiar la hora de encendido de la banda transportadora
			{
				brazo = 3;     //Activar cambio de hora y minuto
				_delay_ms(5);
				UART_PrintNum(horaencendido);
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART("_");    //Separador de hora y minuto
				UART_PrintNum(minutoencendido);
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART("\n");
				_delay_ms(5);
			}
			
			if (brazo == 3)  //Si se desea cambiar la hora de encendido de la banda
			{
				if (receivedChar == 'a')
				{
					horaencendido++;
				}
				if (receivedChar == 'b')
				{
					horaencendido--;
				}
				
				if (receivedChar == 'c')
				{
					minutoencendido++;
				}
				if (receivedChar == 'd')
				{
					minutoencendido--;
				}
				
				if (horaencendido >= 23)
				{
					horaencendido = 23;
				}
				
				if (horaencendido <= 0)
				{
					horaencendido = 0;
				}
				
				if (minutoencendido >= 59)
				{
					minutoencendido = 59;
				}
				
				if (minutoencendido <= 0)
				{
					minutoencendido = 0;
				}
				
				
			}
			
			
			if (receivedChar == 'Y') //Si se desea cambiar la hora de apagado de la banda transportadora
			{
				brazo = 4;     //Activar cambio de hora y minuto
				_delay_ms(5);
				UART_PrintNum(horaapagado);
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART("_");    //Separador de hora y minuto
				UART_PrintNum(minutoapagado);
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART(" ");
				writeTextUART("\n");
				_delay_ms(5);
			}
			
			if (brazo == 4)  //Si se desea cambiar la hora de apagado de la banda
			{
				if (receivedChar == 'a')
				{
					horaapagado++;
				}
				if (receivedChar == 'b')
				{
					horaapagado--;
				}
				
				if (receivedChar == 'c')
				{
					minutoapagado++;
				}
				if (receivedChar == 'd')
				{
					minutoapagado--;
				}
				
				if (horaapagado >= 23)
				{
					horaapagado = 23;
				}
				
				if (horaapagado <= 0)
				{
					horaapagado = 0;
				}
				
				if (minutoapagado >= 59)
				{
					minutoapagado = 59;
				}
				
				if (minutoapagado <= 0)
				{
					minutoapagado = 0;
				}
				
				
			}
			
			if (receivedChar == 'K')
			{
				 dato1 = I2C_leer_dato(ESCLAVO1, &dato1);   //Leer el dato del esclavo 1
				UART_PrintNum(dato1);
				 writeTextUART("_");    //Separador 
				 writeTextUART("\n");
				 
			}
			
			receivedChar = 0;
		}
		


		 _delay_ms(10);  //Pequeño retardo para evitar malos funcionamientos
    }
}



ISR(USART_RX_vect)
{
	receivedChar = UDR0; // Almacena el carácter recibido
	
	while(!(UCSR0A & (1<<UDRE0)));    //Mientras haya caracteres
	
}


ISR(PCINT0_vect){
	if(((PINB) & (1<<PINB0)) == 0){   //Condicional que compara si se detecto una caja
		_delay_ms(20);  //antirrebote
		
		I2C_esclavo(12,ESCLAVO1);
	UART_PrintNum(2);
		
		while ((PINB & (1 << PINB0)) == 0)   //While para evitar mas pulsos
		{
			_delay_ms(10);
		}
	}
	
	
	if(((PINB) & (1<<PINB1)) == 0){   //Condicional que compara si se detecto una caja
		_delay_ms(20);  //antirrebote
		
		I2C_esclavo(13,ESCLAVO1);
		UART_PrintNum(1);
		while ((PINB & (1 << PINB1)) == 0)   //While para evitar mas pulsos
		{
			_delay_ms(10);
		}
	}
	

}



