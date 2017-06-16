# Analog-to-Digital Converter

## Overview

Adc is an interactive command line program that demonstrates control of an ATmega1284p Analog-to-Digital Converter from pins PA0 through PA7. 


# EEPROM Memory map 

A map of calibration valuses in EEPROM. 

```
function            type        ee_addr:
id                  UINT16      30
ref_extern_avcc     UINT32      32
ref_intern_1v1      UINT32      36
```

The AVCC pin is used to power the analog to digital converter and is also used as a reference. On RPUno the AVCC pin is powered by a switchmode supply that can be measured to use as a calibrated reference.

The internal 1V1 bandgap is not trimmed by the manufacturer, so it is nearly useless until it is measured. However, once it is known it is a good reference.


# Firmware Upload

With a serial port connection (set the BOOT_PORT in Makefile) and optiboot installed on the RPUno run 'make bootload' and it should compile and then flash the MCU.

``` 
rsutherland@conversion:~/Samba/Irrigate7/Adc$ make bootload
...
avrdude done.  Thank you.
``` 

Now connect with picocom (or ilk). Note I am often at another computer doing this through SSH. The Samba folder is for editing the files from Windows.


``` 
#exit is C-a, C-x
picocom -b 38400 /dev/ttyUSB0
``` 

or log the terminal session

``` 
script -f -c "picocom -b 38400 /dev/ttyUSB0" stuff.log
``` 


# Commands

Commands are interactive over the serial interface at 38400 baud rate. The echo will start after the second character of a new line. 


## /[rpu_address]/[command [arg]]

rpu_address is taken from the I2C address 0x29 (e.g. get_Rpu_address() which is included form ../Uart/id.h). The value of rpu_address is used as a character in a string, which means don't use a null value (C strings are null terminated), but the ASCII value for '1' (0x31) is easy and looks nice, though I fear it will cause some confusion when it is discovered that the actual address value is 49.

Commands and their arguments follow.


## /0/id? [name|desc|avr-gcc]

identify 

``` 
/1/id?
{"id":{"name":"Adc","desc":"Irrigate7 Board /w atmega1284p and LT3652","avr-gcc":"4.9"}}
```

##  /0/analog? 0..7[,0..7[,0..7[,0..7[,0..7]]]]    

Analog-to-Digital Converter reading from up to 5 ADMUX channels. The reading repeats every 60 Seconds until the Rx buffer gets a character. On RPUno channel 6 is the solar input voltage, channel 7 is the main power node (PWR) voltage, channel 3 is the battery discharge current, channel 2 is the battery charging current, channel 0,1,4, and 5 are user inputs (example data is a floating input).

``` 
/1/analog? 2,3,6,7
{"CHRG_A":"0.052","DISCHRG_A":"0.000","PV_V":"20.04","PWR_V":"13.61"}
{"CHRG_A":"0.052","DISCHRG_A":"0.000","PV_V":"20.04","PWR_V":"13.60"}
/1/analog? 0,1,4,5
{"ADC0":"1.65","ADC1":"1.57","ADC4":"1.79","ADC5":"1.51"}
```

The value reported is based on the referance value which is saved in EEPROM, see bellow.


##  /0/avcc 4500000..5500000

Calibrate the AVCC reference in microvolts. This is the reference used for all measurements.

``` 
/1/avcc 5009000
{"REF":{"extern_avcc":"5.0090",}}
``` 


##  /0/onevone 900000..1300000

Calibrate the 1V1 internal bandgap reference in microvolts. This is not used at this time.

```
/1/onevone 1100000
{"REF":{"intern_1v1":"1.1000",}}
``` 


##  /0/reftoee

Save the reference in static memory to EEPROM.

```
/1/reftoee
{"REF":{"extern_avcc":"5.0090","intern_1v1":"1.1000",}}
```


##  /0/reffrmee

Load the reference from EEPROM into static memory.

```
/1/reffrmee
{"REF":{"extern_avcc":"5.0090","intern_1v1":"1.1000",}}
```

