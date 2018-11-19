# Self-Test

## Overview

Check Irrigate7 Board Functions. This runs once after a reset and then loops to blink the LED's.

For a quick idea of what is going on lets look at R1 (33.3 Ohm) which has the 22mA current sources (CS0..CS3) feeding through it. R1 voltage is measured with ADC0. The 17mA current source (CS_ICP1) is sent into the ICP1 input which has a 100 Ohm resistor. The ICP1 resistor voltage is measured with ADC1.

The red and green LED are used to indicate test status.

## Wiring Needed for SelfTest

![Wiring](./Setup/SelfTestWiring.png)

## Power Supply

Connect a power supply with CV and CC mode. Set CC at 300mA then increase CV to 12V8. 

## Firmware Upload

With a serial port connection (set the BOOT_PORT in Makefile) and xboot installed on the Irrigate7 run 'make bootload' and it should compile and then flash the MCU.

``` 
make bootload
...
avrdude done.  Thank you.
make clean
rm -f SelfTest.hex SelfTest.map
``` 

Connect with picocom (exit is C-a, C-x). 

```
picocom -b 38400 /dev/ttyUSB0
...
Terminal ready
Irrigate7 Self Test date: Nov 19 2018
avr-gcc --version: 5.4.0
I2C provided address 0x31 from RPUadpt serial bus manager
adc reading for PWR_V: 356
PWR at: 12.739 V
ADC0 R1 /W all CS off: 0.000 V
ADC1 at ICP1 with CS_ICP1_EN off: 0.000 V
ADC2 on green LED floating: 1.670 V
ADC3 on yellow LED with TX1 sinking and CS2_EN off: 0.000 V
ICP1 input should be HIGH with 0mA loop current: 1 
CS0 on R1: 0.022 A
CS1 source on R1: 0.022 A
   ADC0 reading used to calculate ref_intern_1v1_uV: 709 of 1023
   calculated ref_intern_1v1_uV: 1064816 uV
REF_EXTERN_AVCC old value found in eeprom: 4999700 uV
REF_INTERN_1V1 old value found in eeprom: 1069341 uV
REF_EXTERN_AVCC from eeprom is same
PWR_I at no load use INTERNAL_1V1: 0.017 A
CS2 source on R1: 0.022 A
Yellow LED D4 fwd /w CS2 V: 2.026 V
TX1 shunting CS2: 0.015 A
CS3 source on R1: 0.022 A
RX1 shunting CS3: 0.015 A
CS_ICP1 in ICP1 input: 0.019 A
Green LED D1 fwd /w CS_ICP1 V: 2.778 V
ICP1 /w 17mA on termination reads: 0 
Boost feedback /w shutdown/hack ON from ADC5 V: 3.100 V
BOOST9V@10mSec: 5.081 V
BOOST9V@2000mSec: 9.089 V
BOOST12V@10mSec: 5.227 V
BOOST12V@2000mSec: 11.960 V
BOOST24V@10mSec: 4.865 V
BOOST24V@2000mSec: 23.859 V
[PASS]
``` 
