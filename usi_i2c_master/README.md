usi_twi_master
==============

C module for using the Two Wire Interface (TWI) mode of the
Universal Serial Interface (USI) in AVR microcontrollers such
as the ATtiny85.

Adapted from original work by Adam Honse, to remove various
compiler warnings, use a more consistent naming, and generally
tidy up.

Usage
-----

Include the module header and call the init function as part of
your AVR program setup routine. Subsequently, you can transfer
data to and from I2C slave devices using the other functions of 
the module. See `usi_twi_master.h` for descriptions of the available
functions.

```c
#include "usi_twi_master.h"

void setup(void) {
  twi_master_init();
}
```
