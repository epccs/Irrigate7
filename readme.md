# Irrigate7 

From <https://github.com/epccs/Irrigate7/>

## Overview

This ATmega1284p based controller board has seven half H-bridge and a common half H-bride to control seven Latching (or Holding) Solenoids, a Capture input for timing pulse events (e.g. turbine rotation of a flow meter), and four Analog inputs with current sources to control and measure loop sensors (e.g. pressure, temperature...). Power with 7 thru 36V DC.

It is similar to the [RPUno] but has integrated K7 (which is a scaled up [K3]). It's MCU is connected to headers that work with the [RPUpi], or [RPUadpt] shields. The bootload port works with [USBuart].

[K3]: https://github.com/epccs/Driver/tree/master/K3
[RPUno]: https://github.com/epccs/RPUno
[RPUpi]: https://github.com/epccs/RPUpi
[RPUadpt]: https://github.com/epccs/RPUadpt
[USBuart]: https://github.com/epccs/Driver/tree/master/USBuart

[Forum](https://rpubus.org/bb/viewforum.php?f=17)

[HackaDay](https://hackaday.io/project/25599-irrigate7-m1284p-board-for-holding-solenoids)

## Status

[Available](https://rpubus.org/Order_Form.html)

![Stock](./Hardware/M_Stock.jpg "Stock")

[![Build Status](https://travis-ci.org/epccs/Irrigate7.svg?branch=master)](https://travis-ci.org/epccs/Irrigate7)

![Status](./Hardware/status_icon.png "Status")

## [Hardware](./Hardware)

Hardware files include schematic, bill of materials, and notes for testing and evaluation. The [Eagle] files are kept in a different location.

[Eagle]: https://github.com/epccs/Eagle/tree/master/Irrigate7

## Example with RPU BUS 

RPUBUS is a serial bus that allows multiple controller boards to be connected to a host computer. An [RPUpi] shield has a connector for a Pi Zero to act as the host computer, and connect to other RPUbus boards with a daisy-chain. 

![MultiDrop](./Hardware/Documents/MultiDrop.png "RPUno MultiDrop")

The host computer has a full duplex multi-drop serial link that works with common serial software (e.g., avrdude, Python's PySerial, picocom, and others.) 


## AVR toolchain

The core files for this board are in the /lib folder. Each example has its files and a Makefile in its own folder. The toolchain packages that I use are available on Ubuntu and Raspbian. 

```
sudo apt-get install make git gcc-avr binutils-avr gdb-avr avr-libc avrdude
git clone https://github.com/epccs/Irrigate7
```

* [gcc-avr](https://packages.ubuntu.com/search?keywords=gcc-avr)
* [binutils-avr](https://packages.ubuntu.com/search?keywords=binutils-avr)
* [gdb-avr](https://packages.ubuntu.com/search?keywords=gdb-avr)
* [avr-libc](https://packages.ubuntu.com/search?keywords=avr-libc)
* [avrdude](https://packages.ubuntu.com/search?keywords=avrdude)

I place an [Bootloader] on the bare metal microcontroller with a fuse step and a step that uploads using an ISP tool. 

[Bootloader]: https://github.com/epccs/Irrigate7/tree/master/Bootloader

```
cd Irrigate7/Bootloader
# note /dev/ttyACM0 is my ICSP tool.
make fuse
make isp
```

The other applications are loaded through the bootloader using the host serial port. Note that the fuse cannot be changed with the bootloader thus reducing user issues with an application upload.

```
cd ~/Irrigate7/Adc
# note /dev/ttyUSB0 is a FTDI USBuart, and /dev/ttyAMA0 is a Raspberry Pi
make bootload
```

The software is a guide, it is in C because that is my preference for microcontrollers. If you want additional software please add a Github issue to this repository where we can discuss it. 


## Irrigation Valves

The seven half bridges form an H-bridge with a single (common) bridge to direct a pulse of current through the coil in either a set or reset direction that can be used to control seven latching type solenoids. The valve voltage can be selected with software (9V, 12V, 24V). A boost converter draws 300mA from +5V (which is a 1A5 SMPS POL), so it does not disturb the power too much and charges the 2200uF reasonably quick. Once energized the boost SMPS is turned off, and the charge can be directed through one the seven coils in either a set or reset direction. Many latching valves will work with this setup, but try one first and start an issue if you want some help with the research.


## Pull Pulse Solenoids

A latching H-bridge driver is also used to operate Pull Pulse Solenoids.

http://www.deltaww.com/filecenter/Products/download/04/0409/DSML-0224.pdf

As well as other bi-stable pulse-operated circuits.

https://www.scoretronics.com/components/Digits_Brochure.pdf


## Flow Meter

I suggest using a flow meter with a hall sensor although a variable reluctance pickup with signal conditioner may also work. Wire it like a current loop device with all the returning current going into the 100 Ohm ICP1 sense resistor. When more than 6mA is on the sense resistor, a transistor will pull down the ICP1 pin to measure the flow pulse event. There are many ways to make this work, if you have a flow meter in mind and need some help with the research, then start a Github issue.

https://github.com/epccs/Document/tree/master/FlowMeter
