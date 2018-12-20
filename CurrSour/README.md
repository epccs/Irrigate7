# Current Source

## Overview

There are four 22mA current sources CS0..CS3 controlled with ATmega1284p pins PB0, PD4, PB3, and PC7. There is one 17mA current source for ICP1 controlled with PD5.


# Firmware Upload

With a serial port connection (set the BOOTLOAD_PORT in Makefile) and xboot installed on the Irrigate7 run 'make bootload' and it should compile and then flash the MCU.

``` 
sudo apt-get install git gcc-avr binutils-avr gdb-avr avr-libc avrdude
git clone https://github.com/epccs/Irrigate7/
cd /Irrigate7/CurrSour
make bootload
...
avrdude done.  Thank you.
``` 

Now connect with picocom (or ilk).


``` 
#exit is C-a, C-x
picocom -b 38400 /dev/ttyUSB0
``` 


# Commands

Commands are interactive over the serial interface at 38400 baud rate. The echo will start after the second character of a new line. 


## /\[rpu_address\]/\[command \[arg\]\]

rpu_address is taken from the I2C address 0x29 (e.g. get_Rpu_address() which is included form ../lib/rpu_mgr.h). The value of rpu_address is used as a character in a string, which means don't use a null value (C strings are null terminated) as an adddress. The ASCII value for '1' (0x31) is easy and looks nice, though I fear it will cause some confusion when it is discovered that the actual address value is 49.

Commands and their arguments follow.


## /0/id? \[name|desc|avr-gcc\]

identify 

``` 
/1/id?
{"id":{"name":"CurrSour","desc":"Irrigate7 (14320^5) Board /w atmega1284p","avr-gcc":"5.4.0"}}
```

##  /0/cs 0..3,ON|OFF

Turn on (or off) 22mA current sources CS0..CS3

``` 
/1/cs 0,ON
{"PB0":"ON"}
/1/cs 0,OFF
{"PB0":"OFF"}
/1/cs 2,ON
{"PB3":"ON"}
/1/cs 2,OFF
{"PB3":"OFF"}
```


##  /0/csp 0,ON|OFF

Turn on (or off) 17mA current source for ICP1

``` 
/1/csp 0,ON
{"ICP1":"ON"}
/1/csp 0,OFF
{"ICP1":"OFF"}
```

