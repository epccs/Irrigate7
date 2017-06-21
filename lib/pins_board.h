/*
 * Pin definitions for Irrigate7 used with pin_num.h Digital IO library
 * Copyright (C) 2016 Ronald Sutherland
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with the DigitalIO Library.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef Pins_Board_h
#define Pins_Board_h

#define NUM_DIGITAL_PINS            32
#define NUM_ANALOG_INPUTS        8
// only pin 11 has pwm and can be used as a digital output so I did not list the others
#define digitalPinHasPWM(p)         ((p) == 11)

// UART on Irrigate7 is for serial communication (never use these pins)
#define RX0 0 
#define TX0 1

// Plugable Digital Input/Outputs with Level shift
#define DIO2 2
#define DIO3 3
#define DIO4 4

// Charge Controler
#define CC_SHUTDOWN 5

// Battery Control (Warning this will disconnect the battery)
#define BAT_DISCONNECT 6

// Charge Controler not Fault (note use a weak pull up to read fault state) 
#define CC_nFAULT 7

// ICP1 pin reads inverted from the plugable input with 100 Ohm termination
#define ICP1 8

// Current Source Enable
#define CURR_SOUR_EN 9

// Plugable Digital Input/Outputs with Level shift
#define DIO10 10

#define DIO11 11

// SPI pins MISO and SCK is maped with plugable DIO
#define DIO12 12
#define MISO 12

#define DIO13 13
#define SCK 13

// Irrigate7 has no led but this is the normal place it would be found
#define LED_BUILTIN 13 

// SPI pin MOSI is mapped with ICP3
#define MOSI 14
#define ICP3 14

//K3/K7/CL8 Interface
#define K3_E3 15
#define K7_NE2 15
#define CL8_NE2 15
#define K7_E3 16
#define K3_NE2 16
#define CL8_NE1 16

// JTAG on Irrigate7 is mapped with K7
#define TDI 16

//K3/K7/CL8 Interface A0, A1, A2
#define A2 17
#define TDO 17

#define A1 18
#define TMS 18

#define A0 19
#define TCK 19

// I2C on Irrigate7
#define SDA 20
#define SCL 21

// VIN  pin shield power control
#define SHLD_VIN_EN 22

//K3/K7/CL8 Interface
#define CL8_E3 23
#define K3_NE1 23
#define K7_NE1 23

// ADC channels
// There are values from 0 to 1023 for 1024 slots where each reperesents 1/1024 of the reference. Last slot has issues
// https://forum.arduino.cc/index.php?topic=303189.0   

// ADC0 has a Plugable input with a 20mA current source
// its voltage is analogRead(ADC0)*(5.0/1024.0)
#define ADC0 0

// ADC1 has a Plugable input with a 20mA current source
// its voltage is analogRead(ADC1)*(5.0/1024.0)
#define ADC1 1

// CHRG_ADC2 voltage is analogRead(CHRG_I)*(5.0/1024.0)/(0.068*50.0)
#define CHRG_I 2

// DISCHRG_ADC3 voltage is analogRead(DISCHRG_I)*(5.0/1024.0)/(0.068*50.0)
#define DISCHRG_I 3

// ADC4 has a Plugable input with a 20mA current source
// its voltage is analogRead(ADC4)*(5.0/1024.0)
#define ADC4 4

// ADC5 has a Plugable input with a 20mA current source
// its voltage is analogRead(ADC5)*(5.0/1024.0)
#define ADC5 5

// PV_IN_ADC6 voltage is analogRead(PV_V)*(5.0/1024.0)*(532.0/100.0)
#define PV_V 6 

// PWR_ADC7 or Battery voltage is analogRead(PWR_V)*(5.0/1024.0)*(3.0/2.0)
#define PWR_V 7

#endif // Pins_Board_h
