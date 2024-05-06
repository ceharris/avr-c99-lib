
#ifndef SPI_H
#define SPI_H

void spi_init(void);
void spi_enable(void);
void spi_disable(void);
uint8_t spi_transfer(uint8_t data);

#endif //SPI_H
