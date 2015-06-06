/*
 * StudioBlink.c
 *
 * Created: 6/3/2015 5:59:04 PM
 *  Author: rsutherland
 * 
 */ 

#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>

int main() {
	// Set B7 as output, e.g. port B bit 7 which is on pin3 of the ATMEGA1284P-AU and is mapped to IO13 of Irrigate7
	DDRB |= (1<<PB7); // PB7 is really just 7, so shift 1 seven places to the right for 10000000

	while(1) {
		// Turn the led on by setting corresponding register bit high.
		PORTB |= (1<<PB7); // or gives 1******* where * is no change

		_delay_ms(500);

		// Turn led off by setting corresponding register bit low.
		PORTB &= ~(1<<PB7); // not gives 01111111, and gives 0******* 

		_delay_ms(500);

	}
}