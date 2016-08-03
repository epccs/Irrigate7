# Charge Control test

## Overview

I made a  [CCtest]<http://epccs.org/indexes/Board/CCtest> board that has high side current sensing for the PV input and Battery charging and discharging. The high side sensor only measures one direction so two are needed for the battery. A load is also provided, it has four digital control lines that enable current sinks. 

At startup, the program waits until the PV_IN is over 19V which means solar has charged the battery to its float level. It then delays for 3hr to allow an absorption cycle to complete. After which a delay occurs until PV_IN has dropped to less than 5V (e.g. night). Next, the load settings are stepped through for a record. Then it discharges and reports as the Battery voltage crosses 50mV thresholds until the discharge voltage is reached. If the serial receives a character it will interrupt the test and enable charging.

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

Program:   11580 bytes (8.8% Full)
(.text + .data + .bootloader)

Data:        223 bytes (1.4% Full)
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
avrdude: writing flash (11580 bytes):

Writing | ################################################## | 100% 1.60s

avrdude: 11580 bytes of flash written
avrdude: verifying flash memory against Adc.hex:
avrdude: load data flash data from input file Adc.hex:
avrdude: input file Adc.hex auto detected as Intel Hex
avrdude: input file Adc.hex contains 11580 bytes
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 1.24s

avrdude: verifying ...
avrdude: 11580 bytes of flash verified

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

For zero or three argument test, first the program waits until the PV_IN is over 19V which means solar has charged the battery to its float level. Next, the load settings are stepped through for a record. Then the Loop enters now to discharge and reports as the Battery voltage crosses 50mV thresholds until the discharge voltage is reached. Cycling through a verify MPPT mode that starts charging up to the float voltage. Following an absorption cycle to complete the charge. After which it waits until Night (PV_IN less than 5V). And then the loop cycles back into discharge. If the serial line receives a character it will interrupt the test and enable charging.

One argument test sets the load step value and runs with the charger off until receiving a character on the UART.

``` 
/0/cctest?
{"PV_I":"0.052","CHRG":"0.072","PV_IN":"16.60","PWR":"6.59","TIME":"6","LD":"0"}
{"PV_I":"0.043","CHRG":"0.080","PV_IN":"19.01","PWR":"6.77","TIME":"103499","LD":"1"}
{"DISCHRG":"0.062","PV_IN":"19.20","PWR":"6.72","TIME":"105498","LD":"2"}
{"DISCHRG":"0.069","PV_IN":"19.92","PWR":"6.65","TIME":"107499","LD":"3"}
{"DISCHRG":"0.078","PV_IN":"19.92","PWR":"6.64","TIME":"109500","LD":"4"}
{"DISCHRG":"0.086","PV_IN":"19.92","PWR":"6.62","TIME":"111501","LD":"5"}
{"DISCHRG":"0.093","PV_IN":"19.92","PWR":"6.61","TIME":"113502","LD":"6"}
{"DISCHRG":"0.101","PV_IN":"19.92","PWR":"6.60","TIME":"115503","LD":"7"}
{"DISCHRG":"0.108","PV_IN":"19.92","PWR":"6.59","TIME":"117505","LD":"8"}
{"DISCHRG":"0.116","PV_IN":"19.92","PWR":"6.58","TIME":"119505","LD":"9"}
{"DISCHRG":"0.125","PV_IN":"19.92","PWR":"6.58","TIME":"121506","LD":"10"}
{"DISCHRG":"0.132","PV_IN":"19.92","PWR":"6.57","TIME":"123507","LD":"11"}
{"DISCHRG":"0.141","PV_IN":"19.92","PWR":"6.56","TIME":"125508","LD":"12"}
{"DISCHRG":"0.149","PV_IN":"19.92","PWR":"6.56","TIME":"127509","LD":"13"}
{"DISCHRG":"0.155","PV_IN":"19.92","PWR":"6.55","TIME":"129510","LD":"14"}
{"DISCHRG":"0.164","PV_IN":"19.92","PWR":"6.54","TIME":"131511","LD":"15"}
{"DISCHRG":"0.172","PV_IN":"19.92","PWR":"6.53","TIME":"133512","LD":"15"}
{"DISCHRG":"0.148","PV_IN":"19.92","PWR":"6.50","TIME":"157497","LD":"15"}
{"DISCHRG":"0.146","PV_IN":"19.92","PWR":"6.45","TIME":"196733","LD":"15"}
{"rpt":"VerifyMPPT"}
{"DISCHRG":"0.146","PV_IN":"16.96","PWR":"6.39","TIME":"255933","LD":"0"}
{"rpt":"AbsorptionBeg"}
*****{"rpt":"AbsorptionEnd"}
{"rpt":"VerifyNight"}
{"PV_I":"0.055","CHRG":"0.106","PV_IN":"4.99","PWR":"6.77","TIME":"478832","LD":"15"}
{"DISCHRG":"0.148","PV_IN":"12.94","PWR":"6.55","TIME":"501201","LD":"15"}
{"DISCHRG":"0.146","PV_IN":"20.86","PWR":"6.50","TIME":"535259","LD":"15"}
{"DISCHRG":"0.145","PV_IN":"20.86","PWR":"6.45","TIME":"573504","LD":"15"}
{"rpt":"VerifyMPPT"}
{"DISCHRG":"0.145","PV_IN":"16.99","PWR":"6.39","TIME":"628700","LD":"0"}
```

# Notes

TBD more debuging to do.

The battery discharge current reads near 172mA when the serial driver is more active and 146mA when it is not active, this is because the RS485 drivers were not sinking current at the moment of the 146mA reading. 