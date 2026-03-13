
/*
 * SPI.h
 *
 * Created: 29/01/2026 19:09:35
 *  Author: laloj
 */ 

#ifndef INCFILE1_H_
#define INCFILE1_H_

void SPI_init(uint8_t ena_inter, uint8_t data_order, uint8_t Master_slave, uint8_t CLOCK);
void SPI_write(uint8_t data_bus);
void SPI_delay();
uint8_t SPI_read(void);

#endif /* INCFILE1_H_ */