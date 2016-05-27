# Makefile 
# Use this to flash a serial bootloader
# hexfile is from https://github.com/Optiboot/optiboot
TARGET = Bootloader
LIBDIR = ../lib
OBJECTS = 

## programing ports. the FTDI on my board shows as ttyUSBx, while an Uno shows as ttyACMx
## /dev/ttyUSB0
##/dev/ttyACM0

all:

$(TARGET):

## http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega1284p&LOW=F7&HIGH=D6&EXTENDED=FD&LOCKBIT=2F
## stk500v1 is for Arduino as ISP (e.g. it is an example in the Arduino.cc IDE 1.6.7+)
## lock is from optiboot's Makefile.isp though it should not be needed for ISP
fuse: 
	avrdude -v -p atmega1284p -c stk500v1 -P /dev/ttyACM0 -b 19200 -e -U lock:w:0x3f:m -U lfuse:w:0xF7:m -U hfuse:w:0xD6:m -U efuse:w:0xFD:m

## 1284p can lock the bootloader section so the bootloader does not self-destruct
isp:
	avrdude -v -p atmega1284p -c stk500v1 -P /dev/ttyACM0 -b 19200 -e -U flash:w:optiboot_atmega1284p.hex -U lock:w:0x2f:m

 
