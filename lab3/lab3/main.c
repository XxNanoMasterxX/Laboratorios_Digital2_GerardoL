/*
 * lab3.c
 *
 * Created: 28/01/2026 16:27:17
 * Author : laloj
 */ 
#define F_CPU 16000000
#include <avr/interrupt.h>

#include <avr/io.h>
#include <stdint.h>
#include "SPI\SPI.h"

void setup();
void initADC();
void initUART();
void bit8_a_string(uint8_t bit8, char *str);
void wChar(char character);
void wStr(char *strng);
uint8_t contador_ADC = 0;
uint8_t ADCUno;
uint8_t ADCDos;
static uint8_t descartar = 1;
char bit2string_buffer[4];


int main(void)
{
	setup();
    /* Replace with your application code */
    while (1) 
    {

    }
}

void setup()
{
	cli();
	initADC();
	initUART();
	SPI_init(1,0,0,0b00000010);
	
	DDRD |= (1<<2);
	PORTD &= ~(1<<PORTD2);

	
	sei();
}

void initADC()
{
	ADMUX = 0;
	ADMUX |= (1 << REFS0); //referencia = avcc
	ADMUX |= (1 << ADLAR);
	ADMUX |= (1 << MUX2) | (1 << MUX1); // ACtivando ADC6
	
	ADCSRA = 0;
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADIE);
	ADCSRA |= (1 << ADPS1) | (1 << ADPS1) | (1 << ADPS0);
	
	ADCSRA |= (1 << ADSC);
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
		ADMUX |= (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // ACtivando ADC7
		contador_ADC = 1;
	}
	else if (contador_ADC == 1)
	{
		ADCDos = ADCH;
		ADMUX &= 0Xf0;
		ADMUX |= (1 << MUX2) | (1 << MUX1); // ACtivando ADC6
		contador_ADC = 0;
	}
	ADCSRA |= (1 << ADSC);
}

ISR(SPI_STC_vect)
{
	uint8_t msgIn = SPDR;
	bit8_a_string(ADCUno, bit2string_buffer);
	wStr(bit2string_buffer);
	wChar(' ');
	bit8_a_string(ADCDos, bit2string_buffer);
	wStr(bit2string_buffer);
	wChar(' ');
	wChar(msgIn);
	wChar(' ');
	if (msgIn == 'a')
	{
		
		SPDR = ADCUno;
	}
	else if (msgIn == 'b')
	{

		SPDR = 0xff;
	}
	
}

ISR(USART_RX_vect)
{
	char msgIn = UDR0;
	if (msgIn == '1')
	{
		bit8_a_string(ADCUno, bit2string_buffer);
		wStr(bit2string_buffer);
		wStr("\n");
		bit8_a_string(ADCDos, bit2string_buffer);
		wStr(bit2string_buffer);
		wStr("\n");
	}
}