# Solenoid Control 

## Future improvements (to do)

Did the flow meter not stop when the valve was closed, try to close the valve again a few times. Set a status value for the command line to read, and sort out a way to do a service request, perhaps with the bus manager.

Option to include some scaled time (or flow count) based on PV power received. In other words, keep the peak mA-Sec per hour charge rate and use it to scale the last measured hour of that value to add solenoid time (or flow count). 

## Overview

Solenoid is an interactive command line program that demonstrates control of the latching solenoid drivers. 

The solenoid state machine is implemented with the SolenoidControl() function. However before that can be used LoadSolenoidControlFromEEPROM() needs to fill the solenoid array with saved values and initialize it with StartSolenoid(), both of which are done when the day-night state machine runs the callback_for_day_attach() function.

Once the state machine has started it allows an initial delay, which is used at power up to make sure the valves are all off, they are run for a second with a delay to offset each one. Once the initial delay is done then the cycles begin which includes a runtime and a delay. Durring the runtime the flowmeater is marked in use and the flow counts are recorded for that solenoid. If the delay is not enough to allow the other valves to run then the lowest number valve has priority (due to the logic of the program) and will hog the runtime.

The implementation is very flexible, for example, there are resources constraints like when the boost converter is in use, or when the flow meter is in use. These constraints need to block other solenoids from using them even if they have passed a wait time between usage. This is critical for insuring  the flow meter values for a solenoid are in fact for that solenoid.


## Test Wiring

![Wiring](./Setup/SolenoidTestWiring.png)

This setup is used to test the solenoid drivers. 

Note that the 2200uF capacitor is not discharged after the test, use a 100 Ohm 2W resistor to discharge it before removing the connector.


# EEPROM Memory map 

A map of the solenoid settings in EEPROM. 

```
function            type    ee_addr:K1  K2 .. K7
id                  UINT16          40  60 .. 160
delay_start_sec     UINT32          42  62 .. 162
runtime_sec         UINT32          46  66 .. 166
delay_sec           UINT32          50  70 .. 170
flow_stop           UINT32          54  74 .. 174
cycles              UINT8           58  78 .. 178
```


# Start of Day 

The [day-night][../DayNight] state machine is used to load and run EEPROM values after the morning debounce. This means the valves will start to cycle after the delay_start time has elapsed each morning.


# Flow Sensor

ICP1 is available through the [Capture][../Capture] commands.


# Firmware Upload

With a serial port connection (set the BOOT_PORT in Makefile) and optiboot installed on the Irrigate7 run 'make bootload' and it should compile and then flash the MCU.

``` 
rsutherland@conversion:~/Samba/Irrigate7/Solenoid$ make bootload
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
{"id":{"name":"Solenoid","desc":"Irrigate7 Board /w atmega1284p and LT3652","avr-gcc":"4.9"}}
```


##  /0/run k[,cycles] 

Start the solenoid k (1|2|3) operation, with option to override cycles (1..255). 

If EEPROM does not have settings the solenoids retains initialized values (delay_start = 1..7, runtime= 1, delay = 3600, flow_stop = not used) that will cause each solenoid to operate for a second after a delay_start time that spaces there operation out by 3 seconds each (it insures all are in a known state).

After a solenoid has entered the delay state and let go of the flow meter resource another solenoid that is ready to use the flow meter will do so. Make sure to set the delay time long enough that all the other solenoids can use their runtime, or the flow meter becomes a resource constraint and some zones will get shorted. For example set all the delay times to 360 and make sure the combined runtimes do not add up to 360 (i.e. 100, 80, 120).

```
/1/run 1,1
{"K1":{"delay_start_sec":"1","runtime_Sec":"1","delay_Sec":"3600","cycles":"1"}}
/1/run 2,1
{"K2":{"delay_start_sec":"4","runtime_Sec":"1","delay_Sec":"3600","cycles":"1"}}
/1/run 3,1
{"K3":{"delay_start_sec":"7","runtime_Sec":"1","delay_Sec":"3600","cycles":"1"}}
```


##  /0/save k,cycles 

Save the solenoid k (1|2|3) with cycles (1..255) to EEPROM, it can then autostart.

Saved settings are loaded and operated at the start of each day.

