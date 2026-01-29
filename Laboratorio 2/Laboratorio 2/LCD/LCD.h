
/*
 * LCD.h
 *
 * Created: 22/01/2026 20:45:24
 *  Author: laloj
 */ 

#ifndef LCD_H_
#define LCD_H_
#define  F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

#define E (1<<PORTB1)

void initLCD8bits(void);
void LCD_Port(char a);
void LCD_CMD(char a);
void LCD_W_Char(char c);
void LCD_W_String(char *a);
void LCD_Shift_Right(void);
void LCD_Shift_Left(void);
void LCD_Set_Cursor(char a, char b);
#endif
