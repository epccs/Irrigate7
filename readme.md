# Irrigate7 

From <https://github.com/epccs/Irrigate7/>

## Overview

This Solar Powerd Programmable Controller board has pluggable connections for seven Latching Solenoid Drivers, seven digital I/O, two Input Capture, and four Analog inputs with current sources to power four loops. Use a 3W or 20W PV string of 36 cells to power the 12V SLA charge controller that includes simple battery management. 

It is similar to the [RPUno] design but has some additional pluggable interfaces (e.g. IO2, ICP3, ADC4, ADC5). It's MCU is the ATmega1284p and has Arduino-style headers that work with the [RPUpi], [RPUftdi], or [RPUadpt] shields. The embedded LT3652 solar charge controller manages battery charging and the ATmega1284p interfaces with it (see Hardware for more details).

[RPUno]: https://github.com/epccs/RPUno
[RPUpi]: https://github.com/epccs/RPUpi
[RPUftdi]: https://github.com/epccs/RPUftdi
[RPUadpt]: https://github.com/epccs/RPUadpt

## Status

![Status](./Hardware/status_icon.png "Status")

## [Hardware](./Hardware)

Hardware files and notes for referance.

## Example with RPU BUS (RS-422)

A serial bus that allows multiple microcontroller boards to be connected to a host computer serial port. An [RPUpi] shield with an IDC connector for a single board computer (Pi Zero) that connects its UART to transceivers that can drive differential pair on an RS-422 bus. Remote Irrigate7 boards can connect to the differential pairs with an [RPUadpt] shield over a daisy-chain of up to perhaps 1000 meters of CAT5 cable (how many and how far is not known it depends on the CAT5 cable and many other factors). 

![MultiDrop](./Hardware/Documents/MultiDrop.png "RPUno MultiDrop")

The host computer is able to use it's RS-232 serial port programs (e.g. avrdude, PySerial, picocom...) when communicating with the ATmega1284p, and the MCU can use RS-232 UART control libraries (e.g. like the Arduino Uno core) to communicate with the host. These programs don't need to known how to control the differential transceivers as would normally be necessary. In part because RS-422 is full duplex like RS-232, but also the transceivers are automatically activated. The common UART libraries and host programs (e.g. picocom and PySerial) were done for RS-232, but RS-485 is a half duplex communication, so it is like a push to talk radio. Serial bootloaders like optiboot and xboot work well with avrdude over RS-232, and over RS-422 in a point to point mode with automatically activated transceivers. 

I prefer a textual Command Line Interface (CLI), so that is what the examples use. The CLI is programmed to respond to text commands terminated with a newline, so remember to press enter (which sends a newline) before starting an initial command to place the command processor into a known state. The command includes an address with a leading and trailing forward slash "/". The command echo starts after the address (second byte) is sent. The first byte ('/') will cause any transmitting device to stop and dump its outgoing buffer which should prevent collisions since the echo is delayed until after the second byte.

As a short example, I'll connect with SSH from a desktop computer (an old x86 with Lubuntu) that is on the network with the [RPUpi]'s host. These machines have matching usernames and keys placed so I don't need to use passwords. Then I will use picocom to interact with two different Irrigate7 boards. They are on the serial bus at addresses '1' and '0' (note that ASCII '1' is 0x31, and ASCII '0' is 0x30, so they have an address that looks good on picocom but is probably not what was expected).  

```
rsutherland@conversion:~ $ ssh raspberrypi.local
... TBD ...
rsutherland@raspberrypi:~$ picocom -b 38400 /dev/ttyAMA0
picocom v1.7

port is        : /dev/ttyAMA0
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
/1/id?
{"id":{"name":"Solenoid","desc":"Irrigate7 Board /w atmega1284p and LT3652","avr-gcc":"4.9"}}
/0/id?
{"id":{"name":"I2Cdebug","desc":"Irrigate7 Board /w atmega1284p and LT3652","avr-gcc":"4.9"}}
Ctrl-a,Ctrl-x 
Thanks for using picocom
```

At present, I'm using [I2Cdebug] to set the bus manager on the [RPUpi] shield, it needs to know which address to reset so that it can lockout the others during bootload. Solenoid is the star of the show (so far), it is an attempt to control latching irrigation valves with cycles (inspired by Vinduino) and start the cycle at a daylight based offset, flow sensing for each zone is also working.

[I2Cdebug]: ./i2c-debug

Since this is an interactive console application it can be automated with Pexpect which is a package for python. Expect is a old Unix tool for working with consoles (e.g. a login console). It can scan through the characters echoed during the interactive session and then select the JSON reply to feed the Python JSON decoder.

```
sudo apt-get install python-pexpect
```

When automation is used it is important to have very little framing error since the data is tightly crowded together one character after the next. The congestion leaves little room for the receiving UART to adjust for the next framing offset (this was causing problems on my 16MHz AVR at 115.2k bit/sec, but now I know non-standard bit rates with low framing error like 125k or 250k bit/sec can be used).

## AVR toolchain

The core files for this board are in the /lib folder. Each example has its files and a Makefile in its own folder. The toolchain packages that I use are available on Ubuntu and Raspbian. 

```
sudo apt-get install git gcc-avr binutils-avr gdb-avr avr-libc avrdude
git clone https://github.com/epccs/Irrigate7
```

* [gcc-avr](http://packages.ubuntu.com/search?keywords=gcc-avr)
* [binutils-avr](http://packages.ubuntu.com/search?keywords=binutils-avr)
* [gdb-avr](http://packages.ubuntu.com/search?keywords=gdb-avr)
* [avr-libc](http://packages.ubuntu.com/search?keywords=avr-libc)
* [avrdude](http://packages.ubuntu.com/search?keywords=avrdude)

I hope the software can act as a guide, it is in C because that is what I found works for me.