```
/1/save 1,10
{"K1":{"delay_start_sec":"3","runtime_Sec":"10","delay_Sec":"40","cycles":"10"}}
/1/save 2,10
{"K2":{"delay_start_sec":"16","runtime_Sec":"10","delay_Sec":"40","cycles":"10"}}
/1/save 3,10
{"K3":{"delay_start_sec":"29","runtime_Sec":"10","delay_Sec":"40","cycles":"10"}}
```

##  /0/load k

Load the solenoid k (1|2|3) from EEPROM. Use run to start it.

```
/1/load 1
{"K1":{"delay_start_sec":"10","runtime_Sec":"15","delay_Sec":"60","cycles":"10"}}
/1/load 2
{"K2":{"delay_start_sec":"30","runtime_Sec":"15","delay_Sec":"60","cycles":"10"}}
/1/load 3
{"K3":{"delay_start_sec":"50","runtime_Sec":"15","delay_Sec":"60","cycles":"10"}}
```


##  /0/stop k 

Reduce the delay_start, runtime, and delay to one second each to stop the solenoid k (1|2|3) operation.

To change the solenoids setting use /stop, then /load, and change the desired setting (e.g. /runtime) and finally save it and perhaps /run it.

```
/1/stop 1
{"K1":{"stop_time_sec":"3"}}
```


##  /0/pre k,delay_start_in_sec

Set the solenoid k (1|2|3) one time delay befor cycles run (1..21600, e.g. 6hr max). 

``` 
/1/pre 1,3
{"K1":{"delay_start_sec":"3"}}
/1/pre 2,16
{"K2":{"delay_start_sec":"16"}}
/1/pre 3,29
{"K3":{"delay_start_sec":"29"}}
/1/run 2,1
{"K2":{"delay_start_sec":"16","runtime_sec":"10","delay_sec":"40","cycles":"1"}}
``` 


##  /0/runtime k,runtime_in_sec

Set the solenoid k (1|2|3) run time (1..21600, e.g. 6hr max). 

``` 
/1/runtime 1,10
{"K1":{"runtime_sec":"10"}}
/1/runtime 2,10
{"K2":{"runtime_sec":"10"}}
/1/runtime 3,10
{"K3":{"runtime_sec":"10"}}
/1/run 1,1
{"K1":{"delay_start_sec":"2","runtime_sec":"10","delay_sec":"40","cycles":"1"}}
```


##  /0/delay k,delay_in_sec

Set the solenoid k (1|2|3) delay between runs (1..86400, e.g. 24 hr max). 

```
/1/delay 1,40
{"K1":{"delay_sec":"40"}}
/1/delay 2,40
{"K2":{"delay_sec":"40"}}
/1/delay 3,40
{"K3":{"delay_sec":"40"}}
/1/run 3,1
{"K3":{"delay_start_sec":"7","runtime_sec":"1","delay_sec":"40","cycles":"1"}}
```


##  /0/fstop k,flow_stop

Set the solenoid k (1|2|3) flow_stop (1..0xFFFFFFFF) that also stops the solenoid (e.g. when flow count is reached).

``` 
/1/fstop 3,500
{"K3":{"flow_stop":"500"}}
/1/run 3,1
{"K3":{"delay_start_sec":"10","runtime_sec":"20","delay_sec":"40","cycles":"1","flow_stop":"500"}}
``` 


##  /0/flow? k

Report the solenoid k (1|2|3) flow_cnt or pulses events on ICP1.

``` 
/1/flow? 3
{"K3":{"cycle_state":"11","cycles":"7","flow_cnt":"0"}}
``` 


##  /0/time? k

Report the solenoid k (1|2|3) runtime in millis.

``` 
/1/time? 3
{"K3":{"cycle_state":"11","cycles":"9","cycle_millis":"15000"}}
``` 

## [/0/day?](../DayNight#0day)


## [/0/analog? 0..7[,0..7[,0..7[,0..7[,0..7]]]]](../Adc#0analog-0707070707)


## [/0/initICP icp1,mode,prescale](../Capture#0initicp-icp1modeprescale)


## [/0/count? [icp1]](../Capture#0count-icp1)


## [/0/capture? [icp1,1..15]](../Capture#0capture-icp1115)


## [/0/event? [icp1,1..31]](../Capture#0event-icp1131)