//***************************************************************************
//  File........: lcd_lib.h
//
//  Author(s)...: Pashgan    chipenable.ru
//
//  Target(s)...: ATMega...
//
//  Compiler....: GNU Compiler Collection (GCC) 4.3.2 
//
//  Description.: Драйвер знакосинтезирующего жк дисплея
//
//  Data........: 28.10.09  
//
//***************************************************************************
#ifndef LCD_LIB_H
#define LCD_LIB_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

//порт к которому подключена шина данных ЖКД
#define PORT_DATA PORTA
#define PIN_DATA  PINA
#define DDRX_DATA DDRA

//порт к которому подключены управляющие выводы ЖКД
#define PORT_SIG PORTA
#define PIN_SIG  PINA
#define DDRX_SIG DDRA

//Номера выводов к которым подключены управляющие выводы ЖКД
#define RS 0
#define RW 1
#define EN 2

//#define CHECK_FLAG_BF
#define BUS_4BIT
#define HD44780
//******************************************************************************
//макросы

/*позиционирование курсора*/
#define LCD_Goto(x,y) LCD_WriteCom(((((y)& 1)*0x40)+((x)& 15))|128) 

//прототипы функций
void LCD_Init(void);//инициализация портов и жкд
void LCD_Clear(void);//очистка дисплея
void LCD_WriteData(unsigned char data); //выводит байт данных на жкд
void LCD_WriteCom(unsigned char data); //посылает команду жкд
void LCD_SendStringFlash(const char *str PROGMEM); 
void LCD_SendString(char *str);
#endif