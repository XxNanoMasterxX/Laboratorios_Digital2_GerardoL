/*
 * Laboratorio 2.c
 *
 * Created: 22/01/2026 20:09:08
 * Author : laloj
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "C:\Users\laloj\Documents\Laboratorios_Digital2_GerardoL\Laboratorio 2\Laboratorio 2\LCD\LCD.h"

void setup();
void initADC();
void adc_a_string(uint8_t adc, char *str);
void bit8_a_string(uint8_t bit8, char *str);
void initUART();
void wChar(char character);
void wStr(char *strng);

uint8_t contador_ADC = 0;
uint8_t contador_USART;
uint8_t ADCUno;
uint8_t ADCDos;
static uint8_t descartar = 1;
char string_buffer[6];
char bit2string_buffer[4];

int main(void)
{
	 setup();
    /* Replace with your application code */
    while (1) 
    {
		LCD_W_String("S1:");
		adc_a_string(ADCUno, string_buffer);
		LCD_W_String(string_buffer);
		_delay_ms(20);
		LCD_W_String(" S3:");
		bit8_a_string(contador_USART, bit2string_buffer);
		LCD_W_String(bit2string_buffer);
		LCD_Set_Cursor(1,0);
		_delay_ms(20);
		LCD_W_String("S2:");
		adc_a_string(ADCDos, string_buffer);
		LCD_W_String(string_buffer);
		LCD_W_String("   -Ger");
		LCD_W_Char(0b10101111);
		LCD_Set_Cursor(0,0);

    }
}


void setup()
{
	cli();
	
	DDRD = 0b11111100;
	PORTD = 0x00;
	
	DDRB = 0xFF;
	PORTB = 0;
	
	contador_ADC = 0;
	contador_USART = 15;
	
	initLCD8bits();
	initADC();
	initUART();
	LCD_Set_Cursor(0,0);
	
	//LCD_W_String("Hola :D");
	//LCD_Set_Cursor(1,0);
	//LCD_W_String("TMundo");
	//LCD_W_Char('a');
	
	sei();
}

void initADC()
{
	ADMUX = 0;
	ADMUX |= (1 << REFS0); //referencia = avcc
	ADMUX |= (1 << ADLAR);
	ADMUX |= (1 << MUX0); // ACtivando ADC1
	
	ADCSRA = 0;
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADIE);
	ADCSRA |= (1 << ADPS1) | (1 << ADPS1) | (1 << ADPS0);
	
	ADCSRA |= (1 << ADSC);
}

void adc_a_string(uint8_t adc, char *str)
{
	uint16_t vCom = (adc * 500UL) / 255;
	uint8_t vEnt = vCom / 100;
	uint8_t vDec = vCom % 100;
	
	 str[0] = vEnt + '0';
	 str[1] = '.';
	 str[2] = (vDec / 10) + '0';
	 str[3] = (vDec % 10) + '0';
	 str[4] = 'V';
	 str[5] = '\0';
}
void bit8_a_string(uint8_t bit8, char *str)
{
	uint8_t cienes = bit8 / 100;
	uint8_t dieces = (bit8 / 10) % 10;;
	uint8_t unos = bit8 % 10;
	
	str[0] = cienes + '0';
	str[1] = dieces + '0';
	str[2] = unos + '0';
	str[3] = '\0';
}

void initUART()
{
	DDRD |= (1 << 1);
	DDRD &= ~(1 << 0);
	
	UCSR0A = 0;
	
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	UBRR0 = 103; // BAUD 9600
}

void wChar(char character)
{
	while ((UCSR0A & (1 << UDRE0)) == 0);
	UDR0 = character;
}

void wStr(char* strng)
{
	for (uint8_t i = 0; *(strng+i) != '\0'; i++) // (strng+i) <- direccion de un character | *(strng+i) <- lo que esta contenido dentro de la direcion, es decir el character
	{
		wChar(*(strng+i));
	}
}


ISR(ADC_vect)
{
	
	if (descartar)
	{
		descartar = 0;
		ADCSRA |= (1 << ADSC);
		return;
	}
	descartar = 1;
	if (contador_ADC == 0)
	{
		ADCUno = ADCH;
		ADMUX &= 0Xf0;
		ADMUX |= (1 << MUX1);
		contador_ADC = 1;
	}
	else if (contador_ADC == 1)
	{
		ADCDos = ADCH;
		ADMUX &= 0Xf0;
		ADMUX |= (1 << MUX0);
		contador_ADC = 0;
	}
	ADCSRA |= (1 << ADSC);
}

ISR(USART_RX_vect)
{
	char msgIn = UDR0;	
	if (msgIn == '+')
	{
		contador_USART++;
		bit8_a_string(contador_USART, bit2string_buffer);
		wStr(bit2string_buffer);
		wStr("\n");
	}
	else if (msgIn == '-')
	{
		contador_USART--;
		bit8_a_string(contador_USART, bit2string_buffer);
		wStr(bit2string_buffer);
		wStr("\n");
	}
	else if (msgIn == '1')
	{
		adc_a_string(ADCUno, string_buffer);
		wStr("S1: ");
		wStr(string_buffer);
		wStr("\n");
		adc_a_string(ADCDos, string_buffer);
		wStr("S2: ");
		wStr(string_buffer);
		wStr("\n");
	}
}