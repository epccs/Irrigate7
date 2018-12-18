# Digital Input/Output

## Overview

Digital is an interactive command line program that demonstrates control of the Digital input/output from ATmega1284p pins PD2,PD3 (2,3) and PA0..PA3 (24..27).

## Firmware Upload

With a serial port connection (set the BOOTLOAD_PORT in Makefile) and optiboot installed on the RPUno run 'make bootload' and it should compile and then flash the MCU.

``` 
sudo apt-get install git gcc-avr binutils-avr gdb-avr avr-libc avrdude
git clone https://github.com/epccs/Irrigate7/
cd /Irrigate7/Digital
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

rpu_address is taken from the I2C address 0x29 (e.g. ../Uart/id.h get_Rpu_address() ). The value of rpu_address is used as a character in a string, which means don't use a null value (C strings are null terminated), but the ASCII value for '1' (0x31) is easy and looks nice, though I fear it will cause some confusion when it is discovered that the actual address value is 49.

The STATUS_LED is bliked fast (twice per second) if the I2C address is not found, also the rpu_address defaults to '0'. 

Commands and their arguments follow.


## /0/id? \[name|desc|avr-gcc\]

Identify is from ../Uart/id.h Id().

``` 
/1/id?
{"id":{"name":"Digital","desc":"Irrigate7 (14320^5) Board /w atmega1284p","avr-gcc":"5.4.0"}}
```

##  /0/pMod 2|3|24..27,INPUT|OUTPUT    

Set the Data Direction Register (DDRx) bit that sets a pin as INPUT or OUTPUT.

``` 
/1/pMod 2,OUTPUT
{"PD2":"OUTPUT"}
/1/pMod 3,INPUT
{"PD3":"INPUT"}
```

Note: digital 3 may have a status LED, see showstat bellow.


##  /0/dWrt 2|3|24..27,HIGH|LOW    

Set the Port Data Register (PORTx) bit that drives the pin or if mode (e.g. Port Input Register bit) is set as an INPUT enables a pullup. Returns the Port Input Register PINx bit (e.g. same as read command)

``` 
/1/dWrt 2,LOW
{"PD2":"LOW"}
/1/dWrt 3,HIGH
{"PD3":"LOW"}
```

Digital 3 is set as INPUT so it is not in the push-pull mode, the HIGH turns on a weak pullup and a LOW turns off the pullup, it reads as a LOW because the red LED has 1.6V with the weak pullup.


##  /0/dTog 2|3|24..27

Toggle the Port Data Register (PORTx) bit if the Data Direction Register (DDRx) bit is set as an OUTPUT. Returns the Port Input Register PINx bit (e.g. same as read command)

``` 
/1/dTog 2
{"PD2":"HIGH"}
/1/dTog 3
{"PD3":"LOW"}
```

Toggle will ckeck if in OUTPUT mode befor changing it


##  /0/dRe? 2|3|24..27

Read the Port Input Register (PINx) bit that was latched during last low edge of the system clock.

``` 
/1/dRe? 2
{"PD2":"HIGH"}
/1/dRe? 3
{"PD3":"LOW"}
```


##  /0/showstat ON|OFF

An IO (TX1) can be used to show status, but it is not ON by default.

``` 
/1/showstat ON
{"SHOW":"ON"}
/1/showstat OFF
{"SHOW":"OFF"}
```
