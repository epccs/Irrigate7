# Day-Night State Machine

## Overview

Use Irrigate7's photovoltaic voltage (PV_V on ADC channel 6) to approximately tell when it is day (or night). 

Note: Solenoid uses the Day_AttachDayWork() function to set a callback that will be run at the start of each day. This command line interface was used to debug the day-night state machine.

## Firmware Upload

With a serial port connection (set the BOOT_PORT in Makefile) and optiboot installed on the Irrigate7 run 'make bootload' and it should compile and then flash the MCU.

``` 
rsutherland@conversion:~/Samba/Irrigate7/DayNight$ make bootload
...
avrdude done.  Thank you.
``` 

Now connect with picocom (or ilk). Note I am often at another computer doing this through SSH. The Samba folder is for editing the files from Windows.

``` 
#exit is C-a, C-x
picocom -b 38400 /dev/ttyUSB0
``` 


# Commands

Commands are interactive over the serial interface at 38400 baud rate. The echo will start after the second character of a new line. 

## /[rpu_address]/[command [arg]]

rpu_address is taken from the I2C address 0x29 (e.g. ../Uart/id.h get_Rpu_address() ). The value of rpu_address is used as a character in a string, which means don't use a null value (C strings are null terminated), but the ASCII value for '1' (0x31) is easy and looks nice, though I fear it will cause some confusion when it is discovered that the actual address value is 49.

The LED_BUILTIN is bliked fast (twice per second) if the I2C address is not found, also the rpu_address defaults to '0'. 

Commands and their arguments follow.


## /0/id? [name|desc|avr-gcc]

Identify is from ../Uart/id.h Id().

``` 
/1/id?
{"id":{"name":"DayNight","desc":"Irrigate7 Board /w atmega1284p and LT3652","avr-gcc":"4.9"}}
```


##  /0/day?

Reports status of the state machine every 60 seconds.

``` 
/1/day?
{"day_state":"DAY"}
{"day_state":"DAY"}
{"day_state":"DAY"}
... 11 hr
{"day_state":"DAY"}
{"day_state":"EVENING"}
... 15 min
{"day_state":"EVENING"}
TurnOnLED's
{"day_state":"NIGHT"}
... 12 hr
{"day_state":"NIGHT"}
{"day_state":"MORNING"}
... 15 min
{"day_state":"MORNING"}
WaterTheGarden
{"day_state":"DAY"}
... 20 hr (e.g. using a power supply)
{"day_state":"DAY"}
{"day_state":"FAIL"}
```



