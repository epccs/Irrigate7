/*
cctest is part of CCtest, it is a test for Irrigate7's battery charge control and PV power, 
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
#include "../lib/adc.h"
#include "../lib/timers.h"
#include "cctest.h"

#define LDTST_PRINT_DELAY_MILSEC 2000
#define RUNTST_PRINT_DELAY_MILSEC 60000
static unsigned long serial_print_started_at;

// ADC channels: 7 is battery, 6 is PV, 5 is boost, 4 is battery discharge, 3 is battery charge, 2 is PV_I.
#define START_CHANNEL 2
#define END_CHANNEL 7
static uint8_t adc_index;

// Load Control bits: LD0 is digital 10, LD1 is digital 11, LD2 is digital 12, LD3 is digital 13, 
#define LD0 10
#define LD1 11
#define LD2 12
#define LD3 13
#define START_LD_STEP 0
#define END_LD_STEP 15
static uint8_t step_index;

float PWR;
float PV_IN;

void load_step(uint8_t step)
{
    // set LD0 bit
    if(step & (1<<0) )
    {
        digitalWrite(LD0, HIGH);
    }
    else
    {
        digitalWrite(LD0, LOW);
    }
    
    // set LD1 bit
    if(step & (1<<1) )
    {
        digitalWrite(LD1, HIGH);
    }
    else
    {
        digitalWrite(LD1, LOW);
    }
    
    // set LD2 bit
    if(step & (1<<2) )
    {
        digitalWrite(LD2, HIGH);
    }
    else
    {
        digitalWrite(LD2, LOW);
    }
    
    // set LD3 bit
    if(step & (1<<3) )
    {
        digitalWrite(LD3, HIGH);
    }
    else
    {
        digitalWrite(LD3, LOW);
    }
}


void CCtest(void)
{
    if ( command_done == 10 )
    {
        step_index = START_LD_STEP;
        load_step(step_index);
        command_done = 11;
    }

    if ( command_done == 11 )
    {
        serial_print_started_at = millis();
        printf_P(PSTR("{"));
        adc_index= START_CHANNEL;
        command_done = 12;
    }
    
    else if ( command_done == 12 )
    { // use the channel as an index in the JSON reply
        if (adc_index < 2)
        {
            printf_P(PSTR("\"ADC%d\":"),adc_index);
        }

        if (adc_index == 2)
        {
            printf_P(PSTR("\"PV_I\":"));
        }

        if (adc_index == 3)
        {
            printf_P(PSTR("\"CHRG\":"));
        }

        if (adc_index == 4)
        {
            printf_P(PSTR("\"DISCHRG\":"));
        }

        if (adc_index == 5)
        {
            printf_P(PSTR("\"BOOST\":"));
        }

        if (adc_index == 6)
        {
            printf_P(PSTR("\"PV_IN\":"));
        }

        if (adc_index == 7)
        {
            printf_P(PSTR("\"PWR\":"));
        }
        command_done = 13;
    }

    else if ( command_done == 13 )
    {
        // There are values from 0 to 1023 for 1024 slots where each reperesents 1/1024 of the reference. Last slot has issues
        // https://forum.arduino.cc/index.php?topic=303189.0        
        if (adc_index < 2)
        {
            printf_P(PSTR("\"%1.2f\""),(analogRead(adc_index)*5.0/1024.0));
        }

        // CCtest board has a 50V/V current sense amp to a high side 0.068 Ohm to measure PV current.
        if (adc_index == 2) 
        {
            printf_P(PSTR("\"%1.3f\""),(analogRead(adc_index)*(5.0/1024.0)/(0.068*50.0)));
        }

        // CCtest board has a 50V/V current sense amp to a high side 0.068 Ohm to measure battery charging.
        if (adc_index == 3) 
        {
            printf_P(PSTR("\"%1.3f\""),(analogRead(adc_index)*(5.0/1024.0)/(0.068*50.0)));
        }

        // CCtest board has a 50V/V current sense amp to a high side 0.068 Ohm to measure battery discharg.
        if (adc_index == 4) 
        {
            printf_P(PSTR("\"%1.3f\""),(analogRead(adc_index)*(5.0/1024.0)/(0.068*50.0)));
        }

        // Irrigate7 has a 100k and 5.62k  voltage divider on the solenoid boost converter. The BOOST goes through a 100k  to ADC5 and a 5.62k to ground.
        if (adc_index == 5) 
        {
            printf_P(PSTR("\"%1.2f\""),(analogRead(adc_index)*(5.0/1024.0)*(105.62/5.62)));
        }
        
        // Irrigate7 has a 432k and 100k voltage divider from the solar input. The PV goes through a 432k  to ADC6 and a 100k to ground.
        if (adc_index == 6) 
        {
            PV_IN = analogRead(adc_index)*(5.0/1024.0)*(532.0/100.0);
            printf_P(PSTR("\"%1.2f\""),PV_IN);
        }

        // Irrigate7 has a 100 and 200k voltage divider from the battery(PWR). The PWR goes through a 100k  to ADC7 and a 200k to ground.
        if (adc_index == 7) 
        {
            PWR = analogRead(7)*(5.0/1024.0)*(3.0/2.0);
            printf_P(PSTR("\"%1.2f\""),PWR);
        }

        if ( (adc_index+1) > END_CHANNEL) 
        {
            printf_P(PSTR(","));
            command_done = 14;
        }
        else
        {
            printf_P(PSTR(","));
            adc_index++;
            command_done = 12;
        }
    }

    else if ( command_done == 14 )
    {
        printf_P(PSTR("\"MILLIS\":"));
        command_done = 15;
    }

    else if ( command_done == 15 )
    {
        printf_P(PSTR("\"%lu\","),millis());
        command_done = 16;
    }

    else if ( command_done == 16 )
    {
        printf_P(PSTR("\"LDSTEP\":"));
        command_done = 17;
    }

    else if ( command_done == 17 )
    {
        printf_P(PSTR("\"%d\","),step_index);
        command_done = 18;
    }

    else if ( command_done == 18 )
    {
        printf_P(PSTR("}\r\n"));
        if ( (step_index+1) > END_LD_STEP) 
        {
            //Shutdown the LT3652 (note R4 may have been removed if JTAG was used)
            pinMode(SHUTDOWN,OUTPUT);
            digitalWrite(SHUTDOWN, HIGH);
            
            // reading zero sometimes is caused by problm with ISR (it will cause a skip)
            if ( (PWR < 6.5) && (PWR > 4.0) )
            {
                step_index = START_LD_STEP;
                load_step(step_index);
                digitalWrite(SHUTDOWN, LOW);
            }
            
            command_done = 19;
        }
        else
        {
            // reading zero sometimes is caused by problm with ISR (it will cause a skip)
            // wait to be in CV mode which means the battery is at the float voltage
            if ( PV_IN > 19.0 )
            {
                step_index++;
                load_step(step_index);
                digitalWrite(SHUTDOWN, LOW);
            }
            
            command_done = 19;
        }
    }

    else if ( command_done == 19 ) 
    { // delay between JSON printing
        unsigned long kRuntime= millis() - serial_print_started_at;
        if ((kRuntime) > ((unsigned long)LDTST_PRINT_DELAY_MILSEC))
        {
            command_done = 11; /* This keeps looping output forever (until a Rx char anyway) */
        }
    }

    else
    {
        printf_P(PSTR("{\"err\":\"AdcCmdDoneWTF\"}\r\n"));
        initCommandBuffer();
    }
}

