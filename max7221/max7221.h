#ifndef MAX7221_H
#define MAX7221_H

#include <stdbool.h>
#include <stdint.h>

void max7221_init();
void max7221_config();
void max7221_blank_display();
void max7221_snake_pattern();
void max7221_spin_pattern();
void max7221_blank_digit(uint8_t x);
void max7221_display_hex4(uint8_t x, uint8_t value, bool point);

void max7221_display_uint8(uint8_t x, uint8_t value);
void max7221_display_uint32(uint32_t value);
void max7221_write(uint8_t address, uint8_t data);

#endif //MAX7221_H
