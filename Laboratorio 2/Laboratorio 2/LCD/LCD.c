
/*
 * LCD.c
 *
 * Created: 22/01/2026 20:53:33
 *  Author: laloj
 */ 

#include "LCD.h"

void initLCD8bits(void)
{
		PORTB &= ~(1<<PORTB0);   //RS = 0, se le indica que es modo comando
		PORTB &= ~E;   //E = 0
		_delay_ms(20);
		LCD_CMD(0x30);
		_delay_ms(5);
		LCD_CMD(0x30);
		_delay_ms(5);
		LCD_CMD(0x30);
		_delay_ms(10);
		
		LCD_CMD(0x38);
		LCD_CMD(0x0C); 
		LCD_CMD(0x01);
		LCD_CMD(0x06);
}

void LCD_CMD(char a)
{

	//RS = 0; // => RS = 0 // Dato en el puerto lo va int
	PORTB &= ~(1<<PORTB0);
	LCD_Port(a);
	//EN = 1; // => E = 1
	PORTB |= E;
	_delay_ms(4);
	//EN = 0; // => E = 0
	PORTB &= ~E;

}

void LCD_Port(char a)
{
	// Limpiar solo los pines usados
	PORTD &= ~0b11111100;   // PD2–PD7
	PORTB &= ~0b00001100;   // PB2–PB3

	// Cargar D0–D5 en PD2–PD7
	PORTD |= (a & 0b00111111) << 2;

	// Cargar D6–D7 en PB2–PB3
	PORTB |= (a & 0b11000000) >> 4;

}

void LCD_Set_Cursor(char a, char b)
{
	if (a == 0)
	{
		LCD_CMD(0x80 + b);
	}
	else if (a == 1)
	{
		LCD_CMD(0xC0 + b);
	}
}

void LCD_W_Char(char a)
{
	PORTB |= (1<<PORTB0); // RS = 1
	LCD_Port(a);
	PORTB |= E;       // E = 1
	_delay_ms(1);
	PORTB &= ~E;    // E = 0
	_delay_ms(1);
}

void LCD_W_String(char *a)
{
	int i;
	for(i=0; a[i]!='\0'; i++)
	LCD_W_Char(a[i]);
}

void Lcd_Shift_Right()
{
	LCD_CMD(0x1C);
}

void Lcd_Shift_Left()
{
	LCD_CMD(0x18);
}