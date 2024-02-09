lcd
===

C module for programming the Hitachi HD44780 LCD Controller.
Inspired by Arduino's LiquidCrystal library.
 
This module supports only the 4-bit interface for the HD44780,
with 5x8 characters.

The module is designed such that it can be used with the HD44780
connected directly to GPIO pins of the AVR or via the I2C bus 
(e.g. using the PCF8574).

Usage
-----

To use this module, first determine how the LCD will be interfaced
to the AVR. There are two common options:

1. [Use GPIO Pins](#use-gpio-pins)
2. [Use I2C](#use-i2c)


### Use GPIO Pins

Choose eight available GPIO pins for the connection to the HD44780
LCD controller. The choice of pins will often depend on what else
your AVR is doing or the development board you're using. Using the
ATmega328P, one possible choice would be to use PB0..PB3 for the 
control pins and PD4..PD7 for the data pins, but there are many other 
possibilities.

Let's assume for this example that you are using PB0..PB3 and 
PD4..7 as desribed above. 

Connect the LCD controller as follows:

1. RS <-> PB0
2. RW <-> PB1
3. E  <-> PB2
4. Backlight (if available) <-> PB3
5. D4 <-> PD4
6. D5 <-> PD5
7. D6 <-> PD6
8. D7 <-> PD7


As part of your AVR program initialization, you'll need to configure all 
of these pins as outputs.

```c
DDRB |= 0xf;
DDRD |= 0xf0;
```

Write a C function that will be used as a callback each time that
the LCD module wants to send data to the LCD controller. 

```c
int lcd_gpio_write(uint8_t ctx, uint8_t r) {
  // The lower 4 bits of r contain the desired state of the control pins
  // (see bit descriptions in lcd.h)
  PORTB = (PORTB & 0xf0) | (r & 0x0f)
  // The upper 4 bits of r contain 4 bits of data to be written
  PORTD = (PORTD & 0x0f) | (r & 0xf0)
  return 0;
}
```

The `ctx` argument is a single byte of context that you can specify 
when setting up the LCD module. It is passed to your function, 
unchanged. In this example, it isn't needed. In the case of the I2C 
interface, this byte could be used to hold the I2C address of the LCD 
controller.

The return value from the function isn't used in current versions of
the LCD module. For compatibility with future versions of the module,
your function should return zero.

To use the LCD module, you'll declare and initialize of the LCD context
structure, then call `lcd_init`.

```c
#include "lcd.h"

LCD lcd;

void setup() {
  // configure GPIO pins as outputs
  DDRB |= 0xf;
  DDRD |= 0xf0;

  // specify the callback function
  lcd.write = lcd_gpio_write;
  // specify the value to be passed to the function as context
  lcd.ctx = 0;    // we don't care about this value in this example
  
  lcd_init(&lcd);
}
```

After the LCD module is initialized, you can use any of the functions
it provides to control the LCD and display information on it.


### Use I2C

Virtually all of the commonly available LCD modules that have an I2C
interface use the PCF8574 chip for I2C and use the same pinout between
the PCF8474 and the HD44780 LCD controller.

You'll connect the LCD's I2C interface pins (SDA and SCL) to the
corresponding pins of the AVR. You'll want external pullup resistors
on these pins.

Depending on which AVR chip you're using, you'll have some support
built-in support for I2C. See "Two Wire Interface" (TWI) in the the 
AVR datasheet.

#### ATmega328P and Related Microcontollers

The ATmega328P and its cousins have high-level TWI support that
can be used by simply manipulating the appropriate I/O registers.

```
TODO -- provide an example
```

#### ATtiny85 and Related Microcontrollers

The smaller microcontrollers (e.g. ATtiny85) provide TWI support via 
the Universal Serial Interface (USI). The programming is a bit more
complicated. You'll need code that allows the USI to act as a TWI bus 
master. The [usi_twi_master](../usi_twi_master/README.md) module in
this library provides a reasonable solution that interfaces nicely
with the LCD module. Of course, you can roll your own or use someone
else's implementation. The LCD module simply requires a function for
writing a single byte to the LCD controller as an I2C slave, as
documented in `lcd.h`.

Assuming that you're using the `usi_twi_master` module, using an I2C
LCD is mostly a matter of performing the correct setup sequence.

```c
#include "usi_twi_master.h"
#include "lcd.h"

// I2C address of the LCD controller (0..0x7f)
#define LCD_ADDRESS 0x27

LCD lcd;

void setup(void) {
  // configure the LCD write callback function and context
  lcd.write = twi_master_out;
  lcd.ctx = LCD_ADDRESS;
  // initialize the TWI mode of the USI hardware
  twi_master_init();
  // initialize the LCD controller
  lcd_init(&lcd);
}
```

After the LCD module is initialized, you can use any of the functions
it provides to control the LCD and display information on it.
