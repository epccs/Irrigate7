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

##  /0/cctest? [load_step]|[start_step,end_step,bat_mv]

Zero or three argument test, first it waits to enter float mode (PV_IN above 19V), then it will step through each load setting with charger turned off to see the DISCHRG amps. Once at the end_step (default is 15) load, it waits for the voltage to drop bellow bat_mv (6500mV is default) then the load is set to start_step (default is 0) and waits for the PV_IN to change from MPPT mode to float mode (e.g. PV_IN is above 19V) again. Once in float, the process repeats until receiving a character on the UART. 

One argument test sets the load step value and runs with the charger off until receiving a character on the UART.

``` 

/0/cctest?
{"PV_I":"0.080","CHRG":"0.146","DISCHRG":"0.000","BOOST":"4.59","PV_IN":"16.63","PWR":"6.72","MILLIS":"57428","LDSTEP":"0",}
{"PV_I":"0.080","CHRG":"0.145","DISCHRG":"0.000","BOOST":"4.59","PV_IN":"16.63","PWR":"6.72","MILLIS":"59428","LDSTEP":"0",}
{"PV_I":"0.079","CHRG":"0.144","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"16.63","PWR":"6.74","MILLIS":"61429","LDSTEP":"0",}
{"PV_I":"0.079","CHRG":"0.144","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"16.65","PWR":"6.75","MILLIS":"63430","LDSTEP":"0",}
{"PV_I":"0.080","CHRG":"0.144","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"16.65","PWR":"6.76","MILLIS":"65431","LDSTEP":"0",}
{"PV_I":"0.076","CHRG":"0.136","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"16.88","PWR":"6.77","MILLIS":"67432","LDSTEP":"0",}
{"PV_I":"0.055","CHRG":"0.122","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"21.04","PWR":"6.77","MILLIS":"69433","LDSTEP":"0",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.037","BOOST":"4.59","PV_IN":"21.14","PWR":"6.67","MILLIS":"71434","LDSTEP":"1",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.045","BOOST":"4.59","PV_IN":"21.17","PWR":"6.65","MILLIS":"73436","LDSTEP":"2",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.053","BOOST":"4.59","PV_IN":"21.14","PWR":"6.64","MILLIS":"75437","LDSTEP":"3",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.060","BOOST":"4.59","PV_IN":"21.14","PWR":"6.63","MILLIS":"77437","LDSTEP":"4",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.069","BOOST":"4.50","PV_IN":"21.14","PWR":"6.62","MILLIS":"79438","LDSTEP":"5",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.076","BOOST":"4.50","PV_IN":"21.14","PWR":"6.61","MILLIS":"81439","LDSTEP":"6",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.085","BOOST":"4.50","PV_IN":"21.14","PWR":"6.61","MILLIS":"83440","LDSTEP":"7",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.093","BOOST":"4.50","PV_IN":"21.14","PWR":"6.60","MILLIS":"85441","LDSTEP":"8",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.101","BOOST":"4.50","PV_IN":"21.14","PWR":"6.59","MILLIS":"87442","LDSTEP":"9",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.109","BOOST":"4.50","PV_IN":"21.14","PWR":"6.58","MILLIS":"89443","LDSTEP":"10",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.116","BOOST":"4.50","PV_IN":"21.14","PWR":"6.58","MILLIS":"91444","LDSTEP":"11",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.125","BOOST":"4.50","PV_IN":"21.14","PWR":"6.57","MILLIS":"93446","LDSTEP":"12",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.132","BOOST":"4.50","PV_IN":"21.14","PWR":"6.56","MILLIS":"95447","LDSTEP":"13",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.141","BOOST":"4.50","PV_IN":"21.14","PWR":"6.56","MILLIS":"97447","LDSTEP":"14",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.50","PV_IN":"21.14","PWR":"6.56","MILLIS":"99448","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.50","PV_IN":"21.14","PWR":"6.55","MILLIS":"101449","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.55","MILLIS":"103450","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.50","PV_IN":"21.14","PWR":"6.54","MILLIS":"105451","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.54","MILLIS":"107452","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.53","MILLIS":"109453","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.000","BOOST":"4.50","PV_IN":"21.14","PWR":"6.53","MILLIS":"111454","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.53","MILLIS":"113456","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.53","MILLIS":"115457","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.53","MILLIS":"117457","LDSTEP":"15",}
/0/cctest? 8
{"PV_I":"0.049","CHRG":"0.108","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"21.04","PWR":"6.77","MILLIS":"152857","LDSTEP":"8",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.093","BOOST":"4.50","PV_IN":"21.17","PWR":"6.59","MILLIS":"162857","LDSTEP":"8",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.093","BOOST":"4.50","PV_IN":"21.14","PWR":"6.58","MILLIS":"172858","LDSTEP":"8",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.092","BOOST":"4.50","PV_IN":"21.14","PWR":"6.57","MILLIS":"182859","LDSTEP":"8",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.092","BOOST":"4.50","PV_IN":"21.14","PWR":"6.56","MILLIS":"192861","LDSTEP":"8",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.092","BOOST":"4.50","PV_IN":"21.14","PWR":"6.55","MILLIS":"202861","LDSTEP":"8",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.092","BOOST":"4.40","PV_IN":"21.14","PWR":"6.53","MILLIS":"212862","LDSTEP":"8",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.092","BOOST":"4.40","PV_IN":"21.14","PWR":"6.53","MILLIS":"222863","LDSTEP":"8",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.092","BOOST":"4.40","PV_IN":"21.14","PWR":"6.52","MILLIS":"232864","LDSTEP":"8",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.092","BOOST":"4.40","PV_IN":"21.14","PWR":"6.51","MILLIS":"242866","LDSTEP":"8",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.092","BOOST":"4.40","PV_IN":"21.14","PWR":"6.50","MILLIS":"252866","LDSTEP":"8",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.092","BOOST":"4.40","PV_IN":"21.14","PWR":"6.50","MILLIS":"262867","LDSTEP":"8",}
{"PV_I":"0.080","CHRG":"0.085","DISCHRG":"0.000","BOOST":"4.50","PV_IN":"16.63","PWR":"6.58","MILLIS":"272868","LDSTEP":"8",}
{"PV_I":"0.080","CHRG":"0.085","DISCHRG":"0.000","BOOST":"4.50","PV_IN":"16.63","PWR":"6.60","MILLIS":"282869","LDSTEP":"8",}
/0/cctest? 0,15,6400
{"PV_I":"0.020","CHRG":"0.033","DISCHRG":"0.000","BOOST":"4.68","PV_IN":"21.07","PWR":"6.78","MILLIS":"17174","LDSTEP":"0",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.039","BOOST":"4.59","PV_IN":"21.14","PWR":"6.72","MILLIS":"19174","LDSTEP":"1",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.046","BOOST":"4.59","PV_IN":"21.14","PWR":"6.70","MILLIS":"21175","LDSTEP":"2",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.053","BOOST":"4.59","PV_IN":"21.14","PWR":"6.69","MILLIS":"23176","LDSTEP":"3",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.062","BOOST":"4.59","PV_IN":"21.14","PWR":"6.67","MILLIS":"25178","LDSTEP":"4",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.070","BOOST":"4.59","PV_IN":"21.14","PWR":"6.67","MILLIS":"27179","LDSTEP":"5",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.078","BOOST":"4.59","PV_IN":"21.14","PWR":"6.66","MILLIS":"29179","LDSTEP":"6",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.085","BOOST":"4.50","PV_IN":"21.14","PWR":"6.64","MILLIS":"31180","LDSTEP":"7",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.093","BOOST":"4.50","PV_IN":"21.14","PWR":"6.64","MILLIS":"33181","LDSTEP":"8",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.102","BOOST":"4.50","PV_IN":"21.14","PWR":"6.63","MILLIS":"35182","LDSTEP":"9",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.109","BOOST":"4.50","PV_IN":"21.14","PWR":"6.62","MILLIS":"37183","LDSTEP":"10",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.118","BOOST":"4.50","PV_IN":"21.14","PWR":"6.61","MILLIS":"39184","LDSTEP":"11",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.126","BOOST":"4.50","PV_IN":"21.14","PWR":"6.61","MILLIS":"41185","LDSTEP":"12",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.134","BOOST":"4.50","PV_IN":"21.14","PWR":"6.60","MILLIS":"43187","LDSTEP":"13",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.164","BOOST":"4.50","PV_IN":"21.14","PWR":"6.58","MILLIS":"45189","LDSTEP":"14",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.149","BOOST":"4.50","PV_IN":"21.14","PWR":"6.58","MILLIS":"47190","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.149","BOOST":"4.50","PV_IN":"21.14","PWR":"6.58","MILLIS":"49190","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.149","BOOST":"4.50","PV_IN":"21.14","PWR":"6.58","MILLIS":"51191","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.149","BOOST":"4.50","PV_IN":"21.14","PWR":"6.58","MILLIS":"53192","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.149","BOOST":"4.50","PV_IN":"21.14","PWR":"6.57","MILLIS":"55193","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.149","BOOST":"4.50","PV_IN":"21.14","PWR":"6.57","MILLIS":"57194","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.149","BOOST":"4.50","PV_IN":"21.14","PWR":"6.56","MILLIS":"59195","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.149","BOOST":"4.50","PV_IN":"21.14","PWR":"6.56","MILLIS":"61196","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.149","BOOST":"4.50","PV_IN":"21.14","PWR":"6.56","MILLIS":"63198","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.149","BOOST":"4.40","PV_IN":"21.14","PWR":"6.56","MILLIS":"65200","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.149","BOOST":"4.40","PV_IN":"21.14","PWR":"6.56","MILLIS":"67201","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.149","BOOST":"4.50","PV_IN":"21.14","PWR":"6.55","MILLIS":"69201","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.149","BOOST":"4.40","PV_IN":"21.14","PWR":"6.55","MILLIS":"71202","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.54","MILLIS":"73203","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.54","MILLIS":"75204","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.54","MILLIS":"77205","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.53","MILLIS":"79206","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.53","MILLIS":"81207","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.53","MILLIS":"83208","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.53","MILLIS":"85210","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.53","MILLIS":"87211","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.52","MILLIS":"89211","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.52","MILLIS":"91212","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.52","MILLIS":"93213","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.51","MILLIS":"95214","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.51","MILLIS":"97215","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.50","MILLIS":"99216","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.50","MILLIS":"101217","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.50","MILLIS":"103219","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.50","MILLIS":"105221","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.50","MILLIS":"107222","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.50","MILLIS":"109222","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.49","MILLIS":"111223","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.49","MILLIS":"113224","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.49","MILLIS":"115225","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.47","MILLIS":"117226","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.48","MILLIS":"119227","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.48","MILLIS":"121228","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.47","MILLIS":"123230","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.47","MILLIS":"125232","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.47","MILLIS":"127233","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.47","MILLIS":"129233","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.47","MILLIS":"131234","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.47","MILLIS":"133235","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.47","MILLIS":"135236","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.47","MILLIS":"137237","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.46","MILLIS":"139238","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.46","MILLIS":"141239","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.46","MILLIS":"143240","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.45","MILLIS":"145242","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.45","MILLIS":"147243","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.45","MILLIS":"149243","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.45","MILLIS":"151244","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.31","PV_IN":"21.14","PWR":"6.45","MILLIS":"153245","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.31","PV_IN":"21.14","PWR":"6.45","MILLIS":"155246","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.31","PV_IN":"21.14","PWR":"6.45","MILLIS":"157247","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.40","PV_IN":"21.14","PWR":"6.45","MILLIS":"159248","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.31","PV_IN":"21.14","PWR":"6.45","MILLIS":"161249","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.31","PV_IN":"21.14","PWR":"6.44","MILLIS":"163251","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.31","PV_IN":"21.14","PWR":"6.44","MILLIS":"165253","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.31","PV_IN":"21.14","PWR":"6.44","MILLIS":"167254","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.31","PV_IN":"21.14","PWR":"6.44","MILLIS":"169254","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.31","PV_IN":"21.14","PWR":"6.44","MILLIS":"171255","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.43","MILLIS":"173256","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.31","PV_IN":"21.14","PWR":"6.43","MILLIS":"175257","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.43","MILLIS":"177258","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.43","MILLIS":"179259","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.31","PV_IN":"21.14","PWR":"6.43","MILLIS":"181260","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.31","PV_IN":"21.14","PWR":"6.43","MILLIS":"183262","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.31","PV_IN":"21.14","PWR":"6.42","MILLIS":"185264","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.42","MILLIS":"187265","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"0.00","PWR":"6.42","MILLIS":"189265","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.42","MILLIS":"191266","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.42","MILLIS":"193267","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.148","BOOST":"4.31","PV_IN":"21.14","PWR":"6.42","MILLIS":"195268","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.42","MILLIS":"197269","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.42","MILLIS":"199270","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.42","MILLIS":"201271","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.42","MILLIS":"203272","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.42","MILLIS":"205274","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.42","MILLIS":"207275","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.42","MILLIS":"209275","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.42","MILLIS":"211276","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.41","MILLIS":"213277","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.41","MILLIS":"215278","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.41","MILLIS":"217279","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.41","MILLIS":"219280","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.41","MILLIS":"221281","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.41","MILLIS":"223283","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.41","MILLIS":"225285","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.41","MILLIS":"227286","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.40","MILLIS":"229286","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.40","MILLIS":"231287","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.40","MILLIS":"233288","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.40","MILLIS":"235289","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.40","MILLIS":"237290","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.40","MILLIS":"239291","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.40","MILLIS":"241292","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.40","MILLIS":"243294","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.40","MILLIS":"245296","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.40","MILLIS":"247297","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.40","MILLIS":"249297","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.40","MILLIS":"251298","LDSTEP":"15",}
{"PV_I":"0.000","CHRG":"0.000","DISCHRG":"0.146","BOOST":"4.31","PV_IN":"21.14","PWR":"6.39","MILLIS":"253299","LDSTEP":"15",}
{"PV_I":"0.080","CHRG":"0.149","DISCHRG":"0.000","BOOST":"4.40","PV_IN":"16.63","PWR":"6.55","MILLIS":"263301","LDSTEP":"0",}
{"PV_I":"0.080","CHRG":"0.149","DISCHRG":"0.000","BOOST":"4.40","PV_IN":"16.63","PWR":"6.57","MILLIS":"273301","LDSTEP":"0",}
{"PV_I":"0.080","CHRG":"0.149","DISCHRG":"0.000","BOOST":"4.50","PV_IN":"16.63","PWR":"6.59","MILLIS":"283302","LDSTEP":"0",}

```