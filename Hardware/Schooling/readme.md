# Description

Some lessons I learned doing Irrigate7.

# Table Of Contents:

7. ^1 RPUno
6. ^1 CL8
5. ^1 K3
4. ^1 Digital IO Current Limit Resistor
3. ^0 C1 Bypass Moved to LT3652 VIN
2. ^0 LMV358 Does Not have 5V Befor Battery Connect
1. ^0 C11 Interference with HW1 (DIN mount)


## ^1 RPUno

[RPUno] went from ^2 to ^6, and the updates for RPUno are valid for the next version of Irrigate7.

[RPUno]: https://github.com/epccs/RPUno


## ^1 CL8 

The [CL8] board was done based on Irrigate7^1's current loop sensors.

[CL8]: https://github.com/epccs/Driver/tree/master/CL8

The current loop selection is going to be removed from the next version so that the ICP1 can be used as a general purpose sensor input, e.g. either a flow sensor or selectable pulse sensors (with the CL8 board) can be connected.


## ^1 K3 

The [K3] board was done based on Irrigate7^1's solenoid drivers, and it has updates.

[K3]: https://github.com/epccs/Driver/tree/master/K3


## ^1 Digital IO Current Limit Resistor 

Should go between K38 and MCU.


## ^0 C1 Bypass Moved to LT3652 VIN

Bypass does not work on the PV input like it seems to on CC01, so after some trial and error I found it was in fact needed at the LT3652 VIN pin. I thought for sure the 10uF was close enough, but it does not act like the 100nF bypass.

![Irrigate7^0 C1 Bypass Moved](./14320^0A,C1bypassMoved.jpg "Irrigate7^0 C1 Bypass Moved")

This move also got the LT3652 TIMER (C3) working better than I've seen, so I tried it on the CC01 board as well and the float time repeats now,  I think that is the general location to put it.


## ^0 LMV358 Does Not have 5V Befor Battery Connect


I checked that the charge control circuit was working befor populating the MCU and other items. But then found the battery would not connect and U4 only had .65V on its referance input while PWR node had 6.25V. After some hair pulling it turned out the LMV358 positive rail has zero voltes and prevents teh TLVH431 from sensing the 6V level to connect. First step is to remove Op-Amp*, then read up on the AVR ADC to see if resistor dividers can be made to work.

![Irrigate7^0 LMV358 Removed](./14320^0A,U7rm&jump.jpg "Irrigate7^0 LMV358 Removed")

Removed the Op-Amp. Next add the resistor dividers to measure voltages. Also add a decoupling capacitor so the sample and hold capacitor has low impedance to read the value, and I can use high resistance for the divider.

![Irrigate7^0 ADC6](./14320^0A,ADC6dividerFromPV.jpg "Irrigate7^0 ADC6")

![Irrigate7^0 ADC7](./14320^0A,ADC7dividerFromPWR.jpg "Irrigate7^0 ADC7")

So that worked, the ADC can read the value and the power loss is low (it is also SMD art).


## ^0 C11 Interference with HW1 (DIN mount)

Layout error. 

![Irrigate7^0 C11 Interference](./14320^0A,C11Interference.png "Irrigate7^0 C11 Interference")



