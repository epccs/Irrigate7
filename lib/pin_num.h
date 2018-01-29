/* ATmega1284 DigitalIO Library
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
    [24] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA0 }, // [ADC0]
    [25] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA1 }, // [ADC1]
    [26] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA2 }, // [ADC2]
    [27] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA3 }, // [ADC3]
    [28] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA4 }, // [ADC4]
    [29] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA5 }, // [ADC5]
    [30] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA6 }, // [PWR_I]
    [31] = { .ddr=&DDRA, .pin=&PINA, .port=&PORTA, .bit= PA7 }, // [PWR_V]
    [4] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB0 }, // [IO4]
    [23] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB1 }, // T1 [IO23]
    [22] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB2 }, // INT2 [IO22]
    [6] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB3 }, // OC0A [IO6]
    [5] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB4 }, // OC0B/nSS [IO5]
    [14] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB5 }, // ICP3/MOSI [IO14]
    [12] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB6 }, // OC3A/MISO [IO12]
    [13] = { .ddr=&DDRB, .pin=&PINB, .port=&PORTB, .bit= PB7 }, // OC3B/SCK [IO13]
    [21] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC0 }, // SCL [IO21]
    [20] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC1 }, // SDA [IO20]
    [19] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC2 }, // TCK [IO19]
    [18] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC3 }, // TMS [IO18]
    [17] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC4 }, // TDO [IO17]
    [16] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC5 }, // TDI [IO16]
    [15] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC6 }, // [IO15]
    [7] = { .ddr=&DDRC, .pin=&PINC, .port=&PORTC, .bit= PC7 }, // [IO7]
    [0] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD0 }, // RX0 [IO0]
    [1] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD1 }, // TX0 [IO1]
    [2] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD2 }, // RX1/INT0 [IO2]
    [3] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD3 }, // TX0/INT1 [IO3]
    [10] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD4 }, // OC1B [IO10]
    [9] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD5 }, // OC1A [IO9]
    [8] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD6 }, // ICP1 [IO8]
    [11] = { .ddr=&DDRD, .pin=&PIND, .port=&PORTD, .bit= PD7 } // OC2A [IO11]
};
#else
    #error Pin Map for MCU not available
#endif 

void badPinNumber(void) __attribute__((error("Pin number is too large or not a constant")));

static inline __attribute__((always_inline)) void badPinCheck(uint8_t pin) 
{
    if (pin >= NUM_DIGITAL_PINS) badPinNumber();
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
    badPinCheck(pin_num);
    return (*pinMap[pin_num].pin >> pinMap[pin_num].bit) & 1;
}

/* set pin value LOW and HIGH */
static inline __attribute__((always_inline))
void digitalWrite(uint8_t pin_num, bool value_for_bit) {
  badPinCheck(pin_num);
  bitWrite(pinMap[pin_num].port, pinMap[pin_num].bit, value_for_bit);
}

/* toggle pin*/
static inline __attribute__((always_inline))
void digitalToggle(uint8_t pin) {
    badPinCheck(pin);
    // Ckeck if pin is in OUTPUT mode befor changing it
    if( ( ( (*pinMap[pin].ddr) >> pinMap[pin].bit ) & 1) == OUTPUT )  
    {
        digitalWrite(pin, !digitalRead(pin));
    }
}

/* set pin mode INPUT and OUTPUT */
static inline __attribute__((always_inline))
void pinMode(uint8_t pin_num, bool output_mode) {
  badPinCheck(pin_num);
  bitWrite(pinMap[pin_num].ddr, pinMap[pin_num].bit, output_mode);
}

#endif  // DigitalPin_h

