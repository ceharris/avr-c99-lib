/***************************************************************
 * C module for using the Two Wire Interface (TWI) mode of the
 * Universal Serial Interface (USI) in AVR microcontrollers such
 * as the ATtiny85.
 *
 * Adapted from original work by Adam Honse, to remove various
 * compiler warnings, use a more consistent naming, and generally
 * tidy up.
 *
 * @author Carl Harris
 ***************************************************************/

#include <util/delay.h>
#define delay_us(us) (_delay_us(us))

#include "usi_twi_master.h"

#define USISR_TRANSFER_8_BIT 		(0b11110000 | (0x00<<USICNT0))
#define USISR_TRANSFER_1_BIT 		(0b11110000 | (0x0E<<USICNT0))
#define USICR_CLOCK_STROBE_MASK		0b00101011
#define USI_CLOCK_STROBE()		  (USICR = USICR_CLOCK_STROBE_MASK)
#define USI_SET_SDA_OUTPUT()		(DDR_USI |=  (1 << PORT_USI_SDA))
#define USI_SET_SDA_INPUT() 		(DDR_USI &= ~(1 << PORT_USI_SDA))
#define USI_SET_SDA_HIGH()			(PORT_USI |=  (1 << PORT_USI_SDA))
#define USI_SET_SDA_LOW()			  (PORT_USI &= ~(1 << PORT_USI_SDA))
#define USI_SET_SCL_OUTPUT()		(DDR_USI |=  (1 << PORT_USI_SCL))
#define USI_SET_SCL_INPUT() 		(DDR_USI &= ~(1 << PORT_USI_SCL))
#define USI_SET_SCL_HIGH()			(PORT_USI |=  (1 << PORT_USI_SCL))
#define USI_SET_SCL_LOW()			  (PORT_USI &= ~(1 << PORT_USI_SCL))
#define USI_AWAIT_SCL_HIGH()     {while (!(PIN_USI & (1<<PIN_USI_SCL))) {}}

#define USI_I2C_DELAY_HIGH()		(delay_us(I2C_THIGH))
#define USI_I2C_DELAY_LOW()			(delay_us(I2C_TLOW))

typedef enum {
    ADDRESS,
    WRITE,
    READ
} I2C_State;

void twi_master_init(void) {
  DDR_USI  |= (1 << PORT_USI_SDA) | (1 << PORT_USI_SCL);
  PORT_USI |= (1 << PORT_USI_SCL);
  PORT_USI |= (1 << PORT_USI_SDA);
  USIDR = 0xFF;
  USICR = (0 << USISIE)
        | (0 << USIOIE)
        | (1 << USIWM1) | (0 << USIWM0)
        | (1 << USICS1) | (0 << USICS0)
        | (1 << USICLK)
        | (0 << USITC);
  USISR = (1 << USISIF)
        | (1 << USIOIF)
        | (1 << USIPF)
        | (1 << USIDC)
        | (0 << USICNT0);
}

static uint8_t i2c_do_transfer(uint8_t usisr_reg) {
  USISR = usisr_reg;
  do {
    USI_I2C_DELAY_LOW();
    USI_CLOCK_STROBE();								  // positive Edge
    USI_AWAIT_SCL_HIGH();
    USI_I2C_DELAY_HIGH();
    USI_CLOCK_STROBE(); 								// negative Edge
  } while (!(USISR & (1 << USIOIF)));

  USI_I2C_DELAY_LOW();

  return USIDR;
}

int twi_master_transfer(uint8_t *data, size_t length) {
  I2C_State state = ADDRESS;

  /** generate start condition */
  USI_SET_SCL_HIGH();
  USI_AWAIT_SCL_HIGH();

#ifdef I2C_FAST_MODE
  USI_I2C_DELAY_HIGH();
#else
  USI_I2C_DELAY_LOW();
#endif

  USI_SET_SDA_OUTPUT();
  USI_SET_SCL_OUTPUT();
  USI_SET_SDA_LOW();
  USI_I2C_DELAY_HIGH();
  USI_SET_SCL_LOW();
  USI_I2C_DELAY_LOW();
  USI_SET_SDA_HIGH();

  do {
    switch (state) {
      case ADDRESS:
        if (!(*data & 0x01)) {
          state = WRITE;
        }
        else {
          state = READ;
        }
        //Falls through to WRITE to transmit the address byte

      case WRITE:
        USI_SET_SCL_LOW();
        USIDR = *(data);
        data++;
        i2c_do_transfer(USISR_TRANSFER_8_BIT);
        USI_SET_SDA_INPUT();
        if (i2c_do_transfer(USISR_TRANSFER_1_BIT) & 0x01) {
          USI_SET_SCL_HIGH();
          USI_SET_SDA_HIGH();
          return 0;
        }
        USI_SET_SDA_OUTPUT();
        break;

      case READ:
        USI_SET_SDA_INPUT();
        *data = i2c_do_transfer(USISR_TRANSFER_8_BIT);
        data++;
        USI_SET_SDA_OUTPUT();
        if (length == 1) {
          USIDR = 0xFF;			// NACK
        }
        else {
          USIDR = 0x00;			// ACK
        }
        i2c_do_transfer(USISR_TRANSFER_1_BIT);
        break;
    }

  } while (--length);

  USI_SET_SDA_LOW();
  USI_I2C_DELAY_LOW();
  USI_SET_SCL_INPUT();
  USI_AWAIT_SCL_HIGH();

  USI_I2C_DELAY_HIGH();
  USI_SET_SDA_INPUT();
  USI_AWAIT_SCL_HIGH();

  return 1;
}

int twi_master_out(uint8_t address, uint8_t data) {
  uint8_t buf[2];
  buf[0] = address << 1;
  buf[1] = data;
  return twi_master_transfer(buf, sizeof(buf));
}