//******************************************************************************
// Universidad Del Valle De Guatemala
// IE2023: Electrónica digital 2
// Autor: Samuel Tortola - 22094, Alan Gomez - 22115
// Proyecto: Proyecto 1: Red de sensores
// Hardware: Atmega328p
// Creado: 4/08/2024
// Última modificación: 11/08/2024
//******************************************************************************
  //CODIGO DEL CONTROL, controla los Jostick y los pulsadores de los menús

#define F_CPU 16000000
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <util/twi.h>
#include <string.h>

#include "LCD/LCD.h"
#include "UART/UART.h"
#include "ADC/ADC.h"


#define BUFFER_SIZE 30   //Lista buffer para almacenar la fecha y hora completa y dos partes para almacenar indivual la fecha y hora
#define PART1_SIZE 10
#define PART2_SIZE 15

volatile char receivedChar = 0;    //Variable que almacena el valor del UART
int activa2 = 0, caso = 0, pot1x = 0, pot2y = 0, pot1y = 0, pot2x = 0, activaADC = 1; 
volatile int  activador = 0;

volatile int8_t encoder_value = 0;   //Variable que almacena el valor del encoder
uint8_t last_state = 0;    //Variable de proceso para almacenar temeporalmente el valor del encoder
uint8_t cambioencoder = 0, cambioencoder2 = 0;  //Variable apra cambiar entre aumentar horas o minutos


volatile char receivedBuffer[BUFFER_SIZE]; // Buffer para almacenar la cadena recibida
char part1[PART1_SIZE]; // Buffer para la primera parte
char part2[PART2_SIZE]; // Buffer para la segunda parte
volatile uint8_t bufferIndex = 0; // Índice para el buffer

int hora1 = 0, minuto1 = 0, normal = 0, activador3 = 0, gas = 0;


void setup(void);
void splitBuffer(void);


