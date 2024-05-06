max7221
=======

Support for an 8-digit LED display using the MAX 7221 serially interface
LED display driver. The MAX 7221 provides an SPI compatible interface to 
drive eight 7-segment displays.

The default configuration of the module assumes that the MAX 7221 is
connected to the SPI CLK, MOSI, and SS pins of an AVR microcontroller.
If you choose to use another pin for chip select, instead of SS, be sure
to read and understand the note for the MSTR Master/Slave select pin in
the SPCR (SPI Control Register) of the relevant AVR microcontroller
documentation.

