
#ifndef USART_SERIAL_H
#define USART_SERIAL_H

/**
 * Initializes the USART for asynchronous I/O.
 */
void serial_init(void);

/**
 * Transmits a single character over the asynchronous serial interface.
 * @param c the character to transmit
 */
void serial_putc(char c);

/**
 * Transmits a null-terminated string over the asynchronous serial interface.
 * @param s the string to transmit
 */
void serial_puts(const char* s);

#ifdef SERIAL_PRINTF
/**
 * Transmits a formatted string over the asynchronous serial interface.
 * The output string will be truncated at the size specified by the `SERIAL_PRINTF_SIZE`
 * preprocessor directive (default value is specified in `serial.c`)
 * @param fmt format template string
 * @param ... arguments for the format string
 */
void serial_printf(const char* fmt, ...);
#else
#define serial_printf(fmt, ...) ()
#endif
#endif /* USART_SERIAL_H */
