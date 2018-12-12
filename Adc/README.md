# Analog-to-Digital Converter

## Overview

Adc is an interactive command line program that demonstrates control of an Analog-to-Digital Converter from pins PA0 through PA7 on an ATmega1284p. 

A customized library routine is used to operate the AVR's ADC, it has an ISR that is started with the enable_ADC_auto_conversion function to read the channels in a burst (free running is also an option). In this case, the bare metal loop starts the burst at timed intervals of 200 milliseconds. The ADC clock runs at 125kHz and it takes 25 of its clocks to complete a conversion, thus a burst takes 1.6 milliseconds (e.g. 8*25*(1/125000)) to scan the eight channels. The ADC is turned off after each burst so some power is saved, but barely enough to bother.

The ADMUX register is used when selecting ADC channels.  The analog channel has nothing to do with the digital pin number used by [Wiring]. The pin used by ADC channel 0 has been assigned the number 24 for the digital Wiring functions (e.g. pinMod, digitalRead...), however for analogRead the ADC channel number is used.

[Wiring]: https://arduinohistory.github.io/


# EEPROM Memory map 

A map of calibration valuses in EEPROM. 

```
function            type        ee_addr:
id                  UINT16      30
ref_extern_avcc     UINT32      32
ref_intern_1v1      UINT32      36
```

The AVCC pin is used to power the analog to digital converter and is also used as a reference. On Irrigate7 the AVCC pin is powered by a switchmode supply that can be measured to use as a calibrated reference.

The internal 1V1 bandgap is not trimmed by the manufacturer, so it is nearly useless until it is measured. However, once it is known it is a good reference.

[SelfTest] loads calibration values for references in EEPROM.

[SelfTest]: https://github.com/epccs/Irrigate7/tree/master/SelfTest


# Firmware Upload

With a serial port connection (see BOOTLOAD_PORT in Makefile) and xboot installed on the Irrigate7 run 'make bootload' and it should compile and then flash the MCU.

```
sudo apt-get install git gcc-avr binutils-avr gdb-avr avr-libc avrdude
git clone https://github.com/epccs/Irrigate7/
cd /Irrigate7/Adc
make bootload
...
avrdude done.  Thank you.
``` 

Now connect with picocom (or ilk).


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

rpu_address is taken from the I2C address 0x29 (e.g. get_Rpu_address() which is included form ../lib/rpu_mgr.h). The value of rpu_address is used as a character in a string, which means don't use a null value (C strings are null terminated) as an adddress. The ASCII value for '1' (0x31) is easy and looks nice, though I fear it will cause some confusion when it is discovered that the actual address value is 49.

Commands and their arguments follow.


## /0/id? [name|desc|avr-gcc]

identify 

``` 
/1/id?
{"id":{"name":"Adc","desc":"Irrigate7 (14320^5) Board /w atmega1284p","avr-gcc":"5.4.0"}}
```

##  /0/analog? 0..7[,0..7[,0..7[,0..7[,0..7]]]]    

Analog-to-Digital Converter reading from up to 5 ADMUX channels. The reading repeats every 2 Seconds until the Rx buffer gets a character. On Irrigate7 channel 7 is the input voltage (PWR_V), channel 6 is the input current (PWR_I). channel 3, 2,  1, and 0 inputs can read up to 3.5V (higher voltages are blocked by a level shift). Channel 4 and 5 are used for switch mode power supply control.

``` 
/1/analog? 6,7
{"PWR_I":"0.019","PWR_V":"12.74"}
{"PWR_I":"0.017","PWR_V":"12.74"}
/1/analog? 0,1,2,3
{"ADC0":"0.00","ADC1":"0.00","ADC2":"1.96","ADC3":"1.55"}
/1/analog? 4,5
{"ADC4":"SMPS","ADC5":"SMPS"}
```

The value reported is based on the referance value which is saved in EEPROM, see bellow.


##  /0/avcc 4500000..5500000

Calibrate the AVCC reference in microvolts. This is the reference used for all measurements.

``` 
/1/avcc 5009000
# do this with the SelfTest
``` 


##  /0/onevone 900000..1300000

Set the 1V1 internal bandgap reference in microvolts.

```
/1/onevone 1100000
# do this with the SelfTest
``` 


##  /0/reftoee

Save the reference in static memory to EEPROM.

```
/1/reftoee
# do this with the SelfTest
```


##  /0/reffrmee

Load the reference from EEPROM into static memory.

```
/1/reffrmee
{"REF":{"extern_avcc":"4.9997","intern_1v1":"1.0693",}}
```

