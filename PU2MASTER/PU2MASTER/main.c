/*
 * PU2MASTER.c
 *
 * Created: 31.03.2019 21:13:05
 * Author : Morozov
 */ 

#include <avr/io.h>
//#define SET_TX_PIN_HIGH()
#ifndef COMPILERS_H
#define COMPILERS_H

/*****************************************************************************/

#ifdef  __ICCAVR__

#include <ioavr.h>
#include <inavr.h>
#include <intrinsics.h>

#define STRINGIFY(a) #a
#define ISR(vect) _Pragma(STRINGIFY(vector = vect))\
__interrupt void vect##_func(void)
#define INLINE _Pragma(STRINGIFY(inline = forced))

#define read_byte_flash(x) (x)
#define read_word_flash(x) (x)
#endif

/*****************************************************************************/

#ifdef  __GNUC__

#ifndef F_CPU
#define F_CPU 8000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define __save_interrupt()       SREG
#define __restore_interrupt(var) SREG = (var)
#define __disable_interrupt()    cli()
#define __enable_interrupt()     sei()
#define __delay_cycles(var)      _delay_us((unsigned int)(var)/(F_CPU/1000000))

#define __flash
#define read_byte_flash(x) pgm_read_byte(&(x))
#define read_word_flash(x) pgm_read_word(&(x))

#define INLINE
#endif

/*****************************************************************************/

#ifdef __CODEVISIONAVR__

#include <io.h>
#include <delay.h>

#define __save_interrupt()       SREG
#define __restore_interrupt(var) SREG = (var)
#define __disable_interrupt()    #asm("cli")
#define __enable_interrupt()     #asm("sei")
#define __delay_cycles(var)      delay_us((unsigned int)(var)/(_MCU_CLOCK_FREQUENCY_/1000000))

#define read_byte_flash(x) (x)
#define read_word_flash(x) (x)

#define ISR(vect) interrupt [vect] void vect##_func(void)
#define INLINE
#endif

/*****************************************************************************/

#include <stdint.h>

#endif //COMPILERS_H





#ifndef SOFT_UART_H
#define SOFT_UART_H

/*___________________________���������_____________________________________*/

/*�������� ������� ��*/
#ifndef F_CPU
#define F_CPU          16000000L
#endif

/*��������� ������� 1L, 8L, 64L, 256L, 1024L*/
#define SUART_PRESCALER   1L

/*�������� ������*/
#define SUART_BAUD_RATE   38400L

/*����������� ��������� ������*/
#define SUART_BUF_SIZE    32

/*��� ���������*/
#define RX_PINX       PIND
#define RX_PORTX      PORTD
#define RX_DDRX       DDRD
#define RX_PIN        7

/*��� �����������*/
#define TX_PORTX      PORTD
#define TX_DDRX       DDRD
#define TX_PIN        6

/*������� ��� ������, ����������� �� �����
�������� ����������� ������ � �������� �����.
��� ����������� � ������� SUART_GetChar()*/
#define SUART_Idle()

/*________________________���������������� �������___________________________*/

void SUART_Init(void);
char SUART_GetChar(void);
void SUART_PutChar(char ch);
void SUART_PutStr(char *str);
void SUART_PutStrFl(char __flash  *str);
void SUART_FlushInBuf(void);
char SUART_Kbhit(void);
void SUART_TurnRxOn(void);
void SUART_TurnRxOff(void);

/*___________________________________________________________________________*/

#endif //SOFT_UART_H







#define TRUE 1
#define FALSE 0

#define RX_NUM_OF_BITS  10
#define TX_NUM_OF_BITS  10
#define NUM_OF_CYCLES   3

static volatile uint8_t inbuf[SUART_BUF_SIZE];
static volatile uint8_t	qin = 0;
static volatile uint8_t	qout = 0;

static volatile uint8_t 	flag_rx_ready;
static uint8_t 	                flag_rx_waiting_for_stop_bit;
static uint8_t 	                flag_rx_off;
static uint8_t 	                rx_mask;
static uint8_t 	                timer_rx_ctr;
static uint8_t 	                bits_left_in_rx;

static volatile uint8_t 	flag_tx_busy;
static volatile uint8_t 	timer_tx_ctr;
static volatile uint8_t 	bits_left_in_tx;
static volatile uint16_t	internal_tx_buffer;

//______________________________________________________
#define get_rx_pin_status() RX_PINX & (1<<RX_PIN)
#define set_tx_pin_high()   TX_PORTX |= (1<<TX_PIN)
#define set_tx_pin_low()    TX_PORTX &= ~(1<<TX_PIN)


#define TIME_VALUE  (F_CPU/(SUART_BAUD_RATE*NUM_OF_CYCLES*SUART_PRESCALER))
#define TIMER_TCNT_VALUE (0xff - (TIME_VALUE - 1))
#define ERROR_VALUE ((F_CPU*100)/(TIME_VALUE*SUART_BAUD_RATE*NUM_OF_CYCLES*SUART_PRESCALER))

#if ((TIME_VALUE > 256)||(TIME_VALUE == 0))
#error "TIMER_TCNT_VALUE is not correct. Change PRESCALER, BAUD_RATE or F_CPU"
#endif

#if ((ERROR_VALUE > 100)||(ERROR_VALUE < 100))
#warning "Baud rate error > 1%"
#endif

