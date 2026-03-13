
/*
 * SPI.c
 *
 * Created: 29/01/2026 19:09:25
 *  Author: laloj
 */ 

/*Librería para configurar el ATmega328P como maestro y esclavo
  Juan Rodriguez
  Carnet 221593
  Universidad del Valle de Guatemala
*/

//Definiciones y librerias
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include "SPI.h"

// Pines de comunicación
// PB3: MOSI, PB4: MISO, PB5: SCK

// Pin de control para esclavo
// PB2: Esclavo 1



void SPI_init(uint8_t ena_inter, uint8_t data_order, uint8_t Master_slave, uint8_t CLOCK ){
	
	//Interrupciones (casi siempre activadas)
	if (ena_inter==1){
		//Interrupciones de SPI activadas
		SPCR |= (1<<SPIE);
	}
	else{
		//Interrupciones deshabilitadas
		SPCR &= ~((1<<SPIE));
	}
	
	//Configurar el orden de data (No se en que influye)
	if (data_order==1){
		SPCR |= (1<<DORD); //first LSB
		}else {
		SPCR &= ~(1<<DORD); //first MSB
	}
	
	//Master/slave selector
	if (Master_slave==1){
		//ATmega sera master
		SPCR |= (1<<MSTR);
		//Configurar los pines para master
		DDRB &= ~(1 << DDB4);  // MISO como entrada
		DDRB |= ((1 << DDB2) | (1 << DDB3) | (1 << DDB5));  // SS, MOSI, y SCK como salidas
		
		PORTB |= ~(1<<PORTB2); //Seleccionar el esclavo 1 para la primera comunicación
		
		}else{
		//ATMega sera esclavo
		SPCR &= ~(1<<MSTR);
		
		//Configurar los pines para slave
		DDRB |= (1 << DDB4);  // MISO como salida
		DDRB &= ~((1 << DDB2) | (1 << DDB3) | (1 << DDB5));  // SS, MOSI, y SCK como entradas
	}
	
	
	
	//Selecionar la velocidad de transmisión con los primeros bits de CLOCK
	if (Master_slave==1){
		
		if ((CLOCK & 0b00001000) == 0b00001000){
			SPCR |= (1<<CPOL); //Polaridad del reloj inicia arriba o en HIGH
			}else{
			SPCR &= ~(1<<CPOL); //Polaridad del reloj inicia en LOW
		}
		
		if ((CLOCK & 0b00010000) == 0b00010000){
			SPCR |= (1<<CPHA); //phase del reloj segundo flanco
			}else{
			SPCR &= ~(1<<CPHA); //Phase del reloj primer flanco
		}
		
		switch((CLOCK&0b00000111)){
			//DIV2
			case 0:
			SPSR |= (1<<SPI2X);
			SPCR &= ~((1<<SPR0)|(1<<SPR1));
			break;
			
			//DIV4
			case 1:
			SPSR &= ~(1<<SPI2X);
			SPCR &= ~((1<<SPR0)|(1<<SPR1));
			break;
			
			//DIV8
			case 2:
			SPSR |= (1<<SPI2X);
			SPCR &= ~((1<<SPR1));
			SPCR |= (1<<SPR0);
			break;
			
			//DIV16
			case 3:
			SPSR &= ~(1<<SPI2X);
			SPCR &= ~((1<<SPR1));
			SPCR |= (1<<SPR0);
			break;
			
			//DIV32
			case 4:
			SPSR |= (1<<SPI2X);
			SPCR &= ~((1<<SPR0));
			SPCR |= (1<<SPR1);
			break;
			
			//DIV64
			case 5:
			SPSR &= ~(1<<SPI2X);
			SPCR &= ~((1<<SPR0));
			SPCR |= (1<<SPR1);
			break;
			
			//DIV128
			case 6:
			SPSR &= ~(1<<SPI2X);
			SPCR |= (1<<SPR0)|(1<<SPR1);
			break;
			
			default:
			break;
		}
		
	}
	
	//Habilitar el modo SPI
	SPCR |= (1<<SPE);
}

//Función para cargar datos al bus y enviarlas al esclavo.
void SPI_write(uint8_t data_bus){
	SPDR = data_bus;
}

void SPI_delay(){
	while (!(SPSR&(1<<SPIF))); //Esperar hasta que la información este completa
}

uint8_t SPI_read(void){
	SPI_delay();
	return (SPDR);
}