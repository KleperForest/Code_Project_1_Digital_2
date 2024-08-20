#include "PWM2.h"


int P, a;
uint8_t suma = 0;

void initFastPWM2(){
	TCCR2A = 0;
	TCCR2B = 0;
	
	TCCR2A |= (1<<COM2A1);	//  OC2A NO INVERTIDO
	TCCR2A |= (1<<COM2B1);	//  OC2A NO INVERTIDO
	
	TCCR2A |= (1<<WGM21)|(1<<WGM20)|(1<<WGM22);	//Fast mode
	TCCR2B &= ~(1<<WGM22);
	
	TCCR2B |= (1<<CS22)|(1<<CS21)|(1<<CS20);//Prescaler 1024
	OCR2A = 7;
	OCR0B = 0;
}


void convertServo2(uint16_t analogIn2, uint8_t selChannel2){   //Conversion para servos
	
	switch(selChannel2){
		case 1:
		P = analogIn2;
		
		a = (((P)*(28-7))/(255))+7;     //Recreando la funcion de arduino de map(ADCH,0,255,7,38)
		OCR2A = a;
		
		break;
		
		case 2:
		P = analogIn2;
		a = (((P)*(38-7))/(255))+7;
		OCR2B = a;
		break;
	}
}
