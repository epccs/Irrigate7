# Capture 

## Overview

Timer 1 Input Capture (ICP1) is on Irrigate7 Digital I/O 8. 

Referance datasheet Input Capture Unit. A capture copies the value in timer TCNT1 into ICR1, which is a timestamp of the event.

Note the ISR needs about 300 machine cycles to finish. If events happen faster than 300 machine cycles, they may be skipped.

## Firmware Upload

With a serial port connection (see BOOTLOAD_PORT in Makefile) and xboot installed on the Irrigate7 run 'make bootload' and it should compile and then flash the MCU.

```
git clone https://github.com/epccs/Irrigate7/
cd /Irrigate7/Capture
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

## /0/id? [name|desc|avr-gcc]

identify 

``` 
/1/id?
{"id":{"name":"Capture","desc":"Irrigate7 (14320^5) Board /w atmega1284p","avr-gcc":"5.4.0"}}
```

## /0/initICP icp1,mode,prescale

Initialize Input Capture ICP1. Set the Input Capture Edge Select mode: rise, fall, both. Set the prescale: 0 = no clock, 1 = MCU clock, 2 = MCU clock/8, 3 = MCU clock/64, 4 = MCU clock/256, and 5 = MCU clock/1024.

``` 
/1/initICP icp1,both,1
{"icp1":{"edgSel":"both", "preScalr":"1"}}
```

## /0/toggleicp icp1,0..255

Toggle the CS_ICP1_EN a number of times to test the ICP1 input. The capture time stamps are the software loop times.

``` 
/1/toggleicp icp1,31
{"icp1":{"toggle_CS_ICP1_EN":"31"}}
``` 

Note: CS_ICP1_EN starts with a LOW and ends with a LOW, so the count may be off by one.


## /0/count? [icp1]

Return count of ICP1 (ATmega1284 pin PD6) event captures. A zero means icp1 captures are not happening.

``` 
/1/count? icp1
{"icp1":{"count":"31"}}
```

## /0/capture? [icp1,1..15] 

return ICP1 timer delta(s) as a pair of low and high timing values from the buffered capture events. These can be used to find the duty or period. The count is the number of event captures. Note that a group of consecutive captures is, in fact, continous, there are no timing gaps. The resolution of continuous timing events can approach that of the stability of the timing sources (both timing sources have to be stable to gain maximum resolution). Pulse interpolation is a way of using a fast time source to measure a slow time source, but the fast time source occurs in buckets (e.g. quantum) and to measure something with those buckets to 100ppm requires 10k of them. 

``` 
/1/capture? icp1,10
{"icp1":{"count":"31","low":"693","high":"550","status":"1"}}
{"icp1":{"count":"29","low":"865","high":"551","status":"1"}}
{"icp1":{"count":"27","low":"698","high":"551","status":"1"}}
{"icp1":{"count":"25","low":"698","high":"551","status":"1"}}
{"icp1":{"count":"23","low":"698","high":"551","status":"1"}}
{"icp1":{"count":"21","low":"698","high":"551","status":"1"}}
{"icp1":{"count":"19","low":"698","high":"551","status":"1"}}
{"icp1":{"count":"17","low":"698","high":"551","status":"1"}}
{"icp1":{"count":"15","low":"698","high":"551","status":"1"}}
{"icp1":{"count":"13","low":"698","high":"551","status":"1"}}
```

It takes three events to aggregate the data for a capture report. The status reports only the most recent event status since the other two events can be inferred. 

## /0/event? [icp1,1..31] 

return ICP1 event timer values as a 16 bit unsigned integer, which continuously rolls over. The status bit 0 shows rising (0 is falling) edge.

``` 
/1/event? icp1,21
{"icp1":{"count":"31","event":"1637","status":"1"}}
{"icp1":{"count":"30","event":"944","status":"0"}}
{"icp1":{"count":"29","event":"394","status":"1"}}
{"icp1":{"count":"28","event":"65065","status":"0"}}
{"icp1":{"count":"27","event":"64514","status":"1"}}
{"icp1":{"count":"26","event":"63816","status":"0"}}
{"icp1":{"count":"25","event":"63265","status":"1"}}
{"icp1":{"count":"24","event":"62567","status":"0"}}
{"icp1":{"count":"23","event":"62016","status":"1"}}
{"icp1":{"count":"22","event":"61318","status":"0"}}
{"icp1":{"count":"21","event":"60767","status":"1"}}
{"icp1":{"count":"20","event":"60069","status":"0"}}
{"icp1":{"count":"19","event":"59518","status":"1"}}
{"icp1":{"count":"18","event":"58820","status":"0"}}
{"icp1":{"count":"17","event":"58269","status":"1"}}
{"icp1":{"count":"16","event":"57571","status":"0"}}
{"icp1":{"count":"15","event":"57020","status":"1"}}
{"icp1":{"count":"14","event":"56322","status":"0"}}
{"icp1":{"count":"13","event":"55771","status":"1"}}
{"icp1":{"count":"12","event":"55073","status":"0"}}
{"icp1":{"count":"11","event":"54522","status":"1"}}
```


# Notes

## Pulse Skipping

With an [HT] sensor as the temperature goes up the high count goes down for example near Room temp I see 

[HT]: https://github.com/epccs/LoopSensor/tree/master/HT

``` 
{"icp1":{"count":"519107161","low":"1669","high":"550","status":"0"}}
``` 

As the heat is turned up the capture looks like 

``` 
{"icp1":{"count":"514717561","low":"1646","high":"199","status":"0"}}
``` 

and some start to show skipping

``` 
{"icp1":{"count":"514717563","low":"1645","high":"2045","status":"0"}}
``` 

The short pulse is the one that skips because the ISR is not able to change edge detection in time to see the falling edge of the short high pulse so it has to wait for the next falling edge. It takes about 50 machine cycles to enter an interrupt (since all registers used have to be preserved) and then a similar amount of time to restore the interrupted process to its original state. 

Unfortunately, it is the other ISR's like Timer 0 (zero) used for millis() timing that is causing the skips. I need those interrupts to run so this is going to be something I will live with. The minimum pulse width that will ensure a capture is about 50 counts more than the longest interrupt, I am not seeing skips on pulses above 300 counts (18.75 uSec). The program will still gather data down to about 130 counts but some skipping will show up need tossed (they are sampling errors, e.g. outlier). 

Some sensors like flow meters do not need both edges tracked, so the capture can be initialized to track one edge, which may allow pulse events to be gathered at up to about 30kHz. The "capture?" command will not work with only one edge.
