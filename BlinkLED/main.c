/* public domain */

/* MCU, F_CPU and BAUD are set in Makefile  */ 

#include <util/delay.h>
#include <avr/io.h>
//#include "../lib/pin_macros.h"
#include "../lib/pin_num.h"

int main(void)
{
	// Irrigat7 has no LED, but PB7 is wired to pin 13 on header.
    //b7_output;
    pinMode(13,OUTPUT);
    
    while (1) 
    {
        digitalWrite(13,HIGH);
        //b7_high;
        _delay_ms(4000);
        digitalWrite(13,LOW);
        //b7_low;
        _delay_ms(1000);
    }    
}