#if SUART_PRESCALER==1L
#define CSXX (0<<CS02)|(0<<CS01)|(1<<CS00)
#elif SUART_PRESCALER==8L
#define CSXX (0<<CS02)|(1<<CS01)|(0<<CS00)
#elif SUART_PRESCALER==64L
#define CSXX (0<<CS02)|(1<<CS01)|(1<<CS00)
#elif SUART_PRESCALER==256L
#define CSXX (1<<CS02)|(0<<CS01)|(0<<CS00)
#elif SUART_PRESCALER==1024L
#define CSXX (1<<CS02)|(1<<CS01)|(1<<CS00)
#else
#error "prescaller not correct"
#endif


#ifdef __CODEVISIONAVR__
#define TIM0_OVF_VECTOR   TIM0_OVF
#else
#define TIM0_OVF_VECTOR   TIMER0_OVF_vect
#endif

ISR(TIM0_OVF_VECTOR)
{
	uint8_t   start_bit, flag_in;
	uint8_t rx_data;
	static uint16_t  internal_rx_buffer;
	
	TCNT0 += TIMER_TCNT_VALUE;
	
	/* Transmitter Section */
	if (flag_tx_busy){
		--timer_tx_ctr;
		if (!timer_tx_ctr){
			if (internal_tx_buffer & 0x01){
				set_tx_pin_high();
			}
			else{
				set_tx_pin_low();
			}
			internal_tx_buffer >>= 1;
			timer_tx_ctr = NUM_OF_CYCLES;
			--bits_left_in_tx;
			if (!bits_left_in_tx){
				flag_tx_busy = FALSE;
				set_tx_pin_high();
			}
		}
	}
	/* Receiver Section */
	if (flag_rx_off == FALSE){
		if (flag_rx_waiting_for_stop_bit){
			if ( --timer_rx_ctr == 0 ){
				flag_rx_waiting_for_stop_bit = FALSE;
				flag_rx_ready = FALSE;
				rx_data = (uint8_t)internal_rx_buffer;
				if ( rx_data != 0xC2 ){
					inbuf[qin] = rx_data;
					if ( ++qin >= SUART_BUF_SIZE ){
						qin = 0;
					}
				}
			}
		}
		else{   // rx_test_busy
			if ( flag_rx_ready == FALSE ){
				start_bit = get_rx_pin_status();
				if ( start_bit == 0 ){
					flag_rx_ready = TRUE;
					internal_rx_buffer = 0;
					timer_rx_ctr = NUM_OF_CYCLES + 1;
					bits_left_in_rx = RX_NUM_OF_BITS;
					rx_mask = 1;
				}
			}
			else{	// rx_busy
				if ( --timer_rx_ctr == 0 ){
					timer_rx_ctr = NUM_OF_CYCLES;
					flag_in = get_rx_pin_status();
					if ( flag_in ){
						internal_rx_buffer |= rx_mask;
					}
					rx_mask <<= 1;
					if ( --bits_left_in_rx == 0 ){
						flag_rx_waiting_for_stop_bit = TRUE;
					}
				}
			}
		}
	}
	
}


void SUART_Init(void)
{
	/*������������� ������*/
	flag_tx_busy = FALSE;
	flag_rx_ready = FALSE;
	flag_rx_waiting_for_stop_bit = FALSE;
	flag_rx_off = FALSE;
	
	/*��������� ������*/
	RX_DDRX &= ~(1<<RX_PIN);
	RX_PORTX |= (1<<RX_PIN);
	TX_DDRX |= (1<<TX_PIN);
	TX_PORTX |= (1<<TX_PIN);
	
	/*��������� ������� �0*/
	TCCR0 = CSXX;
	TCNT0 = TIMER_TCNT_VALUE;
	TIMSK |= (1<<TOIE0);
}

char SUART_GetChar(void)
{
	uint8_t tmp;
	char ch;
	
	do{
		tmp = qin;
		while (qout == tmp){
			SUART_Idle();
			tmp = qin;
		}
		ch = inbuf[qout];
		if ( ++qout >= SUART_BUF_SIZE ){
			qout = 0;
		}
	}while ((ch == 0x0A) || (ch == 0xC2));
	
	return ch;
}

void SUART_FlushInBuf(void)
{
	qin = 0;
	qout = 0;
}

char SUART_Kbhit(void)
{
	uint8_t tmp = qout;
	return( qin!=tmp );
}

void SUART_TurnRxOn(void)
{
	flag_rx_off = FALSE;
}

void SUART_TurnRxOff(void)
{
	flag_rx_off = TRUE;
}

void SUART_PutChar(char ch)
{
	while(flag_tx_busy){};
	timer_tx_ctr = NUM_OF_CYCLES;
	bits_left_in_tx = TX_NUM_OF_BITS;
	internal_tx_buffer = ((uint16_t)ch<<1) | 0x200;
	flag_tx_busy = TRUE;
}

void SUART_PutStr(char *str)
{
	char ch;
	while (*str){
		ch = *str++;
		SUART_PutChar(ch);
	}
}


void SUART_PutStrFl(char __flash  *str)
{
	char ch;
	ch = read_byte_flash(*str);
	while (ch) {
		SUART_PutChar(ch);
		str++;
		ch = read_byte_flash(*str);
	}
}






//char test_str[] PROGMEM = "gcc proj ";

int main( void )
{
	char buf;
	
	SUART_Init();
	__enable_interrupt();
	
	SUART_PutStr("soft uart ");
	SUART_PutStrFl(test_str);
	
	while(1){
		buf = SUART_GetChar();
		if (buf == '2'){
			SUART_PutChar('*');
			
		}
	}
	return 0;
}

