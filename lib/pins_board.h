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

// JTAG on Irrigate7 (make sure to lift the resistors that connect to the LT3652 if you use JTAG)
#define TDI 16
#define TDO 17
#define TMS 18
#define TCK 19

// LT3652 has some pins that may be connected to mega1284p (JTAG can not be used if these are connected)
#define SHUTDOWN TCK
#define DISCONNECT TDO
#define FAULT TMS

// Irrigate7 board has no led but this is the normal place it would be found
#define LED_BUILTIN 13 

// UART for serial communication (do not use these doing so will conflict with uart.c and uart.h)
#define RX0 0 
#define TX0 1

// SPI is not open on Irrigate7 (e.g. MOSI is ICP3 which is used for the flow meter)
#define nSS 5
#define MOSI 14
#define MISO 12
#define SCK  13

// I2C on Irrigate7
#define SDA 20
#define SCL 21

// Sensor Loop
#define LOOP_EN 5
#define LOOP_ADDR2 7
#define LOOP_ADDR1 15
#define LOOP_ADDR0 16

// ADC channels
// There are values from 0 to 1023 for 1024 slots where each reperesents 1/1024 of the reference. Last slot has issues
// https://forum.arduino.cc/index.php?topic=303189.0

// PV_I_ADC1 is a high side current sense on CCtest board
// PV_I_ADC1 voltage is analogRead(PV_I)*(5.0/1024.0)/(0.068*50.0)
#define PV_I 2

// CHRG_ADC2 voltage is analogRead(CHRG_I)*(5.0/1024.0)/(0.068*50.0)
#define CHRG_I 3

// DISCHRG_ADC3 voltage is analogRead(DISCHRG_I)*(5.0/1024.0)/(0.068*50.0)
#define DISCHRG_I 4

// BOOST voltage from solenoid boost converter is analogRead(BOOST_V)*(5.0/1024.0)*(105.62/5.62).
#define BOOST_V 5
// PV_IN voltage is analogRead(PV_IN_ADC6)*(5.0/1024.0)*(532.0/100.0)
#define PV_V 6
// PWR or Battery voltage is analogRead(PWR_ADC7)*(5.0/1024.0)*(3.0/2.0)
#define PWR_V 7

#endif // Pins_Board_h
