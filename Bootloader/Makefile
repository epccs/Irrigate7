# Makefile 
# Use this to flash a serial bootloader
TARGET = Bootloader
OBJECTS = 

## Chip and project-specific global definitions
MCU = atmega1284p

# the efuse uses the lower three bits, so some tools read it as 0x05.
# also the lockbit uses the lower five bits, so some tools read unlock as 0xFF and lock as 0xEF
# http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega1284p&LOW=F7&HIGH=D0&EXTENDED=FD&LOCKBIT=2F
LOW_FUSE = 0xF7
HIGH_FUSE = 0xD0
EXTENDED_FUSE = 0xFD
# avrdude -v <= 6.2 
#UNLOCK_FUSE = 0x3F
#LOCK_FUSE = 0x2F
# avrdude -v >= 6.3 : "1" is returned for unused bits for -c stk500v1
UNLOCK_FUSE = 0xFF
LOCK_FUSE = 0xEF

## programing ports. the FTDI on my board shows as ttyUSBx, while an Uno shows as ttyACMx
## /dev/ttyUSB0
##/dev/ttyACM0

all:

$(TARGET):

## stk500v1 is for Arduino as ISP (e.g. it is an example in the Arduino.cc IDE 1.6.7+)
## dragon_isp is for Atmel Dragon in ISP mode (Dragone USB is SMD and easy to break) 
## dragon: -c dragon_isp -P usb
## boot flash section is 4k words (8k bytes) for stk500boot_v2_irrigate7.hex bootloader
## lock is from optiboot's Makefile.isp though it should not be needed for ISP
fuse: 
	avrdude -v -p $(MCU) -c stk500v1 -P /dev/ttyACM0 -b 19200 -e -U lock:w:$(UNLOCK_FUSE):m -U lfuse:w:$(LOW_FUSE):m -U hfuse:w:$(HIGH_FUSE):m -U efuse:w:$(EXTENDED_FUSE):m

isp:
	avrdude -v -p $(MCU) -c stk500v1 -P /dev/ttyACM0 -b 19200 -e -U flash:w:xboot_avr109_atmega1284p_-b115200_F_CPU16000000.hex -U lock:w:$(LOCK_FUSE):m
	
verify:
	avrdude -v -p $(MCU) -c stk500v1 -P /dev/ttyACM0 -b 19200  -U flash:v:xboot_avr109_atmega1284p_-b115200_F_CPU16000000.hex

## set fuse like they are from factory
oem: 
	avrdude -v -p $(MCU) -c stk500v1 -P /dev/ttyACM0 -b 19200 -e -U lock:w:$(UNLOCK_FUSE):m -U lfuse:w:0x42:m -U hfuse:w:0x99:m -U efuse:w:0xFF:m

## funny thing: AVR dragon is fast on Ubuntu 16.04 but slow on Win 10 with Atmel Studio 7.0.943
