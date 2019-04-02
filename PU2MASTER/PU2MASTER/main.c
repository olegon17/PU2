#define F_CPU 8000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#define DELAY 1041//1200 bod
#define DELAYINT 65504//8000000/256=31250	1000000/31250=32	1041/32=32.53125 65 536-32=65504
//------------------------------------------—

//unsigned char i;
int INFO = 87654321;
int current_byte;
int i=0;
int j;

enum {nextbyte=0,startbit=1,databit=2,stopbit=3} state=0;
ISR(TIMER1_OVF_vect)							//nfqvth lkz gjlcxtnf dhtvtyb
{
	TCNT1=DELAYINT;
	switch(state)
	{
		case nextbyte:
		current_byte = INFO % 10;
		INFO = INFO / 10;
		state=startbit;
		break;
		case startbit:
		PORTD = 0b00000000;
		j = 0;
		state=databit;
		break;
		case databit:
		if ((current_byte & (1<< j)) == 0)
		{
			PORTD = 0b00000001;
		}
		else
		{
			PORTD = 0b00000000;
		}
		j++;
		if (j==8)
		{
			state=stopbit;
		}
		break;
		case stopbit:
		PORTD = 0b00000001;
		i++;
		state=nextbyte;
		break;	
	}
	if(i>7)
	{
		cli();
	}
	/*
	if(nextbyte==1)
	{
		current_byte = INFO % 10;
		INFO = INFO / 10;
		i=0;
		nextbyte==1;
	}
	if(startbit==1)
	{
		PORTD = 0b00000000;
		k = 7;
		i++;
	}
	if (databit==1)
	{
		if ((current_byte & (1<< i)) == 0)
		{
			PORTD = 0b00000001;
		}
		else
		{
			PORTD = 0b00000000;
		}
	}
	if (stopbit==1)
	{
		PORTD = 0b00000001;
		k--;
	}
			
		
	*/
}



int main(void)
{
	DDRB = 0x00;
	DDRD = 0xFF;

	PORTB = 0xFF;
	PORTD = 0xFF;


	TCCR1B|=(1<<CS12);					//?anoioa =ie/256
	TIMSK|=(1<<TOIE1);					//?ac?aoaiea i?a?uaaiey ii ia?aiieiaie? oaeia?a/n?ao?eea1//I?e onoaiiaeaiiii aeoa OCIE1B e onoaiiaeaiiii aeoa I ?aaeno?a noaoona ?ac?aoaaony i?a?uaaiea ii ia?aiieiaie? oaeia?a/n?ao?eea1
	TCNT1=34286;						//iaeneioi 16aeo=65 536//Niioaaonoaaiii, anee a CSxx Caienaii 100, oi oaaee?aiea cia?aiey a TCNT1 i?ienoiaeo ia ea?aue 256-ue oaeo i?ioanni?a.

	

	
	
	while(1){
		if (!(PINB&0b00000100))
		{
			sei();
			
		}
	}

	}