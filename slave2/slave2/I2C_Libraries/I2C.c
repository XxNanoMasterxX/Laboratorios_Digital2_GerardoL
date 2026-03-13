/*
 * Libreria de comunicacion I2C
 * Created: 2/5/2026 12:05:06 AM
 * Juan Rodriguez - 221593
 * Gerardo Lopez -
 *	Digital 2 - Seccion 11
 */ 
#include "I2C.h"

void I2C_init_Master(uint8_t prescaler, unsigned long CLK){
	//Opciones de prescaler 1, 4,16,64.
	DDRC &= ~((1<<DDC4)|(1<<DDC5));
	switch(prescaler){
		case 1:
		TWSR &= ~((1<<TWPS0)|(1<<TWPS1));
		break;
		case 4:
		TWSR &= ~((1<<TWPS1));
		TWSR |= (1<<TWPS0);
		break;
		case 16:
		TWSR &= ~((1<<TWPS0));
		TWSR |= (1<<TWPS1);
		break;
		case 64:
		TWSR |= (1<<TWPS0)|(1<<TWPS1);
		break;
		default:
		TWSR &= ~((1<<TWPS0)|(1<<TWPS1));
		break;
	}
	TWBR = ((F_CPU/CLK)-16)/(2*prescaler);	//Calcular la velocidad
	TWCR |= (1<<TWEN); //Activar comunicacion I2C
}

void I2C_init_Slave(uint8_t address)
{
	DDRC &= ~((1<<DDC4)|(1<<DDC5));
	
	TWAR = address << 1;
	
	//Se habilita la interfaz, ACK automatico, se habilita la iSR
	
	TWCR = (1<<TWEA)|(1<<TWEN)|(1<<TWIE);
}

//Funcion para iniciar conversacion (el Master Habla)
uint8_t I2C_Start(void){
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//Trabaja en modo master, bandera de interrupcion y condicion de inicio.
	/*IMPORTANTE
		Lo normal con comunicacion I2C es que el maestro este polliando y al esclavo se utilice las interrupciones.
	*/
	while (!(TWCR&(1<<TWINT)));	//Esperamos que ocurra evento con la bandera de TWCR
	
	return ((TWSR & 0xF8)==0x08);	//Nos quedamos con los bits de estado
	/*Porque 0x08? Porque ese es el comando o el codigo de estado que indica que se envio al condicion de inicio.
	  Si se envio la condicion de inicio entonces esta funcion debe regresar un 1.
	*/
}				

//Funcion para seguir conversacion (el master escucha)					
uint8_t I2C_repeatedStart(void){
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//Trabaja en modo master, bandera de interrupcion y condicion de inicio.
	
	while (!(TWCR&(1<<TWINT)));	//Esperamos que ocurra evento con la bandera de TWCR
	
	return ((TWSR & 0xF8)==0x10);	//Nos quedamos con los bits de estado
	/*Porque 0x10? Porque ese es el comando o el codigo de estado que indica que se envio al condicion de inicio.
	  Si se envio la condicion de inicio entonces esta funcion debe regresar un 1.
	*/
	
}	

//Funcion para parar comunicacion			
void I2C_stop(void){
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);	//Inicia la secuencia de parada
	
	while (TWCR & (1<<TWSTO));	//Esperamos que el bit se limpie
}	

//Funcion para escribir									
uint8_t I2C_write(uint8_t dato){
	uint8_t estado;
	
	TWDR=dato;		//Cargo el dato
	TWCR=(1<<TWEN)|(1<<TWINT);	//Inicia la secuencia de envio limpiando la bandera y habilitando la interface
	
	while(!(TWCR&(1<<TWINT)));	//Esperamos al flag TWINT
	estado = TWSR & 0xF8;		//Limpiamos para quedarnos unicamente con los bits de estados
	//Verificar si se le envio una SLA+W con ACK o un dato con ACK
	//Verificar comandos en datasheet o en la presentacion de I2C
	if (estado == 0x18 || estado == 0x28){
		return 1;
	}else {
		return estado;
	}
	
	
}	

//funcion para leer dato			
uint8_t I2C_read(uint8_t *buffer, uint8_t ack){
	uint8_t estado; 
	
	if (ack){
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);			//Habilitamos interface I2C con ack
	}else {
		TWCR = (1<<TWINT)|(1<<TWEN);		//Habilitamos interface I2C sin ack
	}
	//se utiliza el ack obligatoriamente si la interaccion con el slave va a ser mayor a 1 byte sino no es necesario.
	//(Se recomienda siempre usarla)
	
	while (!(TWCR&(1<<TWINT)));			//Esperar el flag
	
	estado = TWSR&0xF8;					//Nos quedamos con los bits de estado nuevamente
	//Verificar si se recibio el dato ya sea con ACK o sin ACK
	if (ack && estado != 0x50) return 0;
	if (ack && estado != 0x58) return 0;
	
	*buffer = TWDR; //obtenemos los resultados en el registro de datos
	return 1;
}	