/*
loop_pwr is part of LoopPwr, 
Copyright (C) 2016 Ronald Sutherland

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

For a copy of the GNU General Public License use
http://www.gnu.org/licenses/gpl-2.0.html
*/
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "../lib/pin_num.h"
#include "../lib/pins_board.h"
#include "../lib/parse.h"
#include "../lib/timers.h"
#include "loop_pwr.h"

void init_sensor_loop(void)
{
    // set digital lines to select loop address zero
    DDRC = DDRC | 0b11100000;
    PORTC = PORTC & 0b00011111;
    
    // disable loop power
    pinMode(LOOP_EN,OUTPUT);
    digitalWrite(LOOP_EN, LOW);
}

// sensor Loops are addressed with PORTC on Irrigate7 [PC7:PC6:PC5]
// the address should be set all at once so the sensor gets a glitch free power up
void address_loop_sensor (uint8_t addr) 
{
  addr = addr << 5; // shift into bit 5 thru 7
  // since the high bits just got nuked there is no need to mask anything
  //Set port C bits 5 thru 7 as output
  DDRC = DDRC | 0b11100000; // note bit 4 thru 0 do not change 
  // now set the address
  PORTC = (PORTC & 0b00011111) | addr; // this will preserve bit 4 thru 0 value
}

void LoopAddr(void)
{
    if ( command_done == 10 )
    {
        // check that argument[0] is in the range 0..7
        if ( ( !( isdigit(arg[0][0]) ) ) || (atoi(arg[0]) < 0) || (atoi(arg[0]) > 7) )
        {
            printf_P(PSTR("{\"err\":\"LoopAddrMax %d\"}\r\n"), 7);
            initCommandBuffer();
            return;
        }
        command_done = 11;
    }

    if ( command_done == 11 )
    {
        // set all three address bits at the same time
        address_loop_sensor( (uint8_t)atoi(arg[0]) );
        
        // address bit 2 is PC7 or digital IO 7. Map is in pin_num.h
        printf_P( PSTR("{\"PC7\":\"%d\","), digitalRead(LOOP_ADDR2) );
        command_done = 12;
    }
    
    else if ( command_done == 12 )
    { 
        // address bit 1 is PC6 or digital IO 15. Map is in pin_num.h
        printf_P( PSTR("\"PC6\":\"%d\","), digitalRead(LOOP_ADDR1) );
        command_done = 13;
    }

    else if ( command_done == 13 )
    {
        // address bit 0 is PC5 or digital IO 16. Map is in pin_num.h
        printf_P( PSTR("\"PC5\":\"%d\""), digitalRead(LOOP_ADDR0) );
        command_done = 14;
    }

    else if ( command_done == 14 )
    {
        printf_P(PSTR("}\r\n"));
        initCommandBuffer();
    }

    else
    {
        printf_P(PSTR("{\"err\":\"CmdDoneWTF\"}\r\n"));
        initCommandBuffer();
    }
}

void LoopEnable(void)
{
    if ( command_done == 10 )
    {
        // check that argument[0] is in the range 0..1
        if ( ( !( isdigit(arg[0][0]) ) ) || (atoi(arg[0]) < 0) || (atoi(arg[0]) > 1) )
        {
            printf_P(PSTR("{\"err\":\"LoopEnableVal\"}\r\n"));
            initCommandBuffer();
            return;
        }
        command_done = 11;
    }

    else if ( command_done == 11 )
    {
        // set loop enable bit to zero or one
        digitalWrite(LOOP_EN, (uint8_t)atoi(arg[0]) );
        
        // loop enable bit is PB4 or digital IO 5. Map is in pin_num.h
        printf_P( PSTR("{\"PB4\":\"%d\"}\r\n"), digitalRead(LOOP_EN) );
        initCommandBuffer();
    }

    else
    {
        printf_P(PSTR("{\"err\":\"CmdDoneWTF\"}\r\n"));
        initCommandBuffer();
    }
}
