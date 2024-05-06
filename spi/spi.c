
#include <avr/io.h>

#include "spi.h"

#define DDR_SPI  DDRB
#define MOSI _BV(PB3)
#define MISO _BV(PB4)
#define SCLK _BV(PB5)

void spi_init(void) {
  DDR_SPI &= ~MISO;
  DDR_SPI |= MOSI | SCLK;
}

void spi_enable(void) {
  SPCR = _BV(SPE) | _BV(MSTR);
}

void spi_disable(void) {
  SPCR &= ~(_BV(SPE) | _BV(MSTR) | _BV(SPR0));
}

uint8_t spi_transfer(uint8_t data) {
  SPDR = data;
  while (!(SPSR & _BV(SPIF))) {
    ; /* wait for the byte to be transmitted */
  }
  data = SPDR;
  return data;
}

