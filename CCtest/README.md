# Charge Control test

## Overview

I made a CCtest board that has high side current sensing for the PV input and Battery charging and discharging. The high side sensor only measures one direction so two are needed for the battery. A load is also provided, it has four digital control lines that enable current sinks. 

At startup, the program steps through the load settings for a record. Then it switches between charge and discharge and reports the Battery, and PV voltage and there current. Each discharge is a little deeper, steping down the battery voltage each time. If the serial receives a character it will interrupt the test and enable charging.

For how I setup my Makefile toolchain <http://epccs.org/indexes/Document/DvlpNotes/LinuxBoxCrossCompiler.html>.

With a serial port connection (set the BOOT_PORT in Makefile) and optiboot installed on the RPUno run 'make bootload' and it should compile and then flash the MCU.

``` 
rsutherland@conversion:~/Samba/Irrigate7/CCtest$ make bootload
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o main.o main.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o cctest.o cctest.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o ../Uart/id.o ../Uart/id.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o ../lib/timers.o ../lib/timers.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o ../lib/uart.o ../lib/uart.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o ../lib/adc.o ../lib/adc.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=115200UL -I.  -mmcu=atmega1284p -c -o ../lib/parse.o ../lib/parse.c
avr-gcc -Wl,-Map,Adc.map  -Wl,--gc-sections  -Wl,-u,vfprintf -lprintf_flt -lm -mmcu=atmega1284p main.o cctest.o ../Uart/id.o ../lib/timers.o ../lib/uart.o ../lib/adc.o ../lib/parse.o -o Adc.elf
avr-size -C --mcu=atmega1284p Adc.elf
AVR Memory Usage
----------------
Device: atmega1284p

Program:   10236 bytes (7.8% Full)
(.text + .data + .bootloader)

Data:        190 bytes (1.2% Full)
(.data + .bss + .noinit)


rm -f Adc.o main.o cctest.o ../Uart/id.o ../lib/timers.o ../lib/uart.o ../lib/adc.o ../lib/parse.o
avr-objcopy -j .text -j .data -O ihex Adc.elf Adc.hex
rm -f Adc.elf
avrdude -v -p atmega1284p -c avr109 -P /dev/ttyUSB0 -b 115200 -e -u -U flash:w:Adc.hex

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
avrdude: reading input file "Adc.hex"
avrdude: input file Adc.hex auto detected as Intel Hex
avrdude: writing flash (10236 bytes):

Writing | ################################################## | 100% 1.40s

avrdude: 10236 bytes of flash written
avrdude: verifying flash memory against Adc.hex:
avrdude: load data flash data from input file Adc.hex:
avrdude: input file Adc.hex auto detected as Intel Hex
avrdude: input file Adc.hex contains 10236 bytes
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 1.05s

avrdude: verifying ...
avrdude: 10236 bytes of flash verified

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

Commands are interactive over the serial interface at 115200 baud rate. The echo will start after second charactor of a new line. 

## /0/id? [name|desc|avr-gcc]

identify 

``` 
/0/id?
{"id":{"name":"CCtest","desc":"Irrigate7 Board /w atmega1284p and LT3652","avr-gcc":"4.9"}}
```

##  /0/cctest? [load_step]|[start_step,end_step,bat_mv]

Zero or three argument test, first it waits to enter float mode (PV_IN above 19V), then it will step through each load setting with charger turned off to see the DISCHRG amps. Once at the end_step (default is 15) load, it waits for the voltage to drop bellow bat_mv (6500mV is default) then the load is set to start_step (default is 0) and waits for the PV_IN to change from MPPT mode to float mode (e.g. PV_IN is above 19V) again. Once in float, the process repeats until receiving a character on the UART. 

One argument test sets the load step value and runs with the charger off until receiving a character on the UART.

``` 
/0/cctest?
{"PV_I":"0.066","CHRG":"0.112","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"16.63","PWR":"6.75","MILLIS":"254573","LDSTEP":"0"}
{"PV_I":"0.067","CHRG":"0.090","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"16.63","PWR":"6.74","MILLIS":"256573","LDSTEP":"0"}
{"PV_I":"0.066","CHRG":"0.090","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"16.63","PWR":"6.75","MILLIS":"258574","LDSTEP":"0"}
{"PV_I":"0.069","CHRG":"0.095","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"16.65","PWR":"6.76","MILLIS":"260575","LDSTEP":"0"}
{"PV_I":"0.073","CHRG":"0.105","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"16.68","PWR":"6.77","MILLIS":"262577","LDSTEP":"0"}
{"PV_I":"0.060","CHRG":"0.105","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"19.87","PWR":"6.77","MILLIS":"264578","LDSTEP":"1"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.060","BOOST":"4.50","PV_IN":"19.98","PWR":"6.64","MILLIS":"266578","LDSTEP":"2"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.067","BOOST":"4.59","PV_IN":"19.98","PWR":"6.62","MILLIS":"268579","LDSTEP":"3"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.076","BOOST":"4.50","PV_IN":"19.98","PWR":"6.61","MILLIS":"270580","LDSTEP":"4"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.085","BOOST":"4.50","PV_IN":"19.98","PWR":"6.60","MILLIS":"272581","LDSTEP":"5"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.093","BOOST":"4.50","PV_IN":"19.98","PWR":"6.58","MILLIS":"274582","LDSTEP":"6"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.101","BOOST":"4.50","PV_IN":"19.98","PWR":"6.58","MILLIS":"276583","LDSTEP":"7"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.108","BOOST":"4.50","PV_IN":"19.98","PWR":"6.57","MILLIS":"278584","LDSTEP":"8"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.116","BOOST":"4.50","PV_IN":"19.98","PWR":"6.56","MILLIS":"280585","LDSTEP":"9"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.125","BOOST":"4.50","PV_IN":"19.98","PWR":"6.56","MILLIS":"282587","LDSTEP":"10"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.132","BOOST":"4.40","PV_IN":"19.98","PWR":"6.55","MILLIS":"284588","LDSTEP":"11"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.141","BOOST":"4.50","PV_IN":"19.98","PWR":"6.54","MILLIS":"286588","LDSTEP":"12"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"19.98","PWR":"6.53","MILLIS":"288589","LDSTEP":"13"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.157","BOOST":"4.40","PV_IN":"19.98","PWR":"6.53","MILLIS":"290590","LDSTEP":"14"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.165","BOOST":"4.40","PV_IN":"19.98","PWR":"6.53","MILLIS":"292591","LDSTEP":"15"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.172","BOOST":"4.40","PV_IN":"16.99","PWR":"6.52","MILLIS":"293593","LDSTEP":"0"}
{"PV_I":"0.055","CHRG":"0.106","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"19.01","PWR":"6.78","MILLIS":"322697","LDSTEP":"15"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"16.99","PWR":"6.50","MILLIS":"357366","LDSTEP":"0"}
{"PV_I":"0.052","CHRG":"0.098","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"19.01","PWR":"6.78","MILLIS":"406141","LDSTEP":"15"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"16.96","PWR":"6.45","MILLIS":"480302","LDSTEP":"0"}
{"PV_I":"0.053","CHRG":"0.103","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"19.01","PWR":"6.77","MILLIS":"577157","LDSTEP":"15"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"16.96","PWR":"6.39","MILLIS":"722406","LDSTEP":"0"}
{"PV_I":"0.057","CHRG":"0.112","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"19.01","PWR":"6.77","MILLIS":"890142","LDSTEP":"15"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.145","BOOST":"4.22","PV_IN":"16.99","PWR":"6.34","MILLIS":"3087073","LDSTEP":"0"}
```

# Notes

PV_IN is updated with the MPPT value when the load is switched off and charge is switched on, however the other values are from the point of maximum discharge, e.g. when the battery is just bellow 6.35V.