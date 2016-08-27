/* public domain */

#include <util/delay.h>
#include <avr/io.h>
#include "../lib/timers.h"
#include "../lib/pin_num.h"
#include "../lib/pins_board.h"

static unsigned long blink_started_at;

int main(void)
{
    pinMode(LED_BUILTIN,OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);  

    // The UART is not used hear, but the RPUadpt shield I use for 
    // uploading firmware needs RX line set high to turn off the 
    // RS-422 driver. 
    pinMode(RX0,OUTPUT);
    digitalWrite(RX0,HIGH);

    initTimers(); //Timer0 Fast PWM mode, Timer1 & Timer2 Phase Correct PWM mode.

    sei(); // Enable global interrupts to start TIMER0

    blink_started_at = millis();

    while (1) 
    {
        unsigned long kRuntime = 0;
        
        kRuntime = millis() - blink_started_at;
        
        if (kRuntime > 1000) 
        {
            digitalToggle(LED_BUILTIN);
            
            // next toggle 
            blink_started_at += 1000; 
        }
    } 
}

