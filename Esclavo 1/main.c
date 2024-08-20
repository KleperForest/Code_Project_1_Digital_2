//******************************************************************************
// Universidad Del Valle De Guatemala
// IE2023: Electrónica digital 2
// Autor: Samuel Tortola - 22094, Alan Gomez - 22115
// Proyecto: Proyecto 1: Red de sensores
// Hardware: Atmega328p
// Creado: 4/08/2024
// Última modificación: 15/08/2024
//******************************************************************************
  //CODIGO DEL ESCLAVO 1
  //Este esclavo controla el brazo robótico y la banda transportadora



#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <util/twi.h>


#include "I2C/I2C.h"
#include "PWM1/PWM1.h"
#include "PWM2/PWM2.h"
#include "UART/UART.h"
#include "Ultrasonico/Ultrasonico.h"


int dato_a_enviarI2C = 0, dato_a_recibirI2C = 0;   //Variables que almacenan los datos a enviar  y recibir por I2C
int codo = 160, garra = 50, brazo = 80, rota = 150;  //Datos de 0 a 255 para colocar angulos de 0 a 180 a cada servo
uint8_t sistema_activado = 0;
char buffer[16];
 
void setup(void);
void setup(void){
	cli();  //Apagar interrupciones
	
	DDRB |= (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB3) | (1 << PORTB4) | ~(1 << PORTB0); //PB1, PB2, PB3 como salida de servos 1,2,3, PB4 como salida de banda transportadora, PB0 entrada sensor de gas
	DDRD |= (1 << DDD3);   //PD3 como salida de servo 4
	//PORTB = 0b00000001;		//pull up encendido en PB0
	
	//channelA, conectado a elevar y bajar garra
	//channelB, conetado a elevar y bajar brazo completo
	//channel2A, conectado a la garra
	//channel2B, conectado a rotar a garra
	
	initFastPWM1(8);  //Iniciar funcion de FASTPWM en timer1
	channel(channelB, modo);
	channel(channelA, modo);
	topValue(29999);   //Valor alto de desbordamiento para 1 a 2 ms que se necesita de servo

	initFastPWM2();  //Iniciar funcion de FASTPWM en timer2
	
	I2C_Config_SLAVE(0x03);   //Iniciar el I2C como esclavo, enviarle su dirección
	initUART9600();  //Iniciar UART
	initUltrasonic(); //Iniciar sensor Ultrasónico
	PORTB = ~(1 << PORTB4);
	
	PCMSK0 |= (1 << 0); //PCINT0
	PCICR |= (1 << 0); //Mascara de interrupción
	
	sei();   //Activar interrupciones
}

int main(void)
{
	setup();

	while(1)
	{
		
		
		
		if (dato_a_recibirI2C != 0)  //Cuando se recibe algo por I2C
		{
			if (dato_a_recibirI2C == 1)        //Verificar el número de servo y aumentar o disminuir su ángulo
			{
				codo = codo + 5;
				if (codo >= 254)
				{
					codo = 255;
				}
			
			}
			
			if (dato_a_recibirI2C == 2)
			{
			
				codo = codo - 5;
				if (codo <= 1)
				{
					codo = 0;
				}
				
			}
			
			if (dato_a_recibirI2C == 4)
			{
				brazo = brazo + 5;
				if (brazo >= 254)
				{
					brazo = 255;
				}
				
			}
			
			if (dato_a_recibirI2C == 3)
			{
				
				brazo = brazo - 5;
				if (brazo <= 1)
				{
					brazo = 0;
				}
				
				
			}
				
			if (dato_a_recibirI2C == 6)
			{
				rota = rota + 2;
				if (rota >= 254)
				{
					rota = 255;
				}
				
			}
			
			if (dato_a_recibirI2C == 5)
			{
				
				rota = rota - 2;
				if (rota <= 1)
				{
					rota = 0;
				}
				
			}
			
			if (dato_a_recibirI2C == 7)
			{
				garra = garra + 7;
				if (garra >= 254)
				{
					garra = 255;
				}
				
			}
			
			if (dato_a_recibirI2C == 8)
			{
				
				garra = garra - 7;
				if (garra <= 1)
				{
					garra = 0;
				}
			
			}
			
			if (dato_a_recibirI2C == 10)
			{
				PORTB = (1 << PORTB4);
				sistema_activado = 1;
			}
			
			if (dato_a_recibirI2C == 11)
			{
				PORTB = ~(1 << PORTB4);
				sistema_activado = 0;
			}
			
			dato_a_recibirI2C = 0;    //Evitar sumas indebidas
		}
		
		
		if (sistema_activado == 1)   //Si el sistema se activo con el RTC
		{
			uint16_t distance = measureDistance();
			dato_a_enviarI2C = distance;
		}
		
		else
		{
			dato_a_enviarI2C = 0;
		}
		
		convertServo2(garra, channel2A);       //Enviar cada dato a los timers para controlar los servos
		convertServo(brazo, channelB);
		convertServo(codo,channelA);
		convertServo2(rota, channel2B);
		_delay_ms(15);  //Pequeño retardo para evitar malos funcionamientos
		
		
	}
}

ISR(TWI_vect){           //Vector de interrupción de I2C
	uint8_t estado;
	
	estado = TWSR & 0xFC;  //Lee el estado de la interfaz
	
	switch(estado){
		case 0x60:
		case 0x70:              //Direccionado con su direccion de esclavo
		TWCR |= (1 << TWINT); //
		break;
		
		case 0x80:
		case 0x90:
		dato_a_recibirI2C = TWDR;  //Recibió el dato, llamada general
		TWCR |= 1 << TWINT; //Borra la bandera TWINT
		break;
		
		case 0xA8: // SLA+R recibido, maestro solicita lectura
		case 0xB8: // Dato transmitido y ACK recibido
		TWDR = dato_a_enviarI2C; // Cargar el dato en el registro de datos*****************
		TWCR |= (1 << TWINT) | (1 << TWEN) | (1 << TWIE)| (1 << TWEA); // Listo para la próxima operación
		
		case 0xC0: // Dato transmitido y NACK recibido
		case 0xC8: // Último dato transmitido y ACK recibido
		TWCR |= (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Listo para la próxima operación
		break;
		
		default:    //Libera el BUS de cualquier errror
		TWCR |= (1 << TWINT) | (1 << TWSTO);
		
	}
	
}




ISR(PCINT0_vect){
	

	if(((PINB) & (1<<0)) == 0){   //Condicional que compara si se detecto gas
		_delay_ms(20);  //antirrebote
	
		dato_a_enviarI2C = 1;
		
	
		while ((PINB & (1 << PINB0)) == 0)   //While para evitar mas pulsos
		{
			_delay_ms(10);
		}
	}
	
	
	else{
		dato_a_enviarI2C =1;
		sistema_activado = 0;
	}
	
	
}

