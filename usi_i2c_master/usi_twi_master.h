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

#ifndef USI_TWI_MASTER_H
#define USI_TWI_MASTER_H

#include <stddef.h>
#include <avr/io.h>

#ifdef I2C_FAST_MODE
//I2C Bus Specification v2.1 FAST mode timing limits
#define I2C_TLOW	1.3
#define I2C_THIGH	0.6
//I2C Bus Specification v2.1 STANDARD mode timing limits
#else
#define I2C_TLOW	4.7
#define I2C_THIGH	4.0
#endif

#if defined (__AVR_ATtiny24__) | \
	defined (__AVR_ATtiny44__) | \
	defined (__AVR_ATtiny84__)
#define DDR_USI			DDRA
#define PORT_USI		PORTA
#define PIN_USI			PINA
#define PORT_USI_SDA	PA6
#define PORT_USI_SCL	PA4
#define PIN_USI_SDA		PINA6
#define PIN_USI_SCL		PINA4
#endif

#if defined(__AVR_ATtiny85__) || \
    defined(__AVR_ATtiny45__) || \
    defined(__AVR_ATtiny25__)
#define DDR_USI			DDRB
#define PORT_USI		PORTB
#define PIN_USI			PINB
#define PORT_USI_SDA	PB0
#define PORT_USI_SCL	PB2
#define PIN_USI_SDA		PINB0
#define PIN_USI_SCL		PINB2
#endif

#if defined(__AVR_AT90Tiny2313__) | \
	defined(__AVR_ATtiny2313__)
#define DDR_USI             DDRB
#define PORT_USI            PORTB
#define PIN_USI             PINB
#define PORT_USI_SDA        PB5
#define PORT_USI_SCL        PB7
#define PIN_USI_SDA         PINB5
#define PIN_USI_SCL         PINB7
#endif

/**
 * Initializes the USI hardware for TWI master operation.
 */
void twi_master_init(void);

/**
 * Transfers data to/from a slave device.
 * @param data the data to be transferred; {@code data[0]} must be encoded as the
 *      I2C slave address (shifted left 1 bit) followed by the R/W bit (read=1)
 * @param length total length of data (including the address byte) to be transferred
 * @return result code
 */
int twi_master_transfer(uint8_t* data, size_t length);

/**
 * Transfers one byte of data from the master to the slave at the given address.
 * @param address I2C slave address (0x0..0x7f)
 * @param data the byte to transfer
 * @return result code
 */
int twi_master_out(uint8_t address, uint8_t data);

#endif /* USI_TWI_MASTER */