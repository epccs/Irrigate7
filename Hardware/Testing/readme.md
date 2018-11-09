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
{ "LEDON_V":[10.7,],
  "PWR@7V_mA":[0.2,],
  "PWR@30V_mA":[1.9,]}
```


## K7 Boost Bias 5V

Setup a current limited supply with 5V and about 30mA limit. Connect the supply to BOOST_IN (J8 pin 2) and 0V (J8 pin 3) to the supply. Measure the input current bias while disabled.

Note: C105 (2200uF) is not on board.

``` 
{"BOOST_DISABLED_mA":[0.01,] }
``` 


## Bias +5V

Apply a 30mA current limited 5V source to +5V (J8 pin 4) and source return to 0V (J8 pin 3). Check that the input current is for a blank MCU (e.g. less than 5mA). Turn off the power.

```
{ "I_IN_BLANKMCU_mA":[4.5,]}
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

Connect a 5V supply with CC mode set at 30mA to the +5V (J8 pin 4) and  0V (J8 pin 3). Connect the ISP tool (J13). The MCU needs its fuses set, so run the Makefile rule to do that. 

```
make fuse
```

Next install the bootloader

```
make isp
```

Disconnect the ICSP tool and measure the input current, wait for the power to be settled. Turn off the power.

```
{ "I_IN_16MHZ_EXT_CRYST_mA":[23.5,]}
```


## Install SMPS

Install U2 and measure its output voltage and input current with the supply set at 12.8V and a 30mA current limit.

```
{ "+5V_V":[5.0001,],
  "I_IN_@12V8_mA":[13.5,]}
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

Connect ICP1 (J8) jumper. Connect the Self Test [Harness] to the UUT. Connect 100 kOhm resistor to both the PV side and BAT side thermistor inputs to simulate room temperature. Connect a 12V SLA battery to the +BAT and -BAT. Connect +PV and -PV to a CC/CV mode supply with CC set at 150mA and  CV set at 0V. Apply power and increase the CV setting to 21V.

[Harness]: https://raw.githubusercontent.com/epccs/RPUno/master/SelfTest/Setup/SelfTestWiring.png

Once the UUT connects power (battery charged to > 13.1V) check that the VIN pin on the shield has power (this is not tested by the self-test so it has to be done manually).

Measure the +5V supply at J7 pin 6 and pin 5.

```
{ "+5V":[4.9959,4.9612,] }
```

Edit the SelfTest main.c such that "#define REF_EXTERN_AVCC 4995900UL" has the correct value for the UUT. Next, run the bootload rule in the Makefile to upload the self-test firmware to the UUT that the remote shield is mounted on.

```
cd ~Irrigate7/SelfTest
gedit main.c
make bootload
# toss the change (if you want)
git checkout -- main.c
```

Use picocom to see the SelfTest results over its UART interface.


```
rsutherland@conversion:~/Irrigate7/SelfTest$ picocom -b 38400 /dev/ttyUSB0
picocom v1.7

port is        : /dev/ttyUSB0
flowcontrol    : none
baudrate is    : 38400
parity is      : none
databits are   : 8
escape is      : C-a
local echo is  : no
noinit is      : no
noreset is     : no
nolock is      : no
send_cmd is    : sz -vv
receive_cmd is : rz -vv
imap is        : 
omap is        : 
emap is        : crcrlf,delbs,

Terminal ready
Irrigate7 DIO and ADC Self Test date: Jun 25 2017
I2C provided address 0x31 from RPU bus manager
+5V needs measured and then set as REF_EXTERN_AVCC: 4.996 V
Charging with CURR_SOUR_EN==low: 0.040 A
PWR (Battery) at: 13.275 V
MPPT at: 16.897 V
ADC0 at: 0.000 V
ADC1 at: 0.000 V
ADC4 at: 0.000 V
ADC5 at: 0.000 V
ICP1 /w 0mA on plug termination reads: 1
ICP3 /w 0mA on plug termination reads: 1
CC_nFAULT measured with a weak pull-up: 1
22MA@DIO10 curr source on R1: 0.021 A
22MA@DIO2 curr source on R2: 0.021 A
Dischrg with CURR_SOUR_EN==high: 0.046 A
Chrg delta with CURR_SOUR_EN==high: 0.086 A
22MA@ADC0 on R1: 0.022 A
22MA@ADC5 on R2: 0.022 A
10MA@ICP1 with ICP1 PL input: 0.010 A
ICP1 /w 10mA on plug termination reads: 0
10MA@ICP3 with ICP3 PL input: 0.010 A
ICP3 /w 10mA on plug termination reads: 0
Dischrging with CC_SHUTDOWN==high : 0.141 A
PV open circuit (LT3652 off) at: 20.972 V
22mA@ADC0 and 22mA@ADC1 on R1: 0.044 A
22MA@DIO10, 22mA@ADC0, 22mA@ADC1 on R1: 0.065 A
22mA@ADC4 and 22mA@ADC5 on R2: 0.043 A
22MA@DIO2, 22mA@ADC4, 22mA@ADC5 on R2: 0.065 A
ICP1 10mA + 17mA curr source on ICP1_TERM: 0.028 A
ICP1 10mA with DIO4 sinking 17mA: 0.010 A
ICP1 10mA with DIO3 sinking 17mA: 0.010 A
ICP3 10mA + 17mA curr source on ICP3_TERM: 0.028 A
ICP1 10mA with DIO4 sinking 17mA: 0.010 A
Dischrg /w CC_SHUTDOWN, !K3_E3, !CURR_SOUR_EN: 0.046 A
Dischrg@100mSec /w CC_SHUTDOWN, K3_E3, !CURR_SOUR_EN: 0.050 A
Dischrg@300mSec /w CC_SHUTDOWN==high, K3_E3==hight: 0.118 A
Dischrg@500mSec /w CC_SHUTDOWN==high, K3_E3==hight: 0.123 A
Dischrg@2000mSec /w CC_SHUTDOWN==high, K3_E3==hight: 0.069 A
BOOST@2000mSec: 24.110 V
To disconnect battery turn off the PV supply and LED should stop blinking
[PASS]
```

Before truning off the PV power check that the VIN pin on the shield has no power, the test turns it off. Then turn off the power supply and verify battery was disconnected.

Run the SelfTest for each of the three voltage settings and measure the value with a DMM.

```
{ "9VBOOSTLD_V":[9.2,9.1,],
"12VBOOST_V":[12.2,12.1,],
"24VBOOST_V":[24.1,24.1,]}
```

## Solenoid Test

Items.

![Irrigate7 Solenoid Test Items](./14320,SolenoidTest.jpg "Irrigate7 Solenoid Test Items")

Connect the [Solenoid Test Harness] to the UUT. Connect 100 kOhm resistor to both the PV side and BAT side thermistor inputs to simulate room temperature. Connect a 12V SLA battery to the +BAT and -BAT. Connect +PV and -PV to a CC/CV mode supply with CC set at 150mA and  CV set at 0V. Apply power and increase the CV setting to 21V.

[Solenoid Test Harness]: https://raw.githubusercontent.com/epccs/RPUno/master/SelfTest/Setup/SelfTestWiring.png


```
cd ~Irrigate7/Solenoid
make bootload
```

Use picocom to see the Solenoid initialization on the [Solenoid Test Harness] LED's

```
rsutherland@conversion:~/Irrigate7/Solenoid$ picocom -b 38400 /dev/ttyUSB0
picocom v1.7
```
