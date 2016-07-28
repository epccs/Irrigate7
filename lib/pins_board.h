/*
 * Pin definitions for use with DigitalIO library
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
// only pin 11 has pwm and can be used as a digital output so I do not list the others
#define digitalPinHasPWM(p)         ((p) == 11 )

// JTAG lines
#define TDI 16
#define TDO 17
#define TMS 18
#define TCK 19

// LT3652 has some pins that may be connected to mega1284p (JTAG can not be used if these are connected)
#define SHUTDOWN TCK
#define DISCONNECT TDO
#define FAULT TMS

//#define LED_BUILTIN = 13; // Irrigate7's LED has to do with Boost operaiton, it is not a on/off control

// SPI is not free to use on Irrigate7 (e.g. MOSI is  ICP3, which is used for flow meter)
#define SS 5
#define MOSI 14
#define MISO 12
#define SCK  13

#define SDA 20
#define SCL 21

// these are ADC channels, they do not have digital IO function on ATmega328p
// Irrigate7 has a 100k and 5.62k  voltage divider on the solenoid boost converter.
#define BOOST_ADC5 5
// Irrigate7 has a 432k and 100k voltage divider from the solar input.
#define PV_IN_ADC6 6
// Irrigate7 has a 100 and 200k voltage divider from the battery(PWR)
#define PWR_ADC7 7

#endif // Pins_Board_h
