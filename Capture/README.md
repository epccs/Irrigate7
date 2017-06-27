# Capture 

## Overview

Timer 1 Input Capture (ICP1) is on Irrigate7 Digital (Wiring) pin 8. 

Referance ATmega328 datasheet 16.6 Input Capture Unit (page 118). A capture copies the value in timer TCNT1 into ICR1, which is a timestamp of the event.

Note the ISR needs about 300 machine cycles to finish. If events happen faster than 300 machine cycles, they may be skipped.

## Firmware Upload

With a serial port connection (set the BOOT_PORT in Makefile) and optiboot installed on the Irrigate7 run 'make bootload' and it should compile and then flash the MCU.

``` 
rsutherland@conversion:~/Samba/Irrigate7/Capture$ make bootload
...
avrdude done.  Thank you.
``` 

Now connect with picocom (or ilk). Note I am often at another computer doing this through SSH. The Samba folder is for editing the files from Windows.

``` 
#exit picocom with C-a, C-x
picocom -b 38400 /dev/ttyUSB0
``` 

or log the terminal session

``` 
script -f -c "picocom -b 38400 /dev/ttyUSB0" stuff,capture.log
``` 


# Commands

Commands are interactive over the serial interface at 38400 baud rate. The echo will start after the second character of a new line. 

## /0/id? [name|desc|avr-gcc]

identify 

``` 
/1/id?
{"id":{"name":"Capture","desc":"Irrigate7 Board /w atmega1284p and LT3652","avr-gcc":"4.9"}}
```

## /0/initICP icp1,mode,prescale

Initialize Input Capture ICP1. Set the Input Capture Edge Select mode: rise, fall, both. Set the prescale: 0 = no clock, 1 = MCU clock, 2 = MCU clock/8, 3 = MCU clock/64, 4 = MCU clock/256, 5 = MCU clock/1024, 6 and 7 are for an external (IO5) clock source (see the datasheet). Note that IO5 is used for SHUTDOWN of the solar charge controler.

``` 
/1/initICP icp1,both,1
{"icp1":{"edgSel":"both", "preScalr":"1"}}
```

## /0/count? [icp1]

Return count of ICP1 (ATmega328 pin PB0. Uno pin 8) event captures. A zero means icp1 captures are not happening.

``` 
/1/count? icp1
{"icp1":{"count":"0"}}
{"icp1":{"count":"0"}}
{"icp1":{"count":"41"}}
{"icp1":{"count":"41"}}
{"icp1":{"count":"41"}}
{"icp1":{"count":"41"}}
{"icp1":{"count":"41"}}
{"icp1":{"count":"839"}}
{"icp1":{"count":"843"}}
{"icp1":{"count":"843"}}
{"icp1":{"count":"843"}}
{"icp1":{"count":"2052"}}
{"icp1":{"count":"2052"}}
{"icp1":{"count":"2052"}}
```

## /0/capture? [icp1,1..15] 

return ICP1 timer delta(s) as a pair of low and high timing values from the buffered capture events. These can be used to find the duty or period. The count is the number of event captures. Note that a group of consecutive captures is, in fact, continous, there are no timing gaps. The resolution of continuous timing events can approach that of the stability of the timing sources (both timing sources have to be stable to gain maximum resolution). Pulse interpolation is a way of using a fast time source to measure a slow time source, but the fast time source occurs in buckets (e.g. quantum) and to measure something with those buckets to 100ppm requires 10k of them. 

``` 
/1/capture? icp1,10
{"icp1":{"count":"2052","low":"517","high":"10486","status":"0"}}
{"icp1":{"count":"2050","low":"10575","high":"15829","status":"0"}}
{"icp1":{"count":"2048","low":"38908","high":"65423","status":"0"}}
{"icp1":{"count":"2046","low":"46217","high":"33219","status":"0"}}
{"icp1":{"count":"2044","low":"12040","high":"14514","status":"0"}}
{"icp1":{"count":"2042","low":"10893","high":"61232","status":"0"}}
{"icp1":{"count":"2040","low":"7757","high":"56660","status":"0"}}
{"icp1":{"count":"2038","low":"40104","high":"50118","status":"0"}}
{"icp1":{"count":"2036","low":"6376","high":"50197","status":"0"}}
{"icp1":{"count":"2034","low":"22773","high":"4255","status":"0"}}
{"icp1":{"count":"2052","low":"517","high":"10486","status":"0"}}
{"icp1":{"count":"2050","low":"10575","high":"15829","status":"0"}}
{"icp1":{"count":"2048","low":"38908","high":"65423","status":"0"}}
{"icp1":{"count":"2046","low":"46217","high":"33219","status":"0"}}
{"icp1":{"count":"2044","low":"12040","high":"14514","status":"0"}}
{"icp1":{"count":"2042","low":"10893","high":"61232","status":"0"}}
{"icp1":{"count":"2040","low":"7757","high":"56660","status":"0"}}
{"icp1":{"count":"2038","low":"40104","high":"50118","status":"0"}}
{"icp1":{"count":"2036","low":"6376","high":"50197","status":"0"}}
{"icp1":{"count":"2034","low":"22773","high":"4255","status":"0"}}
```

It takes three events to aggregate the data for a capture report. The status reports only the most recent event status since the other two events can be inferred. 

## /0/event? [icp1,1..31] 

return ICP1 event timer values as a 16 bit unsigned integer, which continuously rolls over. The status bit 0 shows rising (0 is falling) edge.

``` 
/1/event? icp1,21
{"icp1":{"count":"2052","event":"42475","status":"0"}}
{"icp1":{"count":"2051","event":"31989","status":"1"}}
{"icp1":{"count":"2050","event":"31472","status":"0"}}
{"icp1":{"count":"2049","event":"15643","status":"1"}}
{"icp1":{"count":"2048","event":"5068","status":"0"}}
{"icp1":{"count":"2047","event":"5181","status":"1"}}
{"icp1":{"count":"2046","event":"31809","status":"0"}}
{"icp1":{"count":"2045","event":"64126","status":"1"}}
{"icp1":{"count":"2044","event":"17909","status":"0"}}
{"icp1":{"count":"2043","event":"3395","status":"1"}}
{"icp1":{"count":"2042","event":"56891","status":"0"}}
{"icp1":{"count":"2041","event":"61195","status":"1"}}
{"icp1":{"count":"2040","event":"50302","status":"0"}}
{"icp1":{"count":"2039","event":"59178","status":"1"}}
{"icp1":{"count":"2038","event":"51421","status":"0"}}
{"icp1":{"count":"2037","event":"1303","status":"1"}}
{"icp1":{"count":"2036","event":"26735","status":"0"}}
{"icp1":{"count":"2035","event":"42074","status":"1"}}
{"icp1":{"count":"2034","event":"35698","status":"0"}}
{"icp1":{"count":"2033","event":"31443","status":"1"}}
{"icp1":{"count":"2032","event":"8670","status":"0"}}
```

## /0/pwm oc2a,0..255

Pulse width modulation using OC2A (on PD7, which is mapped to Digital pin 11) can be used to pull down the 10mA current source that is enabled with a jumper on the ICP1 PL input. Note that timer2 is used when OC2A is set, and timer1 is used with ICP1 capture, also timer0 is used with the millis() function that is used to provide various timing. 

``` 
/1/pwm oc2a,127
{"pwm":{"OCR2A":"127"}}
``` 

# Notes

## Pulse Skipping

With an HT as the temperature goes up the high count goes down for example near Room temp I see 

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
