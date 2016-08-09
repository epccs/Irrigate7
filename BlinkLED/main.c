/* public domain */

#include <util/delay.h>
#include <avr/io.h>

// pin_num.h has functions like Arduino that take pin numbrs like 13 and map it to DDRB, PINB, PORTB, and bit PB7
#include "../lib/pin_num.h"

// definitions for use with the board, e.g. LED_BUILTIN is 13 (though Irrigate7 has no 
// LED on 13 it is a open pin that is not tied to hardware)
#include "../lib/pins_board.h"

int main(void)
{
    pinMode(LED_BUILTIN,OUTPUT);
    
    // The UART is not used, but I have an RPUadpt shield for 
    // uploading firmware. The RX line is blocked when low
    // so set RX0 high to turn off the RS422 driver and 
    // free the RX pair for others to use. 
    pinMode(RX0,OUTPUT);
    digitalWrite(RX0,HIGH);
    
    while (1) 
    {
        digitalWrite(LED_BUILTIN,HIGH);
        _delay_ms(4000);
        digitalToggle(LED_BUILTIN);
        _delay_ms(1000);
    }    
}

