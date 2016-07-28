# Charge Control test

## Overview

I made a CCtest board that has high side current sensing for the PV input and Battery charging and discharging. The high side sensor only measures one direction so two are needed for the battery. A load is also provided, it has four digital control lines that enable current sinks. 

At startup, the program steps through the current sink settings to record and make sure they are working. Then it gets stuck in a loop that reports the Battery, PV, and Charge Control  status. If the serial receives a character it will interrupt the test, but the test can be resumed see bellow.

For how I setup my Makefile toolchain <http://epccs.org/indexes/Document/DvlpNotes/LinuxBoxCrossCompiler.html>.

With a serial port connection (set the BOOT_PORT in Makefile) and optiboot installed on the RPUno run 'make bootload' and it should compile and then flash the MCU.

``` 
rsutherland@conversion:~/Samba/Irrigate7/Adc$ make bootload
``` 

Now connect with picocom (or ilk). Note I am often at another computer doing this through SSH. The Samba folder is for editing the files from Windows.

``` 
#exit is C-a, C-x
picocom -b 115200 /dev/ttyUSB0
``` 

or log the terminal session

``` 
script -f -c "picocom -b 115200 /dev/ttyUSB0" stuff.log
``` 


# Commands

Commands are interactive over the serial interface at 115200 baud rate. The echo will start after second charactor of a new line. 

## /0/id? [name|desc|avr-gcc]

identify 

``` 
/0/id?
{"id":{"name":"CCtest","desc":"Irrigate7 Board /w atmega1284p and LT3652","avr-gcc":"4.9"}}
```

##  /0/cctest?

Run the test, first it waits to enter float mode (PV_IN above 19V), then it will step through each load setting. Once at the max load the the charger is turned off and it waits for the voltage to drop (to bellow 6.5V) then the load is cleared and waits for the PV_IN to change from MPPT mode to float mode (to over 19V) again. Once in float the process repeats until receiving a character on the UART.

``` 
/0/cctest?
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.030","BOOST":"4.59","PV_IN":"0.78","PWR":"6.66","LDSTEP":"0"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.037","BOOST":"4.59","PV_IN":"0.78","PWR":"6.66","LDSTEP":"1"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.046","BOOST":"4.59","PV_IN":"0.75","PWR":"6.65","LDSTEP":"2"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.053","BOOST":"4.59","PV_IN":"0.75","PWR":"6.65","LDSTEP":"3"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.062","BOOST":"4.59","PV_IN":"0.75","PWR":"6.64","LDSTEP":"4"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.069","BOOST":"4.59","PV_IN":"0.75","PWR":"6.64","LDSTEP":"5"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.078","BOOST":"4.50","PV_IN":"0.75","PWR":"6.64","LDSTEP":"6"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.083","BOOST":"4.59","PV_IN":"0.75","PWR":"6.63","LDSTEP":"7"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.092","BOOST":"4.50","PV_IN":"0.73","PWR":"6.63","LDSTEP":"8"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.101","BOOST":"4.50","PV_IN":"0.73","PWR":"6.62","LDSTEP":"9"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.108","BOOST":"4.50","PV_IN":"0.73","PWR":"6.61","LDSTEP":"10"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.116","BOOST":"4.50","PV_IN":"0.73","PWR":"6.61","LDSTEP":"11"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.124","BOOST":"4.50","PV_IN":"0.73","PWR":"6.61","LDSTEP":"12"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.132","BOOST":"4.50","PV_IN":"0.73","PWR":"6.61","LDSTEP":"13"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.139","BOOST":"4.50","PV_IN":"0.73","PWR":"6.60","LDSTEP":"14"}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.50","PV_IN":"0.70","PWR":"6.59","LDSTEP":"15"}
```