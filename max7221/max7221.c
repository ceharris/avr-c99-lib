#include <avr/pgmspace.h>
#include <util/delay.h>

#include "max7221.h"
#include "spi.h"

#if !defined(MAX7221_DDR) | !defined(MAX7221_PORT) | !defined(MAX7221_MASK)
#define MAX7221_DDR DDRB
#define MAX7221_PORT PORTB
#define MAX7221_MASK _BV(PB2)
#endif

#define MAX7221_TEST_DELAY_MS       1000
#define MAX7221_PATTERN_DELAY_MS    75


const uint8_t PATTERNS[] PROGMEM = {
        0b01111110,
        0b00110000,
        0b01101101,
        0b01111001,
        0b00110011,
        0b01011011,
        0b01011111,
        0b01110000,
        0b01111111,
        0b01111011,
        0b01110111,
        0b00011111,
        0b01001110,
        0b00111101,
        0b01001111,
        0b01000111,
};

void max7221_write(uint8_t address, uint8_t data) {
    // Assert chip select
    MAX7221_PORT &= ~MAX7221_MASK;

    spi_transfer(address);
    spi_transfer(data);

    // Release chip select to load the data
    MAX7221_PORT |= MAX7221_MASK;
}

void max7221_init() {
    // configure the port for the CS pin on the 7219
    MAX7221_DDR |= MAX7221_MASK;
    MAX7221_PORT |= MAX7221_MASK;
}

void max7221_config() {
    // Test display
    max7221_write(0x0F, 0x01);
    _delay_ms(MAX7221_TEST_DELAY_MS);
    max7221_write(0x0F, 0x00);

    // Disable decode
    max7221_write(0x9, 0);

    // Max intensity
    max7221_write(0x0A, 0x0f);

    // All digits
    max7221_write(0x0B, 0x07);

    // No shutdown
    max7221_write(0x0C, 0x01);
}

void max7221_display_hex4(uint8_t x, uint8_t value, bool point) {
    uint8_t pattern = pgm_read_byte(&(PATTERNS[value & 0xf]));
    if (point) {
        pattern |= 0x80;
    }
    max7221_write(8 - x, pattern);
}

void max7221_display_uint8(uint8_t x, uint8_t value) {
    max7221_write(8 - x, pgm_read_byte(&(PATTERNS[value >> 4])));
    max7221_write(8 - x - 1, pgm_read_byte(&(PATTERNS[value & 0xf])));
}

void max7221_display_uint32(uint32_t value) {
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t pattern = pgm_read_byte(&(PATTERNS[value & 0xf]));
        max7221_write(i + 1, pattern);
        value >>= 4;
    }
}

void max7221_blank_digit(uint8_t x) {
    max7221_write(8 - x, 0);
}

void max7221_blank_display() {
    for (int i = 0; i < 8; i++) {
        max7221_write(i + 1, 0);
    }
}

void max7221_snake_pattern() {
    for (int i = 0; i < 8; i++) {
        max7221_write(8 - i, 0b01000000);
        _delay_ms(MAX7221_PATTERN_DELAY_MS);
    }
    max7221_write(1, 0b01100000);
    _delay_ms(MAX7221_PATTERN_DELAY_MS);
    max7221_write(1, 0b01100001);
    _delay_ms(MAX7221_PATTERN_DELAY_MS);
    for (int i = 1; i < 8; i++) {
        max7221_write(i + 1, 0b01000001);
        _delay_ms(MAX7221_PATTERN_DELAY_MS);
    }
    max7221_write(8, 0b01000101);
    _delay_ms(MAX7221_PATTERN_DELAY_MS);
    max7221_write(8, 0b01001101);
    _delay_ms(MAX7221_PATTERN_DELAY_MS);
    for (int i = 1; i < 7; i++) {
        max7221_write(8 - i, 0b01001001);
        _delay_ms(MAX7221_PATTERN_DELAY_MS);
    }
    max7221_write(1, 0b01101001);

    max7221_write(8, 0b00001101);
    for (int i = 1; i < 7; i++) {
        max7221_write(8 - i, 0b00001001);
        _delay_ms(MAX7221_PATTERN_DELAY_MS);
    }
    max7221_write(1, 0b00101001);
    _delay_ms(MAX7221_PATTERN_DELAY_MS);
    max7221_write(1, 0b00001001);
    _delay_ms(MAX7221_PATTERN_DELAY_MS);
    for (int i = 0; i < 7; i++) {
        max7221_write(i + 1, 0b00001000);
        _delay_ms(MAX7221_PATTERN_DELAY_MS);
    }
    max7221_write(8, 0b00001100);
    _delay_ms(MAX7221_PATTERN_DELAY_MS);
    max7221_write(8, 0b00001000);
    _delay_ms(MAX7221_PATTERN_DELAY_MS);
    for (int i = 0; i < 8; i++) {
        max7221_write(8 - i, 0b00000000);
        _delay_ms(MAX7221_PATTERN_DELAY_MS);
    }
}

void max7221_spin_pattern() {
    for (int turn = 0; turn < 8; turn++) {
        uint8_t i = 0x40;
        while (i != 1) {
            for (int j = 0; j < 8; j++) {
                uint8_t pattern = i;
                if (j == 7 - turn) {
                    pattern |= 0x80;
                }
                max7221_write(j + 1, pattern);
            }
            _delay_ms(MAX7221_PATTERN_DELAY_MS);
            i >>= 1;
        }
    }
}

