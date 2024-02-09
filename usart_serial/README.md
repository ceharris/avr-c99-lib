usart_serial
============

C module for asynchronous serial I/O on an AVR microcontroller using the USART peripheral.

Usage
-----

Call the `serial_init` function as part of your program's setup. Then use any of the functions
described in [serial.h](serial.h) to communicate.

```c
void setup(void) {
  serial_init();
  serial_puts("Hello, world.\n");
}
```

Optional `printf` Function
--------------------------

This module includes an optional `serial_printf` function that produces formatted output for
arguments of various data types. This can be handy for certain types of debugging, but it also
significantly increases the size of your program by including portions of `stdio` from the 
standard C library.

By default, `serial_printf` is defined as a no-op preprocessor macro. To use the 
`serial_printf` function in your program, define `SERIAL_PRINTF` in your build. 

The `serial_printf` function uses a fixed length buffer allocated on the stack. The output
produced by a call to the function will be truncated at the size specified by 
the `SERIAL_PRINTF_SIZE` preprocessor directive. A default value is defined in [serial.c](serial.c).