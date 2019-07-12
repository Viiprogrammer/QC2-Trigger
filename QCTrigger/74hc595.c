#include "74hc595.h"
#define N_ELEMENTS(X) (sizeof(X)/sizeof(*(X)))
char ShiftPORT[] = {0b00000000, 0b00000000};
void ShiftRegisterInit(){
  	DATA_DDR |= (1<<DATA);
	SCK_DDR |= (1<<SCK);
	LATCH_DDR |= (1<<LATCH);
	
  	DATA_PORT &=~ (1 << DATA);
	SCK_PORT &=~ (1 << SCK);
	LATCH_PORT &=~ (1 << LATCH);
	#ifdef USE_HARDWARE_SPI
	 SPCR = ((1<<SPE)|(1<<MSTR));//Включение, режим MASTER
	#endif
}
void ShiftRegisterSend(){
	#ifdef USE_HARDWARE_SPI
	for(unsigned int i = 0; inc_data < sizeof(ShiftPORT); i++){
		SPDR = ShiftPORT[i];
		while(!(SPSR & (1<<SPIF)));
	}
	#endif
	#ifndef USE_HARDWARE_SPI
	for(unsigned int i = 0; i < sizeof(ShiftPORT); i++){
		unsigned char data = ShiftPORT[i];
		for(unsigned int i = 0; i < 8; i++){
			if(data & 0x80){
				DATA_PORT |= (1 << DATA);
			}else{
				DATA_PORT &=~ (1 << DATA);
			}
			SCK_PORT |= (1 << SCK);
			SCK_PORT &=~ (1 << SCK);
			data <<= 1;
		}
	}
	#endif   
	LATCH_PORT |= (1 << LATCH);
	LATCH_PORT &=~ (1 << LATCH);
}
void ShiftDigitalWrite(int pin, int lvl, int number){
  if(lvl){
    ShiftPORT[number] |= (1 << pin);
  }else{
	ShiftPORT[number] &= ~(1 << pin);
  }
  ShiftRegisterSend();
}
void ShiftDigitalWritePort(int port, int number){
    ShiftPORT[number] = port;
	ShiftRegisterSend();
}
char ShiftDigitalGetPort(int number){
	 return ShiftPORT[number];
}