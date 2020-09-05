/*
 * GccApplication1.c
 *
 * Created: 12.07.2019 0:59:19
 *  Author: Maxim
 */ 

#define F_CPU 9600000
#include <avr/io.h>
#include "74hc595.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#define HIGH 1
#define LOW 0
#define V_THREE 1
#define V_ZERO_SIX 2
#define V_ZERO 3
#define D_MINUS 1
#define D_PLUS 0
char numbers[] = {
 0b0111111,
 0b0000110,
 0b1011011,
 0b1001111,
 0b1100110,
 0b1101101,
 0b1111101,
 0b0000111,
 0b1111111,
 0b1101111,
};
unsigned char voltage  = 0;
volatile unsigned int number = 0;
/*
 1 - 3.3
 2 - 0.6
 3 - gnd
*/
void pinSetMode(unsigned char pin, unsigned char mode){
	cli();
	if(pin == D_PLUS){
	   if(mode == V_THREE){
		   ShiftDigitalWrite(0, HIGH, 1);
		   ShiftDigitalWrite(1, HIGH, 1);
	   }else if (mode == V_ZERO){
		   ShiftDigitalWrite(0, LOW, 1);
		   ShiftDigitalWrite(1, LOW, 1);
	   }else if (mode == V_ZERO_SIX){
	       ShiftDigitalWrite(0, LOW, 1);
	       ShiftDigitalWrite(1, HIGH, 1);
       }
	   
	}else if(pin == D_MINUS){
	   if(mode == V_THREE){
		   ShiftDigitalWrite(2, HIGH, 1);
		   ShiftDigitalWrite(3, HIGH, 1);
	   }else if (mode == V_ZERO){
		   ShiftDigitalWrite(2, LOW, 1);
		   ShiftDigitalWrite(3, LOW, 1);
	   }else if (mode == V_ZERO_SIX){
		   ShiftDigitalWrite(2, LOW, 1);
		   ShiftDigitalWrite(3, HIGH, 1);
	   }

	}
	strobLatch();
	sei();
}

void set5V(){
	pinSetMode(D_MINUS, V_ZERO);
	pinSetMode(D_PLUS, V_ZERO_SIX);
}
void set9V(){
	pinSetMode(D_MINUS, V_ZERO_SIX);
	pinSetMode(D_PLUS, V_THREE);
}

void set12V(){
	pinSetMode(D_MINUS, V_ZERO_SIX);
	pinSetMode(D_PLUS, V_ZERO_SIX);
}

void set20V(){
	pinSetMode(D_MINUS, V_THREE);
	pinSetMode(D_PLUS, V_THREE);
}

ISR(TIM0_OVF_vect) {
	ShiftDigitalWrite(4, LOW, 1);
	ShiftDigitalWritePort(~numbers[number/10], 0);
	ShiftDigitalWrite(5, HIGH, 1);
	strobLatch();
	
	ShiftDigitalWrite(5, LOW, 1);
	ShiftDigitalWritePort(~numbers[number%10], 0);
	ShiftDigitalWrite(4, HIGH, 1);
	strobLatch();
}

int main(void)
{
	ShiftRegisterInit();
	
	TCCR0B = (1<<CS00) | (1<<CS01);
	TIMSK0 = (1<<TOIE0); 
	TCNT0 = 0x00;
    DDRB &= ~_BV(PB3); 
	PORTB |= _BV(PB3); 
	number = 5;
	set5V();
	_delay_ms(3000);
	sei();
    while(1)
    {
		
	   if((PINB & _BV(PB3)) == 0){
		   _delay_ms(50);         // Устранение дребезга клавиш
		   if((PINB & _BV(PB3)) == 0){
				voltage++;
				if(voltage  == 4){ voltage = 0;}
				while ((PINB & _BV(PB3)) == 0){}
		   }
		   if(voltage == 0){
			   set5V();
			   number = 5;
		   }
		   if(voltage == 1){
			   set9V();
			   number = 9;
		   }
		   if(voltage == 2){
			   set12V();
			   number = 12;
		   }
		   if(voltage == 3){
			   set20V();
			   number = 20;
		   }
		   _delay_ms (500);
	   }
    }
}
