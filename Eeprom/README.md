# AVR EEPROM

## Overview

Eeprom is an interactive command line program that demonstrates the control of an ATmega1284p EEPROM.

Depends on avr/eeprom.h from avr-libc <http://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html>.

For how I setup my Makefile toolchain <http://epccs.org/indexes/Document/DvlpNotes/LinuxBoxCrossCompiler.html>.

With a serial port connection (set the BOOT_PORT in Makefile) and optiboot installed on the RPUno run 'make bootload' and it should compile and then flash the MCU.

``` 
rsutherland@conversion:~/Samba/Irrigate7/Eeprom$ make bootload
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o main.o main.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o ee.o ee.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o ../Uart/id.o ../Uart/id.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o ../lib/timers.o ../lib/timers.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o ../lib/uart.o ../lib/uart.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o ../lib/adc.o ../lib/adc.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o ../lib/parse.o ../lib/parse.c
avr-gcc -Wl,-Map,Eeprom.map  -Wl,--gc-sections  -Wl,-u,vfprintf -lprintf_flt -lm -mmcu=atmega1284p main.o ee.o ../Uart/id.o ../lib/timers.o ../lib/uart.o ../lib/adc.o ../lib/parse.o -o Eeprom.elf
avr-size -C --mcu=atmega1284p Eeprom.elf
AVR Memory Usage
----------------
Device: atmega1284p

Program:    7696 bytes (5.9% Full)
(.text + .data + .bootloader)

Data:        177 bytes (1.1% Full)
(.data + .bss + .noinit)


rm -f Eeprom.o main.o ee.o ../Uart/id.o ../lib/timers.o ../lib/uart.o ../lib/adc.o ../lib/parse.o
avr-objcopy -j .text -j .data -O ihex Eeprom.elf Eeprom.hex
rm -f Eeprom.elf
avrdude -v -p atmega1284p -c avr109 -P /dev/ttyUSB0 -b 115200 -e -u -U flash:w:Eeprom.hex

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
avrdude: reading input file "Eeprom.hex"
avrdude: input file Eeprom.hex auto detected as Intel Hex
avrdude: writing flash (7696 bytes):

Writing | ################################################## | 100% 1.06s

avrdude: 7696 bytes of flash written
avrdude: verifying flash memory against Eeprom.hex:
avrdude: load data flash data from input file Eeprom.hex:
avrdude: input file Eeprom.hex auto detected as Intel Hex
avrdude: input file Eeprom.hex contains 7696 bytes
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 0.78s

avrdude: verifying ...
avrdude: 7696 bytes of flash verified

avrdude done.  Thank you.
``` 

Now connect with picocom (or ilk). Note I am often at another computer doing this through SSH. The Samba folder is for editing the files from Windows.


``` 
#exit is C-a, C-x
picocom -b 115200 /dev/ttyUSB0
``` 

# Commands

Commands are interactive over the serial interface at 115200 baud rate. The echo will start after second charactor of a new line. 

## /0/id? [name|desc|avr-gcc]

identify 

``` 
/0/id?
{"id":{"name":"Eeprom","desc":"Irrigate7 Board /w atmega1284p and LT3652","avr-gcc":"4.9"}}
```

##  /0/ee? 0..4095

Return the EEPROM value at address. This checks if eeprom_is_ready() befor trying to read the EEPROM, if it is not ready the program loops back through the round robin where a received charactor may terminate the command. 

``` 
/0/ee? 0
{"EE[0]":"255"}
```

##  /0/ee 0..1023,0..255

Write the value given as argument one to the address given as Argument zero. This checks if eeprom_is_ready() befor trying to write to the EEPROM, if it is not ready the program loops back through the round robin where a received charactor may terminate the command. If the command is terminated the write may not occure. The JSON response is a read of the EEPROM. 

Warning writing EEPROM can lead to device failure, it is only rated for 100k write cycles.

``` 
/0/ee 0,16
{"EE[0]":"16"}
```