void setup(void){
	cli();  //Apagar interrupciones
	DDRD = 0xFF;  //Puerto D como salida
	DDRB = 0b00000000;   //PB0, PB1 como entrada de pulsadores, PB2 como CLK del encoder, PB3 como DT del encoder, PB4 como SW del encoder
	PORTB = 0b00011111;		//pull up encendido  
	
	PCMSK0 |= (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(1 << 4); //PCINT0, PCINT1, PCINT2, PCINT3, PCINT4
	PCICR |= (1 << 0); //Mascara de interrupción
	 
	Lcd_Init_4bits();
	initUART9600();  //Iniciar UART
	initADC();   //Iniciar la funcion de ADC
	
	_delay_ms(1000);
	Lcd_Set_Cursor(1,0);       //Menú de inicio
	Lcd_Write_String("  Bienvenido al");
	Lcd_Set_Cursor(2,0);
	Lcd_Write_String("    control");
	_delay_ms(5000);
	Lcd_Clear();
	sei(); //Activar interrupciones
}
	




int main(void)
{
    setup();

	
    while (1) 
    {
		
	
		
		if (activador == 0 && activador3 == 0)
		{
			Lcd_Set_Cursor(1,0);          //Menú de funcionamiento normal, o sea, todo en automático
			Lcd_Write_String(" Funcionamiento");
			Lcd_Set_Cursor(2,3);
			Lcd_Write_String("  normal");
			_delay_ms(4000);
			normal = 1;
			gas = 0;
			Lcd_Clear();
			activador3 = 1;
			

		}
		
		else if (activador == 1)
		{
			normal = 0;
			writeUART('A');
			Lcd_Set_Cursor(1,0);
			Lcd_Write_String("Hora: ");
			Lcd_Write_String(part1);  // Muestra la cadena recibida
			Lcd_Set_Cursor(2,0);
			Lcd_Write_String("Fecha:");
			Lcd_Write_String(part2); // Muestra la cadena recibida
			_delay_ms(1000);
		}
		
		else if (activador == 2)
		{
			Lcd_Set_Cursor(1,0);
			Lcd_Write_String("Control de brazo");  //Menú de control de brazo robótico individual
			Lcd_Set_Cursor(2,3);
			Lcd_Write_String("robotico");
			activaADC = 0;   //Activar lectura de ADC
			writeUART('B');
			_delay_ms(5);
			
			if (pot1x >= 250)         //Verificar que potenciómetro cambio a su valor correcto para enviar el dato
			{
				writeUART('C');   //Enviar el dato de que servomotor se debe de mover
			}
			
			if (pot1x <= 10)
			{
				writeUART('D');
			}
			
			if (pot1y >= 250)
			{
				writeUART('F');
			}
			
			if (pot1y <= 10)
			{
				writeUART('E');
			}
			
			
			if (pot2x >= 230)
			{
				writeUART('G');
			}
			
			
			if (pot2x <= 30)
			{
				writeUART('H');
			}
			
			if (pot2y >= 250)
			{
				writeUART('I');
			}
			
			if (pot2y <= 10)
			{
				writeUART('J');
			}


		}
		
		else if (activador == 3)
		{
			_delay_ms(5);
			Lcd_Set_Cursor(1,0);
			Lcd_Write_String("Control de");  //Menú para mover la máquina completa con las llantas
			Lcd_Write_String(" ");
			Lcd_Write_String(" ");
			Lcd_Write_String(" ");
			Lcd_Write_String(" ");
			Lcd_Write_String(" ");
			Lcd_Write_String(" ");
			Lcd_Set_Cursor(2,1);
			Lcd_Write_String("desplazamiento");
			writeUART('R');
			_delay_ms(5);
			
			if (pot1y >= 250)
			{
				writeUART('E');
			}
			
			if (pot1y <= 10)
			{
				writeUART('F');
			}
			
			
			if (pot2x >= 250)
			{
				writeUART('G');
			}
			
			
			if (pot2x <= 10)
			{
				writeUART('H');
			}
			


		}

		else if (activador == 4)
		{
			_delay_ms(5);
			Lcd_Set_Cursor(1,0);
			Lcd_Write_String("Cambio de");  //Menú para cambiar la hora de activado de la banda 
			Lcd_Write_String(" ");
			Lcd_Write_String(" ");
			Lcd_Write_String(" ");
			Lcd_Write_String(" ");
			Lcd_Write_String(" ");
			Lcd_Write_String(" ");
			Lcd_Set_Cursor(2,0);
			Lcd_Write_String("hora de activado");
			_delay_ms(3000);
			activador = 5;
			Lcd_Clear();
			_delay_ms(50);
			activaADC = 1;
		
		}
		
		else if (activador == 6)
		{
			_delay_ms(5);
			Lcd_Set_Cursor(1,0);
			Lcd_Write_String("Cambio de");  //Menú para cambiar la hora de apagado de la banda
			Lcd_Write_String(" ");
			Lcd_Write_String(" ");
			Lcd_Write_String(" ");
			Lcd_Set_Cursor(2,0);
			Lcd_Write_String("hora de apagado");
			_delay_ms(3000);
			activador = 7;
			Lcd_Clear();
			_delay_ms(50);
			
		}
		
		else if (activador == 7)
		{
			writeUART('Y'); //enviar comando para cambiar la hora de apagado de la banda transportadora
			Lcd_Set_Cursor(1,0);
			Lcd_Write_String("Hora: ");
			Lcd_Write_String(part1);
			Lcd_Set_Cursor(2,0);
			Lcd_Write_String("Minuto: ");
			Lcd_Write_String(part2);
			_delay_ms(100);

		}
		
		else if (activador == 5)
		{
			writeUART('T'); //enviar comando para cambiar la hora de encendido de la banda transportadora
			Lcd_Set_Cursor(1,0);
			Lcd_Write_String("Hora: ");
			Lcd_Write_String(part1);
			Lcd_Set_Cursor(2,0);
			Lcd_Write_String("Minuto: ");
			Lcd_Write_String(part2);
			_delay_ms(100);

		}
		
		
		
		if (normal == 1 && gas == 0)
		{
			writeUART('K');
			
			if (strcmp(part1, "0") == 0) //Compara la cadena que tiene part1, con la cadena 0
			{
				Lcd_Set_Cursor(1,0);
				Lcd_Write_String("Sistema esta ");
				Lcd_Set_Cursor(2,4);
				Lcd_Write_String("apagado");
			}
		
			
			else  if (strcmp(part1, "1") == 0) //Compara la cadena que tiene part1, con la cadena 1
			{
				Lcd_Clear();
				_delay_ms(1000);
				Lcd_Set_Cursor(1,0);
				Lcd_Write_String("Se detecto gas");
				Lcd_Set_Cursor(2,4);
				Lcd_Write_String("              ");
				gas = 1;
				_delay_ms(4000);
			}
			
			else
			{
				Lcd_Set_Cursor(1,0);
				Lcd_Write_String("Altura: ");
				Lcd_Write_String(part1);
				Lcd_Write_String(" cm");
				Lcd_Set_Cursor(2,0);
				Lcd_Write_String("                ");
			}
		
			
			
		
			_delay_ms(100);
		}
		
		
		
		
		if (activaADC == 0)  //Cuando se desea controlar las llantas o el brazo desde el control
		{
			ADCSRA |= (1 << ADSC);   //Leer puerto de ADC
		}

	
		
	
    }
}




ISR(USART_RX_vect)
{
	receivedChar = UDR0; // Almacena el carácter recibido
	
	 if (receivedChar == '\n' || receivedChar == '\r') {
		 // Fin de la cadena, terminar de recibir
		 receivedBuffer[bufferIndex] = '\0'; // Terminar la cadena con null
		 bufferIndex = 0; // Reiniciar el índice del buffer
		 splitBuffer(); // Dividir el buffer en dos partes
		 } 
		 else {
		 if (bufferIndex < BUFFER_SIZE - 1) {
			 receivedBuffer[bufferIndex++] = receivedChar; // Almacenar el carácter en el buffer
		 }
	 }
	
	while(!(UCSR0A & (1<<UDRE0)));    //Mientras haya caracteres
	
}


void splitBuffer(void) {    //Función para dividir la trama de la fecha y la hora
	// Convertir volatile char * a char * para usar con strchr
	char *nonVolatileBuffer = (char *)receivedBuffer;
	
	// Buscar el delimitador
	char *delimiter = strchr(nonVolatileBuffer, '_');
	if (delimiter != NULL) {
		// Calcular la longitud de cada parte
		size_t index = delimiter - nonVolatileBuffer;
		if (index < PART1_SIZE) {
			strncpy(part1, nonVolatileBuffer, index); // Copiar la primera parte
			part1[index] = '\0'; // Terminar la cadena
		}
		strncpy(part2, delimiter + 1, PART2_SIZE - 1); // Copiar la segunda parte
		part2[PART2_SIZE - 1] = '\0'; // Terminar la cadena
		
	}
	
	
	
}



ISR (ADC_vect){
	
	if (activa2 == 0){
		switch (caso){
			case 0:
			ADMUX &= ~((1<<MUX2)|(1<<MUX1)|(1<<MUX0));   
			caso = 1;
			pot1x = ADCH;
			break;
			
			case 1:
			ADMUX &= ~((1<<MUX2)|(1<<MUX1)|(1<<MUX0));   
			ADMUX |= (1<<MUX0);
			caso = 2;
			pot1y = ADCH;
			break;
			
			case 2:
			ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2));
			ADMUX|=(1<<MUX1);
			caso = 3;
			pot2x = ADCH;
			break;
			
			case 3:
			ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2));
			ADMUX|=(1<<MUX0)|(1<<MUX1);
			caso = 0;
			pot2y = ADCH;
			break;
		}
		
	}
	ADCSRA |= (1 << ADIF);	//Apagar bandera de interrupcion
}




