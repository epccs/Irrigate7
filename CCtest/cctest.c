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
#define RUNTST_PRINT_DELAY_MILSEC 1000
static unsigned long serial_print_started_at;
static uint8_t runtest;

// ADC channels: 7 is battery, 6 is PV, 5 is boost, 4 is battery discharge, 3 is battery charge, 2 is PV_I.
#define START_CHANNEL 2
#define END_CHANNEL 7
static uint8_t adc_index;

static uint8_t step_index;
static uint8_t start_ld_step;
static uint8_t end_ld_step;

// Voltage at which discharge stops and charging starts is in miliVolt
#define MAX_DISCHARGE 6200
#define FIRST_DISCHARGE 6550
#define DISCHARGE_STEP 50
static uint16_t bat_discharge;

float PV_IN;
float PV_I;
float PWR;
float CHRG;
float DISCHRG;
float BOOST;

void init_load(void)
{
    // set digital lines to contorl load and turn it off
    pinMode(LD0,OUTPUT);
    digitalWrite(LD0, LOW);
    pinMode(LD1,OUTPUT);
    digitalWrite(LD1, LOW);
    pinMode(LD2,OUTPUT);
    digitalWrite(LD2, LOW);
    pinMode(LD3,OUTPUT);
    digitalWrite(LD3, LOW);
}

