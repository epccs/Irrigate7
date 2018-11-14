/* Irrigate7 (Atmega1284p) DigitalIO Library
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
 * along with the Arduino DigitalIO Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 * 
 */
#ifndef PinNum_h
#define PinNum_h

// avr-libc
#include <avr/io.h>
#include <util/atomic.h>

// avr-gcc
#include <stdbool.h>

#define INPUT 0
#define OUTPUT 1

#define LOW 0
#define HIGH 1

typedef struct {
  volatile uint8_t* ddr; 
  volatile uint8_t* pin; 
  volatile uint8_t* port;
  uint8_t bit;  
} Pin_Map;

#if defined(__AVR_ATmega164P__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)

#define NUM_DIGITAL_PINS 32

/* Each of the AVR Digital I/O ports is associated with three I/O registers. 
8 bit Data Direction Register (DDRx) each bit sets a pin as input (=0) or output (=1).
8 bit Port Input Register (PINx) each  bit is the input from a pin that was latched durring last low edge of the system clock.
8 bit Port Data Register (PORTx) each bit drives a pin if set as output (or sets pullup if input)
Where x is the port A, B, C, etc.

Wiring (e.g. Arduino) uses pin numbers to control their functions. 
This is wasteful, but teaching "DDRB |= _BV(PB5)" to set an LED pin as an output is not going to do.
Make no mistake this is for the kids, but is also is for fun :-) */
static const Pin_Map pinMap[NUM_DIGITAL_PINS] = {
    [0] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD0 }, // RX0
    [1] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD1 }, // TX0
    [2] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD2 }, // RX1
    [3] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD3 }, // TX1
    [4] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB0 }, // CS0_EN
    [5] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD4 }, // CS1_EN
    [6] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB3 }, // CS2_EN
    [7] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC7 }, // CS3_EN
    [8] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD6 }, // ICP1
    [9] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD5 }, // CS_ICP1_EN
    [10] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB4 }, // nSS
    [11] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB5 }, // MOSI
    [12] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB6 }, // MISO
    [13] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB7 }, // SCK
    [14] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD7 }, // ALT_EN
    [15] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC6 }, // K3_E3, K7_NE2
    [16] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC5 }, // K3_NE2, K7_E3
    [17] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC4 }, // K_A2
    [18] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC3 }, // K_A1
    [19] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC2 }, // K_A0
    [20] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC1 }, // SDA
    [21] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC0 }, // SCL
    [22] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB2 }, // SHLD_VIN_EN
    [23] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB1 }, // K3_NE1, K7_NE1
    [24] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA0 }, // DIO24
    [25] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA1 }, // DIO25
    [26] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA2 }, // DIO26
    [27] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA3 }, // DIO27
    [28] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA4 }, // DIO28
    [29] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA5 }, // DIO29
    [30] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA6 }, // DIO30 
    [31] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA7 } // DIO31
};
#endif 

// note: the use of dead code elimination is not standard C. 
static inline __attribute__((always_inline)) uint8_t badPin(uint8_t pin) 
{
    if (pin >= NUM_DIGITAL_PINS) 
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static inline __attribute__((always_inline))
void bitWrite(volatile uint8_t* register_addr, uint8_t bit_offset, bool value_for_bit) 
{
    // Although I/O Registers 0x20 and 0x5F, e.g. PORTn and DDRn should not need 
    // atomic change control it does not harm.
    ATOMIC_BLOCK ( ATOMIC_RESTORESTATE )
    {
        if (value_for_bit) 
        {
            *register_addr |= 1 << bit_offset;
        } 
        else 
        {
            *register_addr &= ~(1 << bit_offset);
        }
    }
}

/* read value from pin number */
static inline __attribute__((always_inline))
bool digitalRead(uint8_t pin_num) 
{
    if (!badPin(pin_num)) 
    {
        return (*pinMap[pin_num].pin >> pinMap[pin_num].bit) & 1;
    }
    else
    {
        return 0;
    }
}

/* set pin value HIGH and LOW */
static inline __attribute__((always_inline))
void digitalWrite(uint8_t pin_num, bool value_for_bit) {
    if (!badPin(pin_num)) bitWrite(pinMap[pin_num].port, pinMap[pin_num].bit, value_for_bit);
}

/* toggle pin number  */
static inline __attribute__((always_inline))
void digitalToggle(uint8_t pin_num) {
    if (!badPin(pin_num)) 
    {
        // Ckeck if pin is in OUTPUT mode befor changing it
        if( ( ( (*pinMap[pin_num].ddr) >> pinMap[pin_num].bit ) & 1) == OUTPUT )  
        {
            digitalWrite(pin_num, !digitalRead(pin_num));
        }
    }
}

/* set pin mode INPUT and OUTPUT */
static inline __attribute__((always_inline))
void pinMode(uint8_t pin_num, bool output_mode) {
    if (!badPin(pin_num)) bitWrite(pinMap[pin_num].ddr, pinMap[pin_num].bit, output_mode);
}

#endif  // DigitalPin_h

