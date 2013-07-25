// Plus

ReefAngelClass::ReefAngelClass()
{
	DDRJ=B00000110;
	PORTJ=B11010000;
//	PCMSK0 |= 128;
	PCMSK0 |= (1 << PCINT6);
	PCICR |= 1;
}

#include <ReefAngel_1.5_LCD.h>
