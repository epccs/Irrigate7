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
CS next to RX1 on R1: 0.022 A
   Now open current source next to RX1
ADC0 R1 /W all CS off: 0.000 V
ADC1 at ICP1 with CS_ICP1_EN off: 0.000 V
ADC2 on green LED floating: 1.523 V
ADC3 on yellow LED with TX1 sinking and CS2_EN off: 0.000 V
ICP1 input should be HIGH with 0mA loop current: 1 
CS0 on R1: 0.022 A
CS1 source on R1: 0.022 A
   ADC0 reading used to calculate ref_intern_1v1_uV: 708 of 1023
   calculated ref_intern_1v1_uV: 1066320 uV
REF_EXTERN_AVCC old value found in eeprom: 4999700 uV
REF_INTERN_1V1 old value found in eeprom: 1069341 uV
REF_EXTERN_AVCC from eeprom is same
PWR_I at no load use INTERNAL_1V1: 0.018 A
CS2 source on R1: 0.022 A
Yellow LED D4 fwd /w CS2 V: 2.021 V
TX1 shunting CS2: 0.015 A
CS3 source on R1: 0.022 A
RX1 shunting CS3: 0.015 A
CS_ICP1 in ICP1 input: 0.019 A
Green LED D1 fwd /w CS_ICP1 V: 2.773 V
ICP1 /w 17mA on termination reads: 0 
Boost feedback /w shutdown/hack ON from ADC5 V: 3.042 V
BOOST9V@10mSec: 4.616 V
BOOST9V@2000mSec: 9.125 V
BOOST12V@10mSec after K1 SET discharge: 4.944 V
BOOST12V@2000mSec: 12.007 V
BOOST24V@10mSec after K1 RESET discharge: 4.865 V
BOOST24V@2000mSec: 23.859 V
BOOST24V@10mSec after K2 SET discharge: 4.865 V
BOOST24V@10mSec after K2 RESET discharge: 4.772 V
BOOST24V@10mSec after K3 SET discharge: 4.959 V
BOOST24V@10mSec after K3 RESET discharge: 4.678 V
BOOST24V@10mSec after K4 SET discharge: 4.865 V
BOOST24V@10mSec after K4 RESET discharge: 4.772 V
BOOST24V@10mSec after K5 SET discharge: 4.865 V
BOOST24V@10mSec after K5 RESET discharge: 4.772 V
BOOST24V@10mSec after K6 SET discharge: 4.772 V
BOOST24V@10mSec after K6 RESET discharge: 4.865 V
BOOST24V@10mSec after K7 SET discharge: 4.959 V
BOOST24V@10mSec after K7 RESET discharge: 4.959 V
[PASS]
``` 
