/*
 * Pin definitions for Irrigate7 used with pin_num.h Digital IO library
 * Copyright (C) 2018 Ronald Sutherland
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
#define RX1 2
#define TX1 3

// Current Source Enable
#define CS0_EN 4
#define CS1_EN 5
#define CS2_EN 6
#define CS3_EN 7

// ICP1 pin reads inverted from the plugable input with 100 Ohm termination
#define ICP1 8
#define CS_ICP1_EN 9

// SPI 
#define nSS 10
#define MOSI 11
#define MISO 12
#define SCK 13

// ALTernate power ENable for a small solar PV panel
#define ALT_EN 14

//K3/K7 control
#define K3_E3 15
#define K7_NE2 15
#define K7_E3 16
#define K3_NE2 16
#define K_A2 17
#define K_A1 18
#define K_A0 19

// I2C
#define SDA 20
#define SCL 21

// VIN  pin shield power control
#define SHLD_VIN_EN 22

//K3/K7 Interface+
#define K3_NE1 23
#define K7_NE1 23

// MIX IO has an ADC channel and DIO on each pin
// There are values from 0 to 1023 for 1024 slots where each reperesents 1/1024 of the reference. Last slot has issues
// https://forum.arduino.cc/index.php?topic=303189.0   

// ADC0/DIO24 pin is available for the user
//the ADC channel can be used with analogRead(ADC0)*(<referance>/1024.0)
#define ADC0 0
#define DIO24 24

// ADC1/DIO25 pin is available for the user
// the ADC channel can be used with analogRead(ADC1)*(<referance>/1024.0)
#define ADC1 1
#define DIO25 25

// ADC2/DIO26 pin is available for the user
// the ADC channel can be used with analogRead(ADC2)*(<referance>/1024.0)
#define ADC2 2
#define DIO26 26

// ADC3/DIO27 pin is available for the user
// the ADC channel can be used with analogRead(ADC3)*(<referance>/1024.0)
#define ADC3 3
#define DIO27 27

// ADC4/DIO28 is used to select SMPS boost voltage
// the ADC channel can be used with analogRead(ADC4)*(<referance>/1024.0) to read the feedback voltage
// 9V boot set (DIO28 and DIO29 are INPUT)  analogRead(ADC4)*(<referance>/1024.0)*(115.8/15.8)
// 24V boot set (DIO28 is INPUT, DIO29 is OUTPUT LOW)  analogRead(ADC4)*(<referance>/1024.0)*(105.506/5.506)
#define ADC4 4
#define DIO28 28

// ADC5/DIO29 is used to select SMPS boost voltage
// the ADC channel can be used with analogRead(ADC5)*(5.0/1024.0) to read the feedback voltage
// 9V boot set (DIO28 and DIO29 are INPUT)  analogRead(ADC5)*(<referance>/1024.0)*(115.8/15.8)
// 12V boot set (DIO28 is OUTPUT LOW, DIO29 is INPUT)  analogRead(ADC5)*(<referance>/1024.0)*(111.57/11.57)
#define ADC4 4
#define DIO29 29

// ADC6 voltage is analogRead(PWR_I)*(<referance>/1024.0)/(0.068*50.0)
#define PWR_I 6 

// ADC7 or input voltage is analogRead(PWR_V)*(<referance>/1024.0)*(115.8/15.8)
#define PWR_V 7

// DIO30 and DIO31 should not be used 

#endif // Pins_Board_h
