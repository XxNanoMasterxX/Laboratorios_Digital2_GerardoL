/*
 * slave2.c
 *
 * Created: 5/02/2026 19:39:57
 * Author : laloj
 */ 
//C:\Users\juana\OneDrive\Documentos\GitHub\Digital_2\Proyecto I\slaveS1\slave2\main.c

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include "I2C_Libraries/I2C.h"

#define S2_Address 0x30

#define TIME_MAX 25000UL //milisegundos 

uint8_t buffer = 0;
uint8_t descartar = 1;
volatile uint8_t ADCUno = 0;
char string_buffer[6];

//Variables para sennsor 
uint16_t tiempo_ticks = 0;
volatile uint8_t echo_state = 0;
volatile uint8_t distancia_map = 0;
uint32_t tiempo_us = 0;

//Funcion de 
//void initADC();
void setup();
void initUART();
void adc_a_string(uint8_t adc, char *str);
void wChar(char character);
void wStr(char* strng);
void Start_sensor();



int main(void)
{
    setup();
	
    while (1) 
    {
		if (buffer == 'R')
		{
			PIND |= (1<<PIND2);
			buffer = 0;
		}
		//adc_a_string(ADCUno, string_buffer);
		//wStr(string_buffer);
		//wChar(TWDR);
		if (!echo_state){
			Start_sensor();
		}
		else
		{
			// Flanco de bajada ? capturar tiempo
			tiempo_ticks = TCNT1;
			// Convertir ticks a microsegundos:
			// 1 tick = 0.5 µs
			tiempo_us = tiempo_ticks / 2;

			// Mapear a 0–255 (máx 25000 µs)
			if (tiempo_us > 25000)
			{
				tiempo_us = 25000;
			}

		}
    }
}



void setup()
{
	cli();
	DDRD |= (1<<DDD2);
	PORTD &= ~(1<<PORTD2);
	
	DDRD |= (1<<DDD7);
	
//*******Configuracion del sensor*******//	

	//Configuración de pines del sensor HYSRF05
	DDRC |= (1 << PORTC1);   // PC1 como salida (Trigger)
	DDRC &= ~(1 << PORTC0);  // PC0 como entrada (Echo)
	PORTC &= ~(1<<PORTC1);	//Cero
	
	 // Habilitar Pin Change Interrupt para PC0
	 PCICR |= (1 << PCIE1);      // Habilita grupo PCINT[14:8]
	 PCMSK1 |= (1 << PCINT8);    // PC0 = PCINT8

	 // Timer1 normal mode
	 TCCR1A = 0;
	 TCCR1B = (1 << CS11);  // Prescaler 8
	 
//*******Configuracion del sensor*******//		
	
	//initADC();
	I2C_init_Slave(S2_Address);
	initUART();
	
	sei();
}

//Funcion para leer distancia con el sensor HY-SRF05
void Start_sensor(){
	//Asegurar que el PORTC1 este apagado
	PORTC &= ~(1<<PORTC1);
	_delay_us(2);
	
	//Enviar el pulso de 10uS para inicializar la lectura.
	PORTC |= (1<<PORTC1);
	_delay_us(10);
	PORTC &= ~(1<<PORTC1);
	
	//Toca esperar el pulso de ECHO
}

/*
void initADC()
{
	ADMUX = 0;
	ADMUX |= (1 << REFS0); //referencia = avcc
	ADMUX |= (1 << ADLAR);
	ADMUX |= (1<<MUX2)|(1<<MUX1); //Activando el ADC6
	ADCSRA = 0;
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADIE);
	ADCSRA |= (1 << ADPS1) | (1 << ADPS1) | (1 << ADPS0);
	
	ADCSRA |= (1 << ADSC);
}
*/
void initUART()
{
	DDRD |= (1 << 1);
	DDRD &= ~(1 << 0);
	
	UCSR0A = 0;
	
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	UBRR0 = 103; // BAUD 9600
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

/*
ISR(ADC_vect)
{
	
	if (descartar)
	{
		descartar = 0;
		ADCSRA |= (1 << ADSC);
		return;
	}
	descartar = 1;
	ADCUno = ADCH;
	ADCSRA |= (1 << ADSC);
}
*/

ISR(PCINT1_vect) {
	PORTD |= (1<<PORTD7);

	if (PINC & (1 << PORTC0)) {
		// Flanco de subida ? iniciar timer
		TCNT1 = 0;
		echo_state = 1;
	}
	if (echo_state) {
		distancia_map = (tiempo_us * 255UL) / TIME_MAX;
		echo_state = 0;
		tiempo_ticks = 0;
		tiempo_us = 0;
	}
}

ISR(TWI_vect){
	uint8_t estado = TWSR & 0xFC; // Nos quedamos unicamente con los bits de estado TWI Status
	switch(estado){
		/*******************************/
		// Slave debe recibir dato
		/*******************************/
		case 0x60:
		case 0x70: // General Call
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		//wStr("General Call");
		break;

		case 0x80:
		case 0x90: // Dato recibido General Call, ACK enviado
		buffer = TWDR;
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		//wStr("Dato recibido General Call, ACK enviado");
		break;

		/*******************************/
		// Slave debe transmitir dato
		/*******************************/
		case 0xA8:
		case 0xB8: // Dato transmitido, ACK recibido
		TWDR = distancia_map;   // Dato a enviar
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		//wStr("Dato transmitido, ACK recibido");
		break;
		
		case 0xC0:
		case 0xC8: // Ultimo dato transmitido
		TWCR = 0;
		TWCR = (1<<TWEN)|(1<<TWEA)|(1<<TWIE);
		//wStr("Ultimo dato transmitido");
		break;

		case 0xA0: // STOP o repeated START recibido
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		//wStr("STOP o repeated START recibido");
		break;
		
		default:
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		//wStr("default");
		break;
	}
}