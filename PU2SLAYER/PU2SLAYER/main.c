/*
 * PU2SLAYER.c
 *
 * Created: 01.04.2019 19:12:51
 * Author : Morozov
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include "lcd_lib.h"
#define DELAY 130//9600=7680 в 1000000 мкс
/*
#define SET_TX_PIN_HIGH() ( UART_PORT |= ( 1 << UART_TX_PIN ) )
#define SET_TX_PIN_LOW() ( UART_PORT &= ~( 1 << UART_TX_PIN ) )

#define UART_PORT PORTC
#define UART_TX_PIN 0

void UART_Transmit(unsigned char  data){
	UART_TransmitLowBit();//старт бит
	//передаем 8 бит с младдшего
	for(unsigned char i = 0;i < 8; i++)
	{
		if(0 == (data & 0x01))
		{
			SET_TX_PIN_LOW();
			_delay_us(DELAY);
		}
		else
		{
			SET_TX_PIN_HIGH();
			_delay_us(DELAY);
		}
		data >>=1
	}
	SET_TX_PIN_HIGH();//стоп бит
	_delay_us(DELAY);
}
*/
void pritresult(long digit)
{
	/*long *res =&digit;
	short *Hres=(short*) res;
	short Lres=*(Hres+1);
	long str[8];
	sprintf((short*)str,"%X",Lres);
	sprintf((short*)(str+1),"%X",Hres);*/
	/*long *res =&digit;
	short *Hres=(short*) res;
	short Lres=*(Hres+1);
	char str[8];
	for (int i=0;i<8;i++)
	{
		sprintf((char)(str+i),"%X",(char)(res+i));
	}*/
	//long *res =&digit;
	/*
	short *Hres=(short*) &digit;//работает
	short Lres=*(Hres+1);
	long str[2];
	sprintf((short*)str,"%X",Lres);
	sprintf((short*)(str+1),"%X",*Hres);*/
	/*работает
	short *Hres=(short*) &digit;
	long str[2];
	sprintf((short*)str,"%X",*(Hres+1));
	sprintf((short*)(str+1),"%X",*(short*) &digit);
	LCD_SendString(str);
	*/
	short *res=(short*) &digit;
	long str[2];
	for (int i=0;i<2;i++)
	{
		sprintf((short*)(str+i),"%X",*(res+(1-i)));
	}
	LCD_SendString(str);
}

int main(void)
{
	char data=15;
	DDRD = 0b11111110;
	PORTD = 0b00000011;
	LCD_Init();
	long *result;
	
	//LCD_Goto(0,0);
	//pritresult(0x12345678);
	

	
	LCD_Goto(0,0);
    /* Replace with your application code */
	int t=8;
    while (t) 
    {
		if (!(PIND&0x01))
		{
			for(unsigned char i = 0;i < 8; i++)
			{
				if(!(PIND&0x01))
				{
					data |= ( 1 << i );
					_delay_us(DELAY);
				}
				else
				{
					data &= ~( 1 << i );
					_delay_us(DELAY);
				}
				//data <<=1;
			}
			//подготовка вывода
			if(data&0xFF)
			LCD_SendString("yes");
			t--;
			result[t]=data;
		}
    }
	//вывод данных
	pritresult(result);
}

