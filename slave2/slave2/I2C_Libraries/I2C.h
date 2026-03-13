/*
 * I2C.h
 *
 * Created: 2/5/2026 12:14:03 AM
 *  Author: juana
 */ 


#ifndef I2C_H_
#define I2C_H_

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>

//Libreria para configurar el ATMega328 para comunicacion I2C

//Funcion para inicializar I2C Master/Slave
void I2C_init_Master(uint8_t prescaler, unsigned long CLK);
void I2C_init_Slave(uint8_t address);

//Funciones para comunicacion
uint8_t I2C_Start(void);									//Funcion para iniciar conversacion (el Master Habla)
uint8_t I2C_repeatedStart(void);							//Funcion para seguir conversacion (el master escucha)
void I2C_stop(void);										//Funcion para parar comunicacion
uint8_t I2C_write(uint8_t dato);							//Funcion para escribir
uint8_t I2C_read(uint8_t *buffer, uint8_t ack);				//funcion para leer dato



#endif /* I2C_H_ */