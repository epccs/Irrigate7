# Irrigate7 

From <https://github.com/epccs/Irrigate7/>

## Overview

This ATmega1284p based controller board is programmable with the open source GCC toolchain for AVR. It has seven half H-bridge and a common half H-bride to control seven Latching (or Holding) Solenoids, a Capture input for timing pulse events (e.g. turbine rotation of a flow meter), and four Analog inputs with current sources to control and measure loop sensors (e.g. pressure, temperature...). Power with 7 thru 36V DC.

It is similar to the [RPUno] but has integrated K7 (which is a scaled up [K3]). It's MCU is connected to headers that work with the [RPUpi], or [RPUadpt] shields.

[K3]: https://github.com/epccs/Driver/tree/master/K3
[RPUno]: https://github.com/epccs/RPUno
[RPUpi]: https://github.com/epccs/RPUpi
[RPUadpt]: https://github.com/epccs/RPUadpt

[Forum](https://rpubus.org/bb/viewforum.php?f=17)

[HackaDay](https://hackaday.io/project/25599-irrigate7-a-solar-powered-atmega1284p-board)

## Status

![Status](./Hardware/status_icon.png "Status")

## [Hardware](./Hardware)

Hardware files include schematic, bill of materials, and various notes for testing and evaluation. The [Eagle] files are kept in a different location.

[Eagle]: https://github.com/epccs/Eagle/tree/master/Irrigate7

## Example with RPU BUS 

A serial bus that allows multiple controller boards to be connected to a host computer serial port. An [RPUpi] shield has a connector for a Pi Zero to act as the host computer, and connect to other boards with a [RPUadpt] shield over a daisy-chain of CAT5 cables. 

![MultiDrop](./Hardware/Documents/MultiDrop.png "RPUno MultiDrop")

The host computer is able to use it's serial UART port with common software (e.g. avrdude, PySerial, picocom...) when communicating with the ATmega1284p. These programs don't need to known how to control the differential transceivers as would normally be necessary.


## AVR toolchain

The core files for this board are in the /lib folder. Each example has its files and a Makefile in its own folder. The toolchain is available as standard packages on Ubuntu, Raspbian, Windows 10 (with [WSL] installed), and probably Mac (I do not have a Mac). 

[WSL]: https://docs.microsoft.com/en-us/windows/wsl/install-win10

```
sudo apt-get install git make gcc-avr binutils-avr gdb-avr avr-libc avrdude
git clone https://github.com/epccs/Irrigate7
cd Irrigate7/Adc
make
#uplaod with: make bootload
```

* [gcc-avr](https://packages.ubuntu.com/search?keywords=gcc-avr)
* [binutils-avr](https://packages.ubuntu.com/search?keywords=binutils-avr)
* [gdb-avr](https://packages.ubuntu.com/search?keywords=gdb-avr)
* [avr-libc](https://packages.ubuntu.com/search?keywords=avr-libc)
* [avrdude](https://packages.ubuntu.com/search?keywords=avrdude)

Use my software as guide to do your own, I use C because that is what works for me. The toolchain has been marvelous, but your mileage may vary. 
