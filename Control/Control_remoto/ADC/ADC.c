#include "ADC.h"

void initADC(){
	

	ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2));
	DIDR0 |= (1 <<ADC0D);	//desabilitar el A0 como digital
	DIDR0 |= (1<<ADC1D);     //desabilitar el A1 como digital
	DIDR0 |= (1<<ADC2D);     //desabilitar el A2 como digital
	DIDR0 |= (1<<ADC3D);     //desabilitar el A3 como digital

	//JUSTIFICACION IZQUIERDA
	ADMUX |= (1<<ADLAR);
	//REFERENCIA INTERNA
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1 << REFS1);
	// HABILITAR INTERRUPCION
	ADCSRA |= (1<<ADIE);
	//PRESCALER 128
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	//HABILITAR ADC
	ADCSRA |= (1<<ADEN);
}