ISR(PCINT0_vect){

	if(((PINB) & (1<<0)) == 0){   //Condicional que compara si se presionó el pulsador 1
		_delay_ms(20);  //antirrebote
		
		Lcd_Clear();
		_delay_ms(70);
		activador = activador + 1;
		_delay_ms(10);
		
		if (activador >= 8)
		{
			activador = 0;
			activador3 = 0;
			Lcd_Clear();
		}
		
		
		while ((PINB & (1 << PINB0)) == 0)   //While para evitar mas pulsos
		{
			_delay_ms(10);
		}
	}
	
	
	
	if(((PINB) & (1<<1)) == 0){   //Condicional que compara si se presionó el pulsador  2
		_delay_ms(30);  //antirrebote
		
		activaADC = 1; //Desactivar la lectura de ADC
		activador = 0;
		gas = 0;
		activador3 = 0;
		_delay_ms(70);
		Lcd_Clear();
		_delay_ms(20);
		
		while ((PINB & (1 << PINB1)) == 0)   //While para evitar mas pulsos
		{
			_delay_ms(10);
		}
		
	}
	
	
	
	  if (activador == 5 || activador == 7)
	  {
	  
		 //Lectura de cambios en el encoder
		   uint8_t current_state_clk = PINB & (1 << PINB2);
		   uint8_t current_state_dt = PINB & (1 << PINB3);

		   if (current_state_clk != last_state) { // Detectar cambio de estado en CLK
			  
			   if (current_state_clk == 0) {  // Si CLK ha cambiado de 1 a 0
				   // Si DT es diferente de CLK, entonces es una rotación en sentido horario
				   if (current_state_dt == 0) 
				       {
						   if (cambioencoder == 0)
						   {
							   writeUART('a');
						   }
						   
						   else{
							   writeUART('c');
						   }
					  
					   } 
					   
					   else 
					   {
					     if (cambioencoder == 0)
					     {
						      writeUART('b');
					     }
					     
					     else{
						      writeUART('d');
					     }
					  
				       }
			   }
			   last_state = current_state_clk; // Actualizar el estado anterior de CLK
			   
		   }
		 
		
	 
	 
	 if(((PINB) & (1<<PINB4)) == 0){   //Condicional que compara si se presionó el pulsador  en PB4
		 _delay_ms(30);  //antirrebote
		 
		 if (cambioencoder == 0)
		 {
			 cambioencoder = 1;
		 }
		 
		 else{
			 cambioencoder = 0;
		 }

		 
		 while ((PINB & (1 << PINB4)) == 0)   //While para evitar mas pulsos
		 {
			 _delay_ms(10);
		 }
		 
	       }
		 }
	
	        
}

