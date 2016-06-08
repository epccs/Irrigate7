# Serial parsing for textual commands and arguments

## Overview

Demonstration of command parsing with the redirected stdio functions (e.g. printf() and simular)  from avr-libc. 

For how I setup my Makefile toolchain <http://epccs.org/indexes/Document/DvlpNotes/LinuxBoxCrossCompiler.html>.

With a serial port connection (set the BOOT_PORT in Makefile) and xboot installed on the Irrigate7 run 'make bootload' and it should compile and then flash the MCU.

``` 
rsutherland@conversion:~/Samba/Irrigate7/Parsing$ make bootload
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=9600UL -I.  -mmcu=atmega1284p -c -o main.o main.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=9600UL -I.  -mmcu=atmega1284p -c -o ../lib/uart.o ../lib/uart.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=9600UL -I.  -mmcu=atmega1284p -c -o ../lib/parse.o ../lib/parse.c
avr-gcc -Wl,-Map,Parsing.map  -Wl,--gc-sections  -mmcu=atmega1284p main.o ../lib/uart.o ../lib/parse.o -o Parsing.elf
avr-size -C Parsing.elf
AVR Memory Usage
----------------
Device: Unknown

Program:    4566 bytes
(.text + .data + .bootloader)

Data:        139 bytes
(.data + .bss + .noinit)


rm -f Parsing.o main.o ../lib/uart.o ../lib/parse.o
avr-objcopy -j .text -j .data -O ihex Parsing.elf Parsing.hex
rm -f Parsing.elf
avrdude -v -p atmega1284p -c avr109 -P /dev/ttyUSB0 -b 115200 -e -u -U flash:w:Parsing.hex

avrdude: Version 6.2
         Copyright (c) 2000-2005 Brian Dean, http://www.bdmicro.com/
         Copyright (c) 2007-2014 Joerg Wunsch

         System wide configuration file is "/etc/avrdude.conf"
         User configuration file is "/home/rsutherland/.avrduderc"
         User configuration file does not exist or is not a regular file, skipping

         Using Port                    : /dev/ttyUSB0
         Using Programmer              : avr109
         Overriding Baud Rate          : 115200
         AVR Part                      : ATmega1284P
         Chip Erase delay              : 55000 us
         PAGEL                         : PD7
         BS2                           : PA0
         RESET disposition             : dedicated
         RETRY pulse                   : SCK
         serial program mode           : yes
         parallel program mode         : yes
         Timeout                       : 200
         StabDelay                     : 100
         CmdexeDelay                   : 25
         SyncLoops                     : 32
         ByteDelay                     : 0
         PollIndex                     : 3
         PollValue                     : 0x53
         Memory Detail                 :

                                  Block Poll               Page                       Polled
           Memory Type Mode Delay Size  Indx Paged  Size   Size #Pages MinW  MaxW   ReadBack
           ----------- ---- ----- ----- ---- ------ ------ ---- ------ ----- ----- ---------
           eeprom        65    10   128    0 no       4096    8      0  9000  9000 0xff 0xff
           flash         65    10   256    0 yes    131072  256    512  4500  4500 0xff 0xff
           lock           0     0     0    0 no          1    0      0  9000  9000 0x00 0x00
           lfuse          0     0     0    0 no          1    0      0  9000  9000 0x00 0x00
           hfuse          0     0     0    0 no          1    0      0  9000  9000 0x00 0x00
           efuse          0     0     0    0 no          1    0      0  9000  9000 0x00 0x00
           signature      0     0     0    0 no          3    0      0     0     0 0x00 0x00
           calibration    0     0     0    0 no          1    0      0     0     0 0x00 0x00

         Programmer Type : butterfly
         Description     : Atmel AppNote AVR109 Boot Loader

Connecting to programmer: .
Found programmer: Id = "XBoot++"; type = S
    Software Version = 1.7; No Hardware Version given.
Programmer supports auto addr increment.
Programmer supports buffered memory access with buffersize=256 bytes.

Programmer supports the following devices:
    Device code: 0x7b

avrdude: devcode selected: 0x7b
avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.00s

avrdude: Device signature = 0x1e9705 (probably m1284p)
avrdude: erasing chip
avrdude: reading input file "Parsing.hex"
avrdude: input file Parsing.hex auto detected as Intel Hex
avrdude: writing flash (4566 bytes):

Writing | ################################################## | 100% 0.63s

avrdude: 4566 bytes of flash written
avrdude: verifying flash memory against Parsing.hex:
avrdude: load data flash data from input file Parsing.hex:
avrdude: input file Parsing.hex auto detected as Intel Hex
avrdude: input file Parsing.hex contains 4566 bytes
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 0.47s

avrdude: verifying ...
avrdude: 4566 bytes of flash verified

avrdude done.  Thank you.
``` 

Now connect with picocom (or ilk). Note I am often at another computer doing this through SSH. The Samba folder is for editing the files from Windows.

``` 
#exit is C-a, C-x
picocom -b 9600 /dev/ttyUSB0
``` 


# Command Line

/addr/command [arg[,arg[,arg[,arg[,arg]]]]]

The command line is structured like MQTT topics with some optional arguments 


## addr

The address is a single character. That is to say, "/0/id?" is at address '0', where "/id?" is a command. Echo of the command occurs after the address is received so the serial data can be checked as well as being somewhat keyboard friendly device.

## command

The command is made of one or more characters that are: alpha (see isalpha() function), '/', or '?'. 

## arg

The argument(s) are made of one or more characters that are: alphanumeric (see isalnum() function). 
