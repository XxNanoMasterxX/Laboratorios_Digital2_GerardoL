/*
 * Lab6Arduino.c
 *
 * Created: 12/03/2026 20:08:54
 * Author : laloj
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

void setup();
void initUART();
void wChar(char character);
void wStr(char* strng);

int main(void)
{
    /* Replace with your application code */
	setup();
    while (1) 
    {
    }
}

void setup()
{
	DDRD &= ~0b11111100;     // PD2–PD7 entradas
	PORTD |= 0b11111100;
	initUART();
	PCICR |= (1<<PCIE2); // habilita grupo PCINT[23:16] (PORTD)

	PCMSK2 |= (1<<PCINT18) | (1<<PCINT19) |
	(1<<PCINT20) | (1<<PCINT21) |
	(1<<PCINT22) | (1<<PCINT23);
	sei();
}


void initUART()
{
		DDRD |= (1 << 1);   // TX
		DDRD &= ~(1 << 0);  // RX

		UCSR0A = (1 << U2X0);  // doble velocidad

		UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);

		UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 bits

		UBRR0 = 16; // 115200
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

ISR(PCINT2_vect)
{
	if (!(PIND & (1<<PORTD2))) wStr("Arriba\r\n"); // Arriba
	if (!(PIND & (1<<PORTD3))) wStr("Abajo\r\n"); // Abajo
	if (!(PIND & (1<<PORTD4))) wStr("Derecha\r\n"); // Derecha
	if (!(PIND & (1<<PORTD5))) wStr("Izquierda\r\n");; // Izquierda
	if (!(PIND & (1<<PORTD6))) wStr("A\r\n"); // Acción A
	if (!(PIND & (1<<PORTD7))) wStr("B\r\n"); // Acción B
}

