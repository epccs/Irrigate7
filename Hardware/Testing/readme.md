# Description

This is a list of Test preformed on each Irrigate7 after assembly.

# Table of References


# Table Of Contents:

1. [Basics](#basics)
2. [Assembly check](#assembly-check)
3. [IC Solder Test](#ic-solder-test)
6. [Power Protection](#power-protection)
7. [Power Without SMPS](#power-without-smps)
9. [K7 Bias 5V](#k7-bias-5v)
10. [Bias +5V](#bias-5v)
12. [Set MCU Fuse and Install Bootloader](#set-mcu-fuse-and-install-bootloader)
13. [Install SMPS](install-smps)
14. [Self Test](#self-test)


## Basics

These tests are for an assembled Irrigate7 board 14320^5 which may be referred to as a Unit Under Test (UUT). If the UUT fails and can be reworked then do so, otherwise it needs to be scraped. 

**Warning: never use a soldering iron to rework ceramic capacitors due to the thermal shock.**
    
Items used for test.

![ItemsUsedForTest](./14320,ItemsUsedForTest.jpg "Irrigate7 Items Used For Test")


## Assembly check

After assembly check the circuit carefully to make sure all parts are soldered and correct, note that the device making is labeled on the schematic and assembly drawing.
    
NOTE: U2 is not yet on the board, so +5V will not have power. C105 (2200uF) is also not on board.


## IC Solder Test

U2 is not yet populated. Check that a diode drop to 0V is present from a circuit board pad that is connected to each of the pins of U1, and U3 by measuring with a DMM's diode test set up for reversed polarity. Consult the schematic to determine which pins can be skipped (e.g. ground, power rail, ...).

This is a simplified In-Circuit Test (ICT). It could become more valuable if the node voltage driven with the current source is recorded for each tested location and then used with statistics to determine test limits for each location. 


## Power Protection

Apply a current limited (20mA) supply set with 5V to the PWR and 0V connector J1 in reverse and verify that the voltage does not get through. Adjust the supply to 36V and verify no current is passing.


## Power Without SMPS

Apply a current limited (20mA) supply set with 7V to the PWR and 0V connector J1 and verify that the voltage does get through. Adjust the supply so the LED is on and stable and measure voltage, adjust supply to 30V measure input current. 

```
{ "LEDON_V":[10.7,10.7,10.7,],
  "PWR@7V_mA":[0.2,0.2,0.2,],
  "PWR@30V_mA":[1.9,1.9,1.9,]}
```


## K7 Boost Bias 5V

Setup a current limited supply with 5V and about 30mA limit. Connect the supply to BOOST_IN (J8 pin 2) and 0V (J8 pin 3) to the supply. Measure the input current bias while disabled.

Note: C105 (2200uF) is not on board.

``` 
{"BOOST_DISABLED_mA":[0.01,0.01,0.01,] }
``` 


## Bias +5V

Apply a 30mA current limited 5V source to +5V (J8 pin 4) and source return to 0V (J8 pin 3). Check that the input current is for a blank MCU (e.g. less than 5mA). Turn off the power.

```
{ "I_IN_BLANKMCU_mA":[4.5,5.1,5.5,]}
```

Note if the fuse needs set to the factory (OEM) values there is a make rule in the Bootloader folder.


## Set MCU Fuse and Install Bootloader

Install Git and AVR toolchain on Ubuntu (18.04, on an old computer try https://wiki.ubuntu.com/Lubuntu). 

```
sudo apt-get install git gcc-avr binutils-avr gdb-avr avr-libc avrdude
```

Clone the Irrigat7 repository.

```
cd ~
git clone https://github.com/epccs/Irrigat7
cd ~/Irrigat7/Bootloader
```

Connect a 5V supply with CC mode set at 40mA to the +5V (J8 pin 4) and  0V (J8 pin 3). Connect the ISP tool (J13). The MCU needs its fuses set, so run the Makefile rule to do that. 

```
make fuse
```

Next install the bootloader

```
make isp
```

Disconnect the ICSP tool and measure the input current, wait for the power to be settled. Turn off the power.

```
{ "I_IN_16MHZ_EXT_CRYST_mA":[23.5,26.0,26.0,]}
```


## Install SMPS

Install U2 and measure its output voltage and input current with the supply set at 12.8V and a 40mA current limit.

```
{ "+5V_V":[5.0001,4.9414,4.9173,],
  "I_IN_@12V8_mA":[13.5,13.7,13.9,]}
```


## Self Test

Items.

![Irrigate7 SelfTest Items](./14320,SelfTest.jpg "Irrigate7 SelfTest Items")

Plug an [RPUftdi] shield with [Host2Remote] firmware onto an [RPUno] board and load [I2C-Debug] on it.

[RPUftdi]: https://github.com/epccs/RPUftdi
[Host2Remote]: https://github.com/epccs/RPUftdi/tree/master/Host2Remote
[RPUno]: https://github.com/epccs/RPUno
[I2C-Debug]: https://github.com/epccs/RPUno/tree/master/i2c-debug

Use picocom to set the bootload address on the RPUftdi shield. The RPUftdi is at address 0x30 and the UUT will be at address 0x31.

```
picocom -b 38400 /dev/ttyUSB0
...
Terminal ready
/0/iaddr 41
{"address":"0x29"}
/0/ibuff 3,49
{"txBuffer":[{"data":"0x3"},{"data":"0x31"}]}
/0/iread? 2
{"rxBuffer":[{"data":"0x3"},{"data":"0x31"}]}
```

Exit picocom (Cntl^a and Cntl^x). Plug an [RPUadpt] shield with [Remote] firmware onto the UUT board. Note the RPUadpt address defaults to 0x31 when its firmware was installed.

[RPUadpt]: https://github.com/epccs/RPUadpt
[Remote]: https://github.com/epccs/RPUadpt/tree/master/Remote

Connect the Self Test [Harness] to the UUT. Set supply 12.8V and a 400mA current limit.

[Harness]: https://raw.githubusercontent.com/epccs/RPUno/master/SelfTest/Setup/SelfTestWiring.png

Edit the SelfTest main.c such that "#define REF_EXTERN_AVCC 4999700UL" has the correct value for the UUT (see previous test). Next, run the bootload rule in the Makefile to build and upload the self-test firmware to the UUT.

```
cd ~Irrigate7/SelfTest
gedit main.c
make bootload
# toss the change (if you want)
git checkout -- main.c
```

Use picocom to see the SelfTest results over its UART interface.


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

