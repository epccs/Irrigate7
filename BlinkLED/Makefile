# Makefile simple cross compiler example for learning
# see Elliot Williams write up
# http://hackaday.com/2016/03/15/embed-with-elliot-microcontroller-makefiles
# https://en.wikipedia.org/wiki/Make_%28software%29
# http://ix.io/seE
#
# to see the implicit rules which automagicly turn blinkLED.c into blinkLED.o run
# make -p
TARGET = blinkLED
OBJECTS = main.o

## Chip and project-specific global definitions
MCU   =  atmega1284p
F_CPU = 16000000UL  
BAUD  =  9600UL
CPPFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I. 

## Cross-compilation
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size

## programing ports. the FTDI on my board shows as ttyUSBx, while an Uno shows as ttyACMx
BOOT_PORT = /dev/ttyUSB0
ISP_PORT = /dev/ttyACM0

## Compiler/linker options
CFLAGS = -Os -g -std=gnu99 -Wall
# CFLAGS += -funsigned-char -funsigned-bitfields 
# CFLAGS += -fpack-struct -fshort-enums 
CFLAGS += -ffunction-sections -fdata-sections 

TARGET_ARCH = -mmcu=$(MCU)

LDFLAGS = -Wl,-Map,$(TARGET).map 
LDFLAGS += -Wl,--gc-sections 

all: $(TARGET).hex

$(TARGET): $(TARGET).hex

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
	rm -f $(TARGET).elf

# http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega1284p&LOW=F7&HIGH=D6&EXTENDED=FD&LOCKBIT=2F
## Arduino as ISP (e.g. it is an example in the Arduino.cc IDE 1.6.7+): -c stk500v1 -P /dev/ttyACM0
## dragon: -c dragon_isp -P usb
## to reduce dupliction I only show the fuse setting in ../Bootloader

## lock the bootloader section after flashing, it does not stop isp when bootloader is not used
isp: $(TARGET).hex
	avrdude -v -p $(MCU) -c stk500v1 -P $(ISP_PORT) -b 19200 -e -U flash:w:$(TARGET).hex -U lock:w:0x2f:m

## works with xboot_avr109_irrigate7.hex bootloader.
## stk500v2: erase command isn't supported use -D to disable auto erase, 
##            see https://github.com/sudar/Arduino-Makefile/issues/114
## arduino: optiboot will always auto erase, it may fail if told to do so e.g. do not use -e, 
## avr109: xboot wants to see the erase command -e and needs safemode fuse checking disabled -u
bootload: $(TARGET).hex
	avrdude -p $(MCU) -c avr109 -P $(BOOT_PORT) -b 115200 -e -u -U flash:w:$(TARGET).hex

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ -o $@
	$(SIZE) $@
	rm -f $(TARGET).o $(OBJECTS)

clean: 
	rm -f $(TARGET).hex $(TARGET).map
 
