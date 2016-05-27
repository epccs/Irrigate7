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
## stk500v1 is for Arduino as ISP (e.g. it is an example in the Arduino.cc IDE 1.6.7+)
## lock is from optiboot's Makefile.isp, If I understand it then overwriting an existing bootloader requires two steps
fuse: 
	avrdude -v -p $(MCU) -c stk500v1 -P $(ISP_PORT) -b 19200 -e -U lock:w:0x3f:m -U lfuse:w:0xF7:m -U hfuse:w:0xD6:m -U efuse:w:0xFD:m

## lock the bootloader section after flashing, it does not stop isp when bootloader is not used
isp: $(TARGET).hex
	avrdude -v -p $(MCU) -c stk500v1 -P $(ISP_PORT) -b 19200 -e -U flash:w:$(TARGET).hex -U lock:w:0x2f:m

## works with optiboot which erases flash without being told (e.g. -e )
bootload: $(TARGET).hex
	avrdude -v -p $(MCU) -c arduino -P $(BOOT_PORT) -b 115200 -U flash:w:$(TARGET).hex

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ -o $@
	$(SIZE) $@
	rm -f $(TARGET).o $(OBJECTS)

clean: 
	rm -f $(TARGET).hex $(TARGET).map
 
