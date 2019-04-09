#define F_CPU 8000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#define DELAY 1041//1200 bod
#define DELAYINT 65532//8000000/256=31250	1000000/31250=32	1041/32=32.53125 65 536-32=65504
//8000000/256=31250	1000000/31250=32	104/32=3,25 65536-3,25=65532,75
//65504 01234567 ($0
/*
—корость передачи,
бод	¬рем€ передачи одного бита, мкс	¬рем€ передачи байта,
мкс
4800	208	2083
9600	104	1042
19200	52	521
38400	26	260
57600	17	174
115200	8,7	87
*/



//------------------------------------------Ч

//unsigned char i;
long INFO = 8;
char current_byte;
int i=0;
int j;
//int b=sizeof(unsigned int);
enum {nextbyte=0,startbit=1,databit=2,stopbit=3} state=0;
ISR(TIMER1_OVF_vect)							//nfqvth lkz gjlcxtnf dhtvtyb
{
	TCNT1=DELAYINT;
	if(i<1)
	{
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
			if ((current_byte & (1<< j)))
			{
				PORTD = 0b00000010;
			}
			else
			{
				PORTD = 0b00000000;
			}
			if (j>7)
			state=stopbit;
			else
			j++;
			break;
			case stopbit:
			PORTD = 0b00000010;
			i++;
			state=nextbyte;
			break;
		}
	}
}



int main(void)
{
	DDRB = 0x00;
	DDRD = 0xFF;

	PORTB = 0xFF;
	PORTD = 0xFF;


	TCCR1B|=(1<<CS12);					//?anoioa =ie/256
	TIMSK|=(1<<TOIE1);					//?ac?aoaiea i?a?uaaiey ii ia?aiieiaie? oaeia?a/n?ao?eea1//I?e onoaiiaeaiiii aeoa OCIE1B e onoaiiaeaiiii aeoa I ?aaeno?a noaoona ?ac?aoaaony i?a?uaaiea ii ia?aiieiaie? oaeia?a/n?ao?eea1
	TCNT1=DELAYINT;						//iaeneioi 16aeo=65 536//Niioaaonoaaiii, anee a CSxx Caienaii 100, oi oaaee?aiea cia?aiey a TCNT1 i?ienoiaeo ia ea?aue 256-ue oaeo i?ioanni?a.

	sei();

	//PORTD = 0x02;
	//PORTB = 0b00100000;//5
	while(1){
		if (!(PINB&0b00000100))
		{
			//sei();
			
		}
	}

}