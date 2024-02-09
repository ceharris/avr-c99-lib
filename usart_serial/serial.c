#ifdef SERIAL_PRINTF
#include <stdio.h>
#include <stdarg.h>
#endif /* SERIAL_PRINTF */
#include <avr/io.h>

#include "serial.h"

#ifndef SERIAL_PRINTF_SIZE
#define SERIAL_PRINTF_SIZE 128    // buffer size used by serial_printf
#endif

#ifndef SERIAL_BAUD
#define SERIAL_BAUD 38400         // works well with common AVR clock sources and speeds
#define SELECTED_UBRR (F_CPU/16/SERIAL_BAUD - 1)
#endif

void serial_init(void) {
  UBRR0H = (uint8_t) (SELECTED_UBRR >> 8);
  UBRR0L = (uint8_t) (SELECTED_UBRR & 0xff);
  UCSR0B = (1<<TXEN0);
}

void serial_putc(char c) {
  while (!(UCSR0A & (1<<UDRE0)))
    ; // wait until TX buffer empty
  UDR0 = c;
}

void serial_puts(const char* s) {
  while (*s != 0) {
    serial_putc(*s);
    s++;
  }
}

#ifdef SERIAL_PRINTF
void serial_printf(const char* fmt, ...) {
  va_list args;
  char buffer[SERIAL_PRINTF_SIZE];

  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  serial_puts(buffer);
  va_end(args);
}
#endif /* SERIAL_PRINTF */