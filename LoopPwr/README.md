# Select Loop Power

## Overview

Irrigate7 has eight current loops for pulse transmission that are terminated with a 100 Ohm resistor. Each loop is selectable when a 74HC238 is addressed and its address line is used to pull down the gate of a PMOS that then allows a current source to power the loop. Each loop termination is used to turn on an open collector connected to ICP1 (only one loop can be powered at a time).


For how I setup my Makefile toolchain <http://epccs.org/indexes/Document/DvlpNotes/LinuxBoxCrossCompiler.html>.

With a serial port connection (set the BOOT_PORT in Makefile) and optiboot installed on the RPUno run 'make bootload' and it should compile and then flash the MCU.

``` 
rsutherland@conversion:~/Samba/Irrigate7/LoopPwr$ make bootload
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o main.o main.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o loop_pwr.o loop_pwr.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o ../Uart/id.o ../Uart/id.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o ../lib/timers.o ../lib/timers.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o ../lib/uart.o ../lib/uart.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o ../lib/parse.o ../lib/parse.c
avr-gcc -Wl,-Map,LoopPwr.map  -Wl,--gc-sections  -Wl,-u,vfprintf -lprintf_flt -lm -mmcu=atmega1284p main.o loop_pwr.o ../Uart/id.o ../lib/timers.o ../lib/uart.o ../lib/parse.o -o LoopPwr.elf
avr-size -C --mcu=atmega1284p LoopPwr.elf
AVR Memory Usage
----------------
Device: atmega1284p

Program:    7160 bytes (5.5% Full)
(.text + .data + .bootloader)

Data:        156 bytes (1.0% Full)
(.data + .bss + .noinit)


rm -f LoopPwr.o main.o loop_pwr.o ../Uart/id.o ../lib/timers.o ../lib/uart.o ../lib/parse.o
avr-objcopy -j .text -j .data -O ihex LoopPwr.elf LoopPwr.hex
rm -f LoopPwr.elf
avrdude -v -p atmega1284p -c avr109 -P /dev/ttyUSB0 -b 115200 -e -u -U flash:w:LoopPwr.hex

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
avrdude: reading input file "LoopPwr.hex"
avrdude: input file LoopPwr.hex auto detected as Intel Hex
avrdude: writing flash (7160 bytes):

Writing | ################################################## | 100% 0.95s

avrdude: 7160 bytes of flash written
avrdude: verifying flash memory against LoopPwr.hex:
avrdude: load data flash data from input file LoopPwr.hex:
avrdude: input file LoopPwr.hex auto detected as Intel Hex
avrdude: input file LoopPwr.hex contains 7160 bytes
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 0.76s

avrdude: verifying ...
avrdude: 7160 bytes of flash verified

avrdude done.  Thank you.
``` 

Now connect with picocom (or ilk). Note I am often at another computer doing this through SSH. The Samba folder is for editing the files from Windows.

``` 
#exit is C-a, C-x
picocom -b 115200 /dev/ttyUSB0
``` 

or log the terminal session

``` 
script -f -c "picocom -b 115200 /dev/ttyUSB0" stuff.log
``` 


# Commands

Commands are interactive over the serial interface at 115200 baud rate. The echo will start after the second character of a new line. 

## /0/id? [name|desc|avr-gcc]

identify 

``` 
/0/id?
{"id":{"name":"LoopPwr","desc":"Irrigate7 Board /w atmega1284p and LT3652","avr-gcc":"4.9"}}
```

##  /0/addrloop 0..7

Set loop address to power for ICP1 event capture.

``` 
/0/addrloop 0
{"PC7":"0","PC6":"0","PC5":"0"}
/0/addrloop 7
{"PC7":"1","PC6":"1","PC5":"1"}
```

##  /0/enloop true|false

Enable power to loop address.

``` 
/0/enloop 1
{"PB4":"1"}
```

# Notes

The loop is used to transmit pulse edges that can be captured with ICP1.

Transmission of the oscilator events from a capacitive sensor used in a simple timing circuit has yet to gain acceptance. The input capture software is consideralby more difficult than the analog conversion typicaly attepmted with these sort of projects. Guess I am trying to say "Here be Dragons".  