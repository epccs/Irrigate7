# Makefile based Blink

## Overview

Demonstration of General I/O, e.g. Blink an LED. 

Referance ATmega1284p datasheet 14.0 I/O-Ports

Toolchain setup http://epccs.org/indexes/Document/DvlpNotes/LinuxBoxCrossCompiler.html

For A starting point with Makefile see Elliot Williams of <http://hackaday.com/2016/03/15/embed-with-elliot-microcontroller-makefiles>

``` 
rsutherland@conversion:~/Samba/Irrigate7/BlinkLED$ make bootload
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=9600UL -I.  -mmcu=atmega1284p -c -o main.o main.c
avr-gcc -Wl,-Map,blinkLED.map  -Wl,--gc-sections  -mmcu=atmega1284p main.o -o blinkLED.elf
avr-size blinkLED.elf
   text    data     bss     dec     hex filename
    212       0       0     212      d4 blinkLED.elf
rm -f blinkLED.o main.o
avr-objcopy -j .text -j .data -O ihex blinkLED.elf blinkLED.hex
rm -f blinkLED.elf
avrdude -p atmega1284p -c avr109 -P /dev/ttyUSB0 -b 115200 -e -u -U flash:w:blinkLED.hex

Connecting to programmer: .
Found programmer: Id = "XBoot++"; type = S
    Software Version = 1.7; No Hardware Version given.
Programmer supports auto addr increment.
Programmer supports buffered memory access with buffersize=256 bytes.

Programmer supports the following devices:
    Device code: 0x7b

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.00s

avrdude: Device signature = 0x1e9705 (probably m1284p)
avrdude: erasing chip
avrdude: reading input file "blinkLED.hex"
avrdude: input file blinkLED.hex auto detected as Intel Hex
avrdude: writing flash (212 bytes):

Writing | ################################################## | 100% 0.03s

avrdude: 212 bytes of flash written
avrdude: verifying flash memory against blinkLED.hex:
avrdude: load data flash data from input file blinkLED.hex:
avrdude: input file blinkLED.hex auto detected as Intel Hex
avrdude: input file blinkLED.hex contains 212 bytes
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 0.03s

avrdude: verifying ...
avrdude: 212 bytes of flash verified

avrdude done.  Thank you.
``` 
