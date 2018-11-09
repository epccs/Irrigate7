/*
solenoid is part of Solenoid, it is a serial command interface to some solenoid control functions, 
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
#include <avr/eeprom.h> 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "../lib/parse.h"
#include "../lib/timers.h"
#include "../lib/icp1.h"
#include "../lib/pin_num.h"
#include "../lib/pins_board.h"
#include "solenoid.h"

// K7 Digital pins on Irrigate7 are defined in ../lib/pins_board.h
// A0 is 19, A1 is 18, A2 is 17
// K1..K3 are selected with K3_E3, K3_NE2, K2_NE1
// K4..K7 are selected with K7_E3, K7_NE2, K7_NE1
// Digital 13 is used to blink I2C status in main()
// Digital 4 is used to blink Day-Night status in main()

//The EEPROM memory usage is as follows. 
#define EE_SOLENOID_BASE_ADDR 40
// each solenoid K1..K3 has an array of settings offset by
#define EE_SOLENOID_ARRAY_OFFSET 20
// each setting is at this byte from the array offset
#define EE_SOLENOID_ID 0
#define EE_SOLENOID_DLY_STRT 2
#define EE_SOLENOID_RUNTIME 6
#define EE_SOLENOID_DELAY 10
#define EE_SOLENOID_FLW_STP 14
#define EE_SOLENOID_CYCLES 18

// index zero (e.g. K0) is not a solenoid (e.g. 74HC238 outputs Y0 is not used and Y1 is boost)
#define BOOST 0

#define BOOST_TIME 650
#define PWR_HBRIDGE 50
#define SOLENOID_CLOSE 1000

#define SEC_IN_HR 3600UL
#define SEC_IN_6HR 21600UL
#define SEC_IN_DAY 86400UL

#define FLOW_NOT_SET 0xFFFFFFFFUL

typedef struct {
    unsigned long started_at;  // holds the start time of the present operation
    unsigned long delay_start_sec; // delay befor first set operation (one time)
    unsigned long runtime_sec; // delay after set operation and befor reset
    unsigned long delay_sec;   // delay after reset operation and befor next set operation if (cycles > 0)
    uint8_t cycle_state;
    uint8_t cycles; // keep cycling until zero
    uint32_t flow_stop; // pulse counts after start at which to reset the solenoid
    uint32_t flow_cnt_start; // pulse count when set occured (e.g. ICP1 capture events)
    uint32_t flow_cnt_stop; // pulse count when reset occured
    uint32_t flow_cnt_bank; // pulse count accumulate or store
    // uint8_t use_flow_meter; // the defaut is for all solenoids to use the flow meter, but what if I want to inject fertilizers into flow
    unsigned long cycle_millis_start;
    unsigned long cycle_millis_stop;
    unsigned long cycle_millis_bank; // millis count accumulate or store
}  solenoidTimer;

static solenoidTimer k[SOLENOID_COUNT];

static uint8_t boostInUse; // 0 if free, 1 thru SOLENOID_COUNT and is the solenoid using boost
static uint8_t flowInUse; // 0 only one solenoid can be active so its flow count can be measured

uint8_t k_solenoid_from_arg0 (void)
{
    // check that arg[0] is a digit 
    if ( ( !( isdigit(arg[0][0]) ) ) )
    {
        printf_P(PSTR("{\"err\":\"%sKNaN\"}\r\n"),command[1]);
        return 0;
    }
    uint8_t k_solenoid = atoi(arg[0]);
    if ( ( k_solenoid < 1) || (k_solenoid > SOLENOID_COUNT) )
    {
        printf_P(PSTR("{\"err\":\"%sKOtOfRng\"}\r\n"),command[1]);
        return 0;
    }
    return k_solenoid;
}

unsigned long ul_from_arg1 (unsigned long max)
{
    // check that arg[1] is a digit 
    if ( ( !( isdigit(arg[1][0]) ) ) )
    {
        printf_P(PSTR("{\"err\":\"%s1NaN\"}\r\n"),command[1]);
        return 0;
    }
    unsigned long ul = strtoul(arg[1], (char **)NULL, 10);
    if ( ( ul < 1) || (ul > max) )
    {
        printf_P(PSTR("{\"err\":\"%s1OtOfRng\"}\r\n"),command[1]);
        return 0;
    }
    return ul;
}

// arg[0] is solenoid, arg[1] is delay_start
void DelayStart(void)
{
    if ( (command_done == 10) )
    {
        uint8_t k_solenoid = k_solenoid_from_arg0();
        if (! k_solenoid)
        {
            initCommandBuffer();
            return;
        }
        // and arg[1] value is 1..SEC_IN_6HR 
        unsigned long delay_start = ul_from_arg1(SEC_IN_6HR);
        if (! delay_start)
        {
            initCommandBuffer();
            return;
        }
        // don't change a solenoid that is in use it needs to be stopped first
        if (k[k_solenoid-1].cycle_state)
        {
            printf_P(PSTR("{\"err\":\"K%dInUse\"}\r\n"),k_solenoid);
            initCommandBuffer();
            return;
        }
        k[k_solenoid-1].delay_start_sec = delay_start;
        printf_P(PSTR("{\"K%d\":{"),k_solenoid);
        command_done = 11;
    }
    else if ( (command_done == 11) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"delay_start_sec\":\"%lu\""),(k[k_solenoid-1].delay_start_sec));
        command_done = 12;
    }
    else if ( (command_done == 12) )
    {
        printf_P(PSTR("}}\r\n"));
        initCommandBuffer();
    }
    else
    {
        printf_P(PSTR("{\"err\":\"DlyStCmdDnWTF\"}\r\n"));
        initCommandBuffer();
    }
}

// arg[0] is solenoid, arg[1] is runtime
void RunTime(void)
{
    if ( (command_done == 10) )
    {
        uint8_t k_solenoid = k_solenoid_from_arg0();
        if (! k_solenoid)
        {
            initCommandBuffer();
            return;
        }
        // and arg[1] value is 1..SEC_IN_6HR 
        unsigned long runtime = ul_from_arg1(SEC_IN_6HR);
        if (! runtime)
        {
            initCommandBuffer();
            return;
        }
        // don't change a solenoid that is in use it needs to be stopped first
        if (k[k_solenoid-1].cycle_state)
        {
            printf_P(PSTR("{\"err\":\"K%dInUse\"}\r\n"),k_solenoid);
            initCommandBuffer();
            return;
        }
        k[k_solenoid-1].runtime_sec = runtime;
        printf_P(PSTR("{\"K%d\":{"),k_solenoid);
        command_done = 11;
    }
    else if ( (command_done == 11) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"runtime_sec\":\"%lu\","),(k[k_solenoid-1].runtime_sec));
        command_done = 12;
    }
    else if ( (command_done == 12) )
    {
        printf_P(PSTR("}}\r\n"));
        initCommandBuffer();
    }
    else
    {
        printf_P(PSTR("{\"err\":\"RunTmCmdDnWTF\"}\r\n"));
        initCommandBuffer();
    }
}

// arg[0] is solenoid, arg[1] is delay
void Delay(void)
{
    if ( (command_done == 10) )
    {
        uint8_t k_solenoid = k_solenoid_from_arg0();
        if (! k_solenoid)
        {
            initCommandBuffer();
            return;
        }
        // and arg[1] value is 1..SEC_IN_DAY 
        unsigned long delay = ul_from_arg1(SEC_IN_DAY);
        if (! delay)
        {
            initCommandBuffer();
            return;
        }
        // don't change a solenoid that is in use it needs to be stopped first
        if (k[k_solenoid-1].cycle_state)
        {
            printf_P(PSTR("{\"err\":\"K%dInUse\"}\r\n"),k_solenoid);
            initCommandBuffer();
            return;
        }
        k[k_solenoid-1].delay_sec = delay;
        printf_P(PSTR("{\"K%d\":{"),k_solenoid);
        command_done = 11;
    }
    else if ( (command_done == 11) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"delay_sec\":\"%lu\""),(k[k_solenoid-1].delay_sec));
        command_done = 12;
    }
    else if ( (command_done == 12) )
    {
        printf_P(PSTR("}}\r\n"));
        initCommandBuffer();
    }
    else
    {
        printf_P(PSTR("{\"err\":\"DelayCmdDnWTF\"}\r\n"));
        initCommandBuffer();
    }
}

// arg[0] is solenoid, arg[1] is flow_stop
void FlowStop(void)
{
    if ( (command_done == 10) )
    {
        uint8_t k_solenoid = k_solenoid_from_arg0();
        if (! k_solenoid)
        {
            initCommandBuffer();
            return;
        }
        // and arg[1] value is 1..FLOW_NOT_SET 
        unsigned long flow_stop = ul_from_arg1(FLOW_NOT_SET);
        if (! flow_stop)
        {
            initCommandBuffer();
            return;
        }
        // don't change a solenoid that is in use it needs to be stopped first
        if (k[k_solenoid-1].cycle_state)
        {
            printf_P(PSTR("{\"err\":\"K%dInUse\"}\r\n"),k_solenoid);
            initCommandBuffer();
            return;
        }
        k[k_solenoid-1].flow_stop = flow_stop;
        printf_P(PSTR("{\"K%d\":{"),k_solenoid);
        command_done = 11;
    }
    else if ( (command_done == 11) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"flow_stop\":\"%lu\","),(k[k_solenoid-1].flow_stop));
        command_done = 12;
    }
    else if ( (command_done == 12) )
    {
        printf_P(PSTR("}}\r\n"));
        initCommandBuffer();
    }
    else
    {
        printf_P(PSTR("{\"err\":\"FlwStpCmdDnWTF\"}\r\n"));
        initCommandBuffer();
    }
}

// arg[0] is solenoid, [arg[1] is cycles]
void Run(void)
{
    if ( (command_done == 10) )
    {
        uint8_t k_solenoid = k_solenoid_from_arg0();
        if (! k_solenoid)
        {
            initCommandBuffer();
            return;
        }
        
        uint8_t cycles = k[k_solenoid-1].cycles;
        if (arg[1]!=NULL)
        {
            // and arg[1] value is 1..0xFF 
            cycles = (uint8_t) (ul_from_arg1(0xFF));
            if (! cycles)
            {
                initCommandBuffer();
                return;
            }
        }

         // don't run a solenoid that is in use it needs to be stopped first
        if (k[k_solenoid-1].cycle_state)
        {
            printf_P(PSTR("{\"err\":\"K%dInUse\"}\r\n"),k_solenoid);
            initCommandBuffer();
            return;
        }
        k[k_solenoid-1].cycle_state = 1;
        k[k_solenoid-1].cycles = cycles;
        k[k_solenoid-1].flow_cnt_bank = 0;
        k[k_solenoid-1].cycle_millis_bank = 0;
        k[k_solenoid-1].started_at = millis(); //delay_start_sec is timed from now
        printf_P(PSTR("{\"K%d\":{"),k_solenoid);
        command_done = 11;
    }
    else if ( (command_done == 11) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"delay_start_sec\":\"%lu\","),(k[k_solenoid-1].delay_start_sec));
        command_done = 12;
    }
    else if ( (command_done == 12) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"runtime_sec\":\"%lu\","),(k[k_solenoid-1].runtime_sec));
        command_done = 13;
    }
    else if ( (command_done == 13) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"delay_sec\":\"%lu\","),(k[k_solenoid-1].delay_sec));
        command_done = 14;
    }
    else if ( (command_done == 14) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"cycles\":\"%d\""),(k[k_solenoid-1].cycles));
        command_done = 15;
    }
    else if ( (command_done == 15) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        if (k[k_solenoid-1].flow_stop != FLOW_NOT_SET)
        {
            printf_P(PSTR(",\"flow_stop\":\"%lu\""),(k[k_solenoid-1].flow_stop));
        }
        command_done = 16;
    }
    else if ( (command_done == 16) )
    {
        printf_P(PSTR("}}\r\n"));
        initCommandBuffer();
    }
    else
    {
        printf_P(PSTR("{\"err\":\"RunCmdDnWTF\"}\r\n"));
        initCommandBuffer();
    }
}

// arg[0] is solenoid, arg[1] is cycles
void Save(void)
{
    if ( (command_done == 10) )
    {
        uint8_t k_solenoid = k_solenoid_from_arg0();
        if (! k_solenoid)
        {
            initCommandBuffer();
            return;
        }
        // and arg[1] value is 1..0xFF 
        unsigned long cycles = ul_from_arg1(0xFF);
        if (! cycles)
        {
            initCommandBuffer();
            return;
        }
         // don't save a solenoid that is in use
        if (k[k_solenoid-1].cycle_state)
        {
            printf_P(PSTR("{\"err\":\"K%dInUse\"}\r\n"),k_solenoid);
            initCommandBuffer();
            return;
        }
        if ( eeprom_is_ready() )
        {
            k[k_solenoid-1].cycle_state = 0;
            k[k_solenoid-1].cycles = (uint8_t)cycles;
            k[k_solenoid-1].flow_cnt_bank = 0;
            k[k_solenoid-1].cycle_millis_bank = 0;
            uint16_t value = ((uint16_t) (k_solenoid)) + 0x4B30; //ascii bytes for 'K1', 'K2'...
            eeprom_write_word( (uint16_t *)((k_solenoid-1)*EE_SOLENOID_ARRAY_OFFSET+EE_SOLENOID_BASE_ADDR+EE_SOLENOID_ID), value);
            printf_P(PSTR("{\"K%d\":{"),k_solenoid);
            command_done = 11;
        }
    }
    else if ( (command_done == 11) )
    {  
        if ( eeprom_is_ready() )
        {
            uint8_t k_solenoid = atoi(arg[0]);
            uint32_t value = k[k_solenoid-1].delay_start_sec;
            eeprom_write_dword( (uint32_t *)((k_solenoid-1)*EE_SOLENOID_ARRAY_OFFSET+EE_SOLENOID_BASE_ADDR+EE_SOLENOID_DLY_STRT), value);
            printf_P(PSTR("\"delay_start_sec\":\"%lu\","),(value));
            command_done = 12;
        }
    }
    else if ( (command_done == 12) )
    {
        if ( eeprom_is_ready() )
        {
            uint8_t k_solenoid = atoi(arg[0]);
            uint32_t value = k[k_solenoid-1].runtime_sec;
            eeprom_write_dword( (uint32_t *)((k_solenoid-1)*EE_SOLENOID_ARRAY_OFFSET+EE_SOLENOID_BASE_ADDR+EE_SOLENOID_RUNTIME), value);
            printf_P(PSTR("\"runtime_sec\":\"%lu\","),(value));
            command_done = 13;
        }
    }
    else if ( (command_done == 13) )
    {  
        if ( eeprom_is_ready() )
        {
            uint8_t k_solenoid = atoi(arg[0]);
            uint32_t value = k[k_solenoid-1].delay_sec;
            eeprom_write_dword( (uint32_t *)((k_solenoid-1)*EE_SOLENOID_ARRAY_OFFSET+EE_SOLENOID_BASE_ADDR+EE_SOLENOID_DELAY), value);
            printf_P(PSTR("\"delay_sec\":\"%lu\","),(value));
            command_done = 14;
        }
    }
    else if ( (command_done == 14) )
    {
        if ( eeprom_is_ready() )
        {
            uint8_t k_solenoid = atoi(arg[0]);
            uint32_t value = k[k_solenoid-1].flow_stop;
            eeprom_write_dword( (uint32_t *)((k_solenoid-1)*EE_SOLENOID_ARRAY_OFFSET+EE_SOLENOID_BASE_ADDR+EE_SOLENOID_FLW_STP), value);
            if (k[k_solenoid-1].flow_stop != FLOW_NOT_SET)
            {
                printf_P(PSTR("\"flow_stop\":\"%lu\","),(value));
            }
            command_done = 15;
        }
    }
    else if ( (command_done == 15) )
    {
        if ( eeprom_is_ready() )
        {
            uint8_t k_solenoid = atoi(arg[0]);
            uint8_t value = k[k_solenoid-1].cycles;
            eeprom_write_byte( (uint8_t *)((k_solenoid-1)*EE_SOLENOID_ARRAY_OFFSET+EE_SOLENOID_BASE_ADDR+EE_SOLENOID_CYCLES), value);
            printf_P(PSTR("\"cycles\":\"%d\""),(value));
            command_done = 16;
        }
    }
    else if ( (command_done == 16) )
    {
        printf_P(PSTR("}}\r\n"));
        initCommandBuffer();
    }
    else
    {
        printf_P(PSTR("{\"err\":\"SavCmdDnWTF\"}\r\n"));
        initCommandBuffer();
    }
}

// arg[0] is solenoid
void Load(void)
{
    if ( (command_done == 10) )
    {
        uint8_t k_solenoid = k_solenoid_from_arg0();
        if (! k_solenoid)
        {
            initCommandBuffer();
            return;
        }

         // don't load a solenoid that is in use
        if (k[k_solenoid-1].cycle_state)
        {
            printf_P(PSTR("{\"err\":\"K%dInUse\"}\r\n"),k_solenoid);
            initCommandBuffer();
            return;
        }
        if ( eeprom_is_ready() )
        {
            if (LoadSolenoidControlFromEEPROM(k_solenoid))
            {
                k[k_solenoid-1].flow_cnt_bank = 0;
                k[k_solenoid-1].cycle_millis_bank = 0;
                printf_P(PSTR("{\"K%d\":{"),k_solenoid);
                command_done = 11;
            }
            else
            {
                printf_P(PSTR("{\"err\":\"LdFailK%dnoEEP\"}\r\n"),k_solenoid);
                initCommandBuffer();
                return;
            }
        }
    }
    else if ( (command_done == 11) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"delay_start_sec\":\"%lu\","),(k[k_solenoid-1].delay_start_sec));
        command_done = 12;
    }
    else if ( (command_done == 12) )
    {
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"runtime_sec\":\"%lu\","),(k[k_solenoid-1].runtime_sec));
        command_done = 13;
    }
    else if ( (command_done == 13) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"delay_sec\":\"%lu\","),(k[k_solenoid-1].delay_sec));
        command_done = 14;
    }
    else if ( (command_done == 14) )
    {
        uint8_t k_solenoid = atoi(arg[0]);
        if (k[k_solenoid-1].flow_stop != FLOW_NOT_SET)
        {
            printf_P(PSTR("\"flow_stop\":\"%lu\","),(k[k_solenoid-1].flow_stop));
        }
        command_done = 15;
    }
    else if ( (command_done == 15) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"cycles\":\"%d\""),(k[k_solenoid-1].cycles));
        command_done = 16;
    }
    else if ( (command_done == 16) )
    {
        printf_P(PSTR("}}\r\n"));
        initCommandBuffer();
    }
    else
    {
        printf_P(PSTR("{\"err\":\"LdCmdDnWTF\"}\r\n"));
        initCommandBuffer();
    }
}

// arg[0] is solenoid
void Time(void)
{
    if ( (command_done == 10) )
    {
        uint8_t k_solenoid = k_solenoid_from_arg0();
        if (! k_solenoid)
        {
            initCommandBuffer();
            return;
        }
        printf_P(PSTR("{\"K%d\":{"),k_solenoid);
        command_done = 11;
    }
    else if ( (command_done == 11) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"cycle_state\":\"%d\","),(k[k_solenoid-1].cycle_state));
        command_done = 12;
    }
    else if ( (command_done == 12) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"cycles\":\"%d\","),(k[k_solenoid-1].cycles));
        command_done = 13;
    }
    else if ( (command_done == 13) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"cycle_millis\":\"%lu\""),(k[k_solenoid-1].cycle_millis_bank));
        command_done = 14;
    }
    else if ( (command_done == 14) )
    {
        printf_P(PSTR("}}\r\n"));
        initCommandBuffer();
    }
    else
    {
        printf_P(PSTR("{\"err\":\"TmCmdDnWTF\"}\r\n"));
        initCommandBuffer();
    }
}

// arg[0] is solenoid
void Flow(void)
{
    if ( (command_done == 10) )
    {
        uint8_t k_solenoid = k_solenoid_from_arg0();
        if (! k_solenoid)
        {
            initCommandBuffer();
            return;
        }
        printf_P(PSTR("{\"K%d\":{"),k_solenoid);
        command_done = 11;
    }
    else if ( (command_done == 11) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"cycle_state\":\"%d\","),(k[k_solenoid-1].cycle_state));
        command_done = 12;
    }
    else if ( (command_done == 12) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"cycles\":\"%d\","),(k[k_solenoid-1].cycles));
        command_done = 13;
    }
    else if ( (command_done == 13) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        printf_P(PSTR("\"flow_cnt\":\"%lu\""),(k[k_solenoid-1].flow_cnt_bank));
        command_done = 14;
    }
    else if ( (command_done == 14) )
    {
        printf_P(PSTR("}}\r\n"));
        initCommandBuffer();
    }
    else
    {
        printf_P(PSTR("{\"err\":\"FlwCmdDnWTF\"}\r\n"));
        initCommandBuffer();
    }
}

// arg[0] is solenoid
void Stop(void)
{
    if ( (command_done == 10) )
    {
        uint8_t k_solenoid = k_solenoid_from_arg0();
        if (! k_solenoid)
        {
            initCommandBuffer();
            return;
        }
        printf_P(PSTR("{\"K%d\":{"),k_solenoid);
        command_done = 11;
    }
    else if ( (command_done == 11) )
    {  
        uint8_t k_solenoid = atoi(arg[0]);
        if (k[k_solenoid-1].cycle_state)
        {
            k[k_solenoid-1].delay_start_sec = 1;
            k[k_solenoid-1].runtime_sec = 1; 
            k[k_solenoid-1].delay_sec = 1;
            k[k_solenoid-1].flow_stop = FLOW_NOT_SET;
            k[k_solenoid-1].cycles = 1;
            printf_P(PSTR("\"stop_time_sec\":\"3\""));
        }
        else
        {
            printf_P(PSTR("\"stop_time_sec\":\"0\""));
        }
        command_done = 12;
    }
    else if ( (command_done == 12) )
    {
        printf_P(PSTR("}}\r\n"));
        initCommandBuffer();
    }
    else
    {
        printf_P(PSTR("{\"err\":\"StpCmdDnWTF\"}\r\n"));
        initCommandBuffer();
    }
}

void bridge_off() {
    digitalWrite(K3_E3,LOW); // disables 74HC238 that operate BOOST SMPS and K1..K3 half bridges
    digitalWrite(K7_E3,LOW); // disables 74HC238 that operate K4..K7 half bridges
}

// given a solenoid index config the bridges and apply the SET pulse. 
// A zero index will turn on Y0 which does nothing.
void set_solenoid(uint8_t k_indx) 
{
    digitalWrite(K3_E3,LOW); // disables 74HC238 that operate BOOST SMPS and K1..K3 half bridge
    digitalWrite(K7_E3,LOW); // disables 74HC238 that operate K4..K7 
    
    // each ndex has two operations
    // set is used to drive A0, A1, A2 so mask off the upper bits to cause values in range 4..7 to be in the range 0..3
    // left shift for a value twice that of the index
    // 0 => 0<<1 => Y0 is NA
    // 1 => 1<<1 => Y2 is SET-K1
    // 2 => 2<<1 => Y4 is SET-K2 
    // 3 => 3<<1 => Y6 is SET-K3
    uint8_t set = (k_indx & 0x03) << 1; 
    
    // set A0:A1:A2 bit
    digitalWrite(A0, LOW); // A0 is always LOW, because the 74HC238 decode for SET is an even value 
    if( set & (1<<1) )
    {
        digitalWrite(A1, HIGH);
    }
    else
    {
        digitalWrite(A1, LOW);
    }
    if( set & (1<<2) )
    {
        digitalWrite(A2, HIGH);
    }
    else
    {
        digitalWrite(A2, LOW);
    }
    
    // which 74HC238 chip should I sellect
    if (k_indx <= 3)
    {
        digitalWrite(K3_E3,HIGH);
    }
    else
    {
        digitalWrite(K7_E3,HIGH);
    }
}

// given a solenoid index config the bridge and apply a RESET pulse. 
// A zero index will trun on the SMPS boost and charge the storage cap.
void reset_solenoid(uint8_t k_indx) 
{
    digitalWrite(K3_E3,LOW); // disables 74HC238 that operate BOOST SMPS and K1..K3 half bridge
    digitalWrite(K7_E3,LOW); // disables 74HC238 that operate K4..K7 
    
    // each ndex has two operations
    // set is used to drive A0, A1, A2 so mask off the upper bits to cause values in range 4..7 to be in the range 0..3
    // left shift for a value twice that of the index
    // 0 => (0<<1)+1 => Y1 is SMPS
    // 1 =>(1<<1)+1 => Y3 is RESET-K1
    // 2 => (2<<1)+1 => Y5 is RESET-K2 
    // 3 => (3<<1)+1 => Y7 is RESET-K3
    uint8_t set = (k_indx & 0x03) << 1; 
    
    // set A0:A1:A2 bit
    digitalWrite(A0, HIGH); // A0 is always HIGH, because the 74HC238 decode for RESET is an odd value 
    if( set & (1<<1) )
    {
        digitalWrite(A1, HIGH);
    }
    else
    {
        digitalWrite(A1, LOW);
    }
    if( set & (1<<2) )
    {
        digitalWrite(A2, HIGH);
    }
    else
    {
        digitalWrite(A2, LOW);
    }
    
    // which 74HC238 chip should I sellect
    if (k_indx <= 3)
    {
        digitalWrite(K3_E3,HIGH);
    }
    else
    {
        digitalWrite(K7_E3,HIGH);
    }
}

/* operate the solenoid states without blocking
    cycle_state 
    0 = solenoid not active
    1 = active, wait for start time (delay_start_sec), then start a Boost charge and setup to measure flow.
    2 = wait for BOOST_TIME to finish, then set solenoid.
    3 = wait for PWR_HBRIDGE time to drive solenoid, then trun off H-bridge.
    4 = wait for runTime, then select state 7. or if within 16M counts of flow_stop select state 5. 
    5 = wait for boostInUse, then start a Boost charge
    6 = wait for flow_stop, then reset solenoid and measure flow and slect state 9.
    7 = wait for boostInUse, then start a Boost charge
    8 = wait for BOOST_TIME to finish, then reset solenoid and measure flow rate.
    9 = wait for PWR_HBRIDGE time to drive solenoid, then trun off H-bridge.
    10 = wait for SOLENOID_CLOSE time, then measure flow count, if cycles is set then state 0.
    11 = wait for delay time, then loop to cycle_state = 1 (backdate so delay_start_sec is not used in each loop).
*/ 
void SolenoidControl() {
    for(int i = 0; i < SOLENOID_COUNT; i++){
        // active, wait for start time (delay_start_sec), then start a Boost charge and setup to measure pulse count.
        if ((k[i].cycle_state == 1) && !boostInUse && !flowInUse) 
        {
            unsigned long kRuntime= millis() - k[i].started_at;
            if ((kRuntime) > ((unsigned long)k[i].delay_start_sec * 1000)) 
            {
                reset_solenoid(BOOST);
                k[i].cycle_state = 2;
                k[i].started_at = millis(); // boost started
                boostInUse = i+1; //allow this solenoid to use boost
                flowInUse = i+1; //allow this solenoid to use flow meter
                break;
            }
        }
        if (flowInUse == i+1) // only let the solenoid that has been allocated use of the flow meter run
        {
            // wait for BOOST_TIME to finish, then select solenoid control line to drive.
            if ((k[i].cycle_state == 2) && (boostInUse == i+1)) 
            {
                unsigned long kRuntime= millis() - k[i].started_at;
                if ((kRuntime) > ((unsigned long)BOOST_TIME)) 
                {
                    set_solenoid(i+1);
                    k[i].started_at = millis(); //start time to wait for H-bridge to power a SET
                    k[i].cycle_state = 3;
                    break;
                }
            }

            // wait for PWR_HBRIDGE time to drive solenoid, then trun off H-bridge.
            if ((k[i].cycle_state == 3) && (boostInUse == i+1)) 
            {
                unsigned long kRuntime= millis() - k[i].started_at;
                if ((kRuntime) > ((unsigned long)PWR_HBRIDGE)) 
                {
                    boostInUse = 0;
                    bridge_off();
                    k[i].started_at = millis(); //start runtime e.g. solenoid is set
                    k[i].cycle_millis_start = millis(); 
                    k[i].flow_cnt_start =  icp1.count;
                    k[i].cycle_state = 4;
                    break;
                }
            }

            // wait for runTime, then select state 7. or if not FLOW_NOT_SET counts select state 5.
            if (k[i].cycle_state == 4) 
            {
                unsigned long kRuntime= millis() - k[i].started_at;
                if ((kRuntime) > ((unsigned long)k[i].runtime_sec * 1000)-1) 
                {
                    k[i].cycle_millis_stop = millis(); // correction of -1 millis was added so timer shows expected value
                    k[i].cycle_state = 7;
                }
                if (k[i].flow_stop != FLOW_NOT_SET) 
                {  
                    if ( (icp1.count - k[i].flow_cnt_start) >= k[i].flow_stop) 
                    {
                        k[i].cycle_state = 5;
                        break;
                    }
                }
            }

            // wait for not boostInUse, then start a Boost charge
            if ((k[i].cycle_state == 5) && !boostInUse) 
            {
                reset_solenoid(BOOST);
                k[i].cycle_state = 6;
                k[i].started_at = millis(); // boost started
                boostInUse = i+1;
                break;
            }

            // wait for boost charge, then reset solenoid, measure flow  and select state 9.
            if ((k[i].cycle_state == 6) && (boostInUse == i+1)) 
            {
                unsigned long kRuntime= millis() - k[i].started_at;
                if (((kRuntime) > ((unsigned long)BOOST_TIME)) ) 
                {
                    reset_solenoid(i+1);
                    k[i].started_at = millis(); //start H-bridge SET has power
                    k[i].cycle_state = 9;
                    break;
                }
            }

            // wait for not boostInUse, then start a Boost charge
            if ((k[i].cycle_state == 7) && !boostInUse)
            {
                reset_solenoid(BOOST);
                k[i].cycle_state = 8;
                k[i].started_at = millis(); // boost started
                boostInUse = i+1;
                break;
            }

            // wait for BOOST_TIME to finish, then reset solenoid and measure flow rate.
            if ((k[i].cycle_state == 8) && (boostInUse == i+1)) 
            {
                unsigned long kRuntime= millis() - k[i].started_at;
                if ((kRuntime) > ((unsigned long)BOOST_TIME)) 
                {
                    reset_solenoid(i+1);
                    k[i].started_at = millis(); //start time to wait for H-bridge to power a RESET
                    k[i].cycle_state = 9;
                    break;
                }
            }
            
            // wait for PWR_HBRIDGE time to drive solenoid, then trun off H-bridge.
            if ((k[i].cycle_state == 9) && (boostInUse == i+1))
            {
                unsigned long kRuntime= millis() - k[i].started_at;
                if ((kRuntime) > ((unsigned long)PWR_HBRIDGE)) 
                {
                    boostInUse = 0;
                    bridge_off();
                    k[i].cycle_millis_bank += (k[i].cycle_millis_stop - k[i].cycle_millis_start);
                    k[i].started_at = millis(); // Solenoid may need some time to finish closing befor recording the flow count
                    k[i].cycle_state = 10;
                    break;
                }
            }
              
            // wait for SOLENOID_CLOSE time, then measure flow count.
            if ((k[i].cycle_state == 10)) 
            {
                unsigned long kRuntime= millis() - k[i].started_at;
                if ((kRuntime) > ((unsigned long)SOLENOID_CLOSE)) 
                {
                    flowInUse = 0; // this will allow the next solenoid cycle_state to progress 
                    k[i].flow_cnt_stop = icp1.count; //record the flow meter pulse count after solenoid has closed
                    k[i].flow_cnt_bank += (k[i].flow_cnt_stop - k[i].flow_cnt_start);
                    if (k[i].cycles)
                    {
                        k[i].cycles = k[i].cycles -1;
                    }
                    if (k[i].cycles)
                    {
                        k[i].started_at = millis();
                        k[i].cycle_state = 11;
                        break;
                    }
                    else
                    {
                        k[i].started_at = 0;
                        k[i].cycle_state = 0;
                        break;
                    }
                }
            }
        }
        
        // delay after reset operation and befor next set operation
        if ((k[i].cycle_state == 11)) 
        {
            unsigned long kRuntime= millis() - k[i].started_at;
            if ( (kRuntime) > ((unsigned long)k[i].delay_sec*1000UL) ) 
            {
                // back date by delay_start_sec since it should only be used once
                k[i].started_at = millis()-((unsigned long)k[i].delay_start_sec*1000UL);
                k[i].cycle_state = 1;
                break;
            }
        }
    }
}

