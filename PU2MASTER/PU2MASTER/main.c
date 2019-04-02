#define F_CPU 8000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#define DELAY 100
//------------------------------------------—
unsigned char i;
int INFO = 7;
int main(void)
{
	DDRB = 0x00;
	DDRD = 0xFF;

	PORTB = 0xFF;
	PORTD = 0xFF;

	while(1){
		if (!(PINB&0b00000100))
		{
			_delay_ms(200);
			for (int i=0;i<1;i++)
			{
				int current_byte = INFO % 10;
				INFO = INFO / 10;

				PORTD = 0b00000000;
				_delay_us(DELAY);
				for (int k = 7; k>-1;k-- )
				{
					if ((current_byte & (1<< i)) == 0){
						PORTD = ~0b00000001;
						_delay_us(DELAY);
						} else{
						PORTD = ~0b00000000;
						_delay_us(DELAY);
					}
				}
				PORTD = 0b00000001;
				_delay_us(DELAY);
			}
		}
	}

	}