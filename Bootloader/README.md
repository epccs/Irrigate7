# Serial Bootloader

## Overview

Microcontrollers are programmed through a Programmer interface (e.g. ISP). However, with this firmware loaded in the microcontroller it can receive the firmware (machine code) from an uploader tool (e.g. avrdude) over a serial link and place that firmware image in the flash memory without the programmer. This technique is called serial bootloading. 

Unfortunately to install a serial bootloader requires a programmer tool. Arduino has an ArduinoISP.ino sketch that I load into a separate ATmega328 board to use as the programmer tool. 

I use a two-step process. First run "make fuse" which will program LOCKBIT=3F to open the bootloader area and set fuses LOW=F7, HIGH=D0, EXTENDED=FD. The fuse setting will switch operation form the internal oscillator to an external 16MHz crystal if it is missing the MCU will be locked up. The second step is "make isp" which will load the bootloader and program LOCKBIT=2F, which should prohibit the Store Program Memory (SPM) instruction from running in the boot flash section. 

The bootloader is xboot, it is a submodule of the Irrigate7 superproject. To compile the bootlaoder go into the submodule and make the target.

```
cd [Irrigate7/]xboot
make conf/irrigate7.conf.mk
```