// warning this sets all solenoids for 1 Second operation, 1 cycle, and sets cycle_state so they will run
void Reset_All_K() {
    for(uint8_t i = 0; i < SOLENOID_COUNT; i++)
    {
        k[i].started_at = millis(); // used to delay start
        k[i].delay_start_sec = i+i+i+1; // every 3 sec starting at 1, then 4, and 7
        k[i].runtime_sec = 1; 
        k[i].delay_sec = SEC_IN_HR;
        k[i].flow_stop = FLOW_NOT_SET;
        k[i].cycles = 1;
        k[i].cycle_state = 1;
        k[i].flow_cnt_bank = 0;
        k[i].cycle_millis_bank = 0;
    }
}

uint8_t LoadSolenoidControlFromEEPROM(uint8_t solenoid) 
{
    uint16_t i = solenoid-1;
    if (!k[i].cycle_state)
    {
        uint16_t id = eeprom_read_word((uint16_t*)(i*EE_SOLENOID_ARRAY_OFFSET+EE_SOLENOID_BASE_ADDR+EE_SOLENOID_ID));
        if (id == (i+0x4B31) ) // 'K' is 0x4B and '1' is 0x31, thus K1, K2...
        {
            k[i].started_at = millis(); // delay start is from now.
            k[i].delay_start_sec =eeprom_read_dword((uint32_t*)(i*EE_SOLENOID_ARRAY_OFFSET+EE_SOLENOID_BASE_ADDR+EE_SOLENOID_DLY_STRT)); 
            k[i].runtime_sec = eeprom_read_dword((uint32_t*)(i*EE_SOLENOID_ARRAY_OFFSET+EE_SOLENOID_BASE_ADDR+EE_SOLENOID_RUNTIME));  
            k[i].delay_sec = eeprom_read_dword((uint32_t*)(i*EE_SOLENOID_ARRAY_OFFSET+EE_SOLENOID_BASE_ADDR+EE_SOLENOID_DELAY)); 
            k[i].flow_stop = eeprom_read_dword((uint32_t*)(i*EE_SOLENOID_ARRAY_OFFSET+EE_SOLENOID_BASE_ADDR+EE_SOLENOID_FLW_STP)); 
            k[i].cycles = eeprom_read_byte((uint8_t*)(i*EE_SOLENOID_ARRAY_OFFSET+EE_SOLENOID_BASE_ADDR+EE_SOLENOID_CYCLES)); 
            k[i].flow_cnt_bank = 0;
            k[i].cycle_millis_bank = 0;
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

// return the solenoid cycle_state (e.g. tells if it is running)
uint8_t Live(uint8_t solenoid) 
{
    uint16_t i = solenoid-1;
    if (i<SOLENOID_COUNT)
    {
        return k[i].cycle_state;
    }
    else return 0xFF; // not a valid solenoid 
}

// start the solenoid if it is not running and return cycle state
uint8_t StartSolenoid(uint8_t solenoid) 
{
    uint16_t i = solenoid-1;
    if (i<SOLENOID_COUNT )
    {
        if (! k[i].cycle_state) k[i].cycle_state = 1;
        return k[i].cycle_state;
    }
    else return 0x0; // not a valid solenoid
}

// only use init at setup() not durring loop()
void init_K(void) 
{
    boostInUse = 0;
    flowInUse = 0;
    pinMode(K3_E3,OUTPUT);
    digitalWrite(K3_E3,LOW);
    pinMode(K7_E3,OUTPUT);
    digitalWrite(K7_E3,LOW);
    pinMode(CL8_E3,OUTPUT); // CL8 is an external board
    digitalWrite(CL8_E3,LOW);
    pinMode(A0,OUTPUT);
    digitalWrite(A0,LOW);
    pinMode(A1,OUTPUT);
    digitalWrite(A1,LOW);
    pinMode(A2,OUTPUT);
    digitalWrite(A2,LOW); 
}
