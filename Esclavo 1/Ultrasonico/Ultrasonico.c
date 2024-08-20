#include "Ultrasonico.h"

void initUltrasonic() {
	DDRD |= (1<<PIND6);  // TRIG como salida
	DDRD &= ~(1<<PIND7); // ECHO como entrada
}



uint16_t measureDistance() {
	// Enviar un pulso de 10us en TRIG
	PORTD &= ~(1<<PIND6);
	_delay_us(2);
	PORTD |= (1<<PIND6);
	_delay_us(10);
	PORTD &= ~(1<<PIND6);
	
	// Esperar por el pulso en ECHO
	while (!(PIND & (1<<PIND7)));
	
	// Medir el ancho del pulso
	uint16_t count = 0;
	while (PIND & (1<<PIND7)) {
		count++;
		_delay_us(1);
	}
	
	// Convertir el tiempo a distancia
	return (count / 58)+1;  // La constante 58 depende de la velocidad del sonido
}