void init_pv(void)
{
    // set digital lines to contorl the LT3652 PV MPPT buck converter and turn it on
    // note R4 may have been removed if JTAG was used
    pinMode(SHUTDOWN,OUTPUT);
    digitalWrite(SHUTDOWN, LOW);
}

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
        if (arg_count == 0)
        {
            start_ld_step = START_LD_STEP;
            end_ld_step = END_LD_STEP;
            step_index = start_ld_step;
            bat_discharge = FIRST_DISCHARGE;
            load_step(step_index);
            runtest = 0;
            command_done = 11;
        }
        
        // arg must be a load step setting
        if (arg_count == 1)
        {
            // check that argument[0] is in the range 0..15
            if ( ( !( isdigit(arg[0][0]) ) ) || (atoi(arg[0]) < 0) || (atoi(arg[0]) > END_LD_STEP) )
            {
                printf_P(PSTR("{\"err\":\"LdStpSize %d\"}\r\n"), END_LD_STEP);
                initCommandBuffer();
                return;
            }
            
            // run at a fixed load with RUNTEST time delay.
            start_ld_step = atoi(arg[0]);
            end_ld_step = start_ld_step;
            step_index = start_ld_step;
            bat_discharge = FIRST_DISCHARGE;
            load_step(step_index);
            runtest = 1;
            
            //Shutdown the LT3652
            digitalWrite(SHUTDOWN, HIGH);
            command_done = 11;
        }
        
        // arg[0] is start_step, arg[1] is end_step, and arg[2] is the battery voltage (in mV) to switch from discharging to charging
        if (arg_count == 3)
        {
            // check that argument[0] is in the range 0..15
            if ( ( !( isdigit(arg[0][0]) ) ) || (atoi(arg[0]) < 0) || (atoi(arg[0]) > END_LD_STEP) )
            {
                printf_P(PSTR("{\"err\":\"LdStartMax %d\"}\r\n"), END_LD_STEP);
                initCommandBuffer();
                return;
            }

            // check that argument[1] is in the range 0..15
            if ( ( !( isdigit(arg[1][0]) ) ) || (atoi(arg[1]) < 0) || (atoi(arg[1]) > END_LD_STEP) )
            {
                printf_P(PSTR("{\"err\":\"LdStopMax %d\"}\r\n"), END_LD_STEP);
                initCommandBuffer();
                return;
            }

            // check that argument[2] is in the range 6000..6500
            if ( ( !( isdigit(arg[2][0]) ) ) || (atoi(arg[2]) < 6000) || (atoi(arg[2]) > 6500) )
            {
                printf_P(PSTR("{\"err\":\"LdVolt6V-6V5\"}\r\n"));
                initCommandBuffer();
                return;
            }

            // run at a fixed load with RUNTEST time delay.
            start_ld_step = (uint8_t) (atoi(arg[0]));
            end_ld_step = (uint8_t) (atoi(arg[1]));
            step_index = start_ld_step;
            bat_discharge = (uint16_t) (atoi(arg[2]));
            load_step(step_index);
            runtest = 0;
            command_done = 11;
        }
        PV_IN = analogRead(6)*(5.0/1024.0)*(532.0/100.0);
        PV_I = analogRead(2)*(5.0/1024.0)/(0.068*50.0);
        PWR = analogRead(7)*(5.0/1024.0)*(3.0/2.0);
        CHRG = analogRead(3)*(5.0/1024.0)/(0.068*50.0);
        DISCHRG = analogRead(4)*(5.0/1024.0)/(0.068*50.0);
        BOOST = analogRead(5)*(5.0/1024.0)*(105.62/5.62);
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
            printf_P(PSTR("\"%1.3f\""),PV_I);
        }

        // CCtest board has a 50V/V current sense amp to a high side 0.068 Ohm to measure battery charging.
        if (adc_index == 3) 
        {
            printf_P(PSTR("\"%1.3f\""),CHRG);
        }

        // CCtest board has a 50V/V current sense amp to a high side 0.068 Ohm to measure battery discharg.
        if (adc_index == 4) 
        {
            printf_P(PSTR("\"%1.3f\""),DISCHRG);
        }

        // Irrigate7 has a 100k and 5.62k  voltage divider on the solenoid boost converter. The BOOST goes through a 100k  to ADC5 and a 5.62k to ground.
        if (adc_index == 5) 
        {
            printf_P(PSTR("\"%1.2f\""),BOOST);
        }
        
        // Irrigate7 has a 432k and 100k voltage divider from the solar input. The PV goes through a 432k  to ADC6 and a 100k to ground.
        if (adc_index == 6) 
        {
            printf_P(PSTR("\"%1.2f\""),PV_IN);
        }

        // Irrigate7 has a 100 and 200k voltage divider from the battery(PWR). The PWR goes through a 100k  to ADC7 and a 200k to ground.
        if (adc_index == 7) 
        {
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
        printf_P(PSTR("\"%d\""),step_index);
        printf_P(PSTR("}\r\n"));
        command_done = 18;
    }

    else if ( command_done == 18 )
    {
        // step through loads to check each load setting
        PWR = analogRead(7)*(5.0/1024.0)*(3.0/2.0);
        PV_IN = analogRead(6)*(5.0/1024.0)*(532.0/100.0);
        PV_I = analogRead(2)*(5.0/1024.0)/(0.068*50.0);
        CHRG = analogRead(3)*(5.0/1024.0)/(0.068*50.0);
        DISCHRG = analogRead(4)*(5.0/1024.0)/(0.068*50.0);
        BOOST = analogRead(5)*(5.0/1024.0)*(105.62/5.62);
        if ( (step_index+1) > end_ld_step) 
        {
            // max load needs to end when battery voltage has discharged to the setpoint 
            // reading zero sometimes is caused by a problm with ISR (it will cause a skip, not a problem atm)
            if ( ( PWR < (bat_discharge/1000.0) ) && (PWR > 4.0) )
            {
                step_index = start_ld_step;
                load_step(step_index);
                
                // switch form loadtest delay to runtest delay
                runtest =1;
                
                //Enable the LT3652
                digitalWrite(SHUTDOWN, LOW);
                
                // verfiy that we have MPPT mode
                command_done = 19;
            }
            
            // durring load test use timeing delay
            if (runtest==0)
            {
                command_done = 20;
            }
        }
        else
        {
            // If LT3652 is not shutdown then the MPPT mode can be used to wait
            // to be charging in CV mode
            if ( (PV_IN > 19.0 ) && ( (arg_count == 0) || (arg_count == 3) ) )
            {
                // shutdown PV so the the full discharge load is shown for each step
                if (step_index == start_ld_step)
                {
                    //Shutdown the LT3652 (note R4 may have been removed if JTAG was used)
                    digitalWrite(SHUTDOWN, HIGH);
                }
                
                // step through the load settings the firts time, but use max load after that so I can see the charge vs discharge as fixed values
                if (runtest==0)
                {
                    step_index++;
                }
                else
                {
                    step_index = end_ld_step;
                }
                load_step(step_index);
                
                // durring run test delay until setpoint is reached
                if (runtest)
                {
                    // reduce the discharge setpoint each cycle up to the max
                    if (bat_discharge > MAX_DISCHARGE)
                    {
                        bat_discharge -= DISCHARGE_STEP;
                    }
                    command_done = 20;
                }
            }
            
            // durring load test use timeing delay
            if (runtest==0)
            {
                command_done = 20;
            }
        }
    }

    else if ( command_done == 19 ) 
    { 
        if ( (arg_count == 0) || (arg_count == 3) )
        {
            // verify LT3652 gets into MPPT mode so that charging is not skiped
            // note PV_I, CHRG, are huge when MPPT starts with my bench supply
            // also DISCHRG is zip when MPPT has got going, but I want to see the old value
            PV_IN = analogRead(6)*(5.0/1024.0)*(532.0/100.0);
            if ( (PV_IN > 16.0 ) &&  (PV_IN < 17.0 ))
            {
                command_done = 20;
            }
        }
        else
        {
            command_done = 20;
        }
    }

    else if ( command_done == 20 ) 
    { // delay between JSON printing
        unsigned long kRuntime= millis() - serial_print_started_at;
        if (runtest == 0)
        {
            if ((kRuntime) > ((unsigned long)LDTST_PRINT_DELAY_MILSEC))
            {
                command_done = 11; /* This keeps looping output forever (until a Rx char anyway) */
            }
        }
        if (runtest > 0)
        {
            if ((kRuntime) > ((unsigned long)RUNTST_PRINT_DELAY_MILSEC))
            {
                command_done = 11; /* This keeps looping output forever (until a Rx char anyway) */
            }
        }
    }

    else
    {
        printf_P(PSTR("{\"err\":\"AdcCmdDoneWTF\"}\r\n"));
        initCommandBuffer();
    }
}

