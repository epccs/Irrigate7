# Select Loop Power

## Overview

CL8 can select one of eight current loop sensors terminates on the 100 Ohm resistor of the ICP3 input.

The hardware on CL8 was was part of Irrigate7 but it was removed and this firmware has not yet been changed to work with the CL8 setup.


For how I setup my Makefile toolchain <http://epccs.org/indexes/Document/DvlpNotes/LinuxBoxCrossCompiler.html>.

With a serial port connection (set the BOOT_PORT in Makefile) and optiboot installed on the RPUno run 'make bootload' and it should compile and then flash the MCU.

``` 
rsutherland@conversion:~/Samba/Irrigate7/LoopPwr$ make bootload
...
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