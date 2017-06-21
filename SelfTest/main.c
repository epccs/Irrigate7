/* 
Irrigate7 SelfTest
Copyright (C) 2017 Ronald Sutherland

This Library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with the Arduino DigitalIO Library.  If not, see
<http://www.gnu.org/licenses/>.
*/ 

#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/io.h>
#include "../lib/timers.h"
#include "../lib/uart.h"
#include "../lib/twi.h"
#include "../lib/adc.h"
#include "../lib/rpu_mgr.h"
#include "../lib/pin_num.h"
#include "../lib/pins_board.h"
#include "../Adc/references.h"

#define BLINK_DELAY 1000UL

// Save the Value of the References for ADC converter 
// measure AVCC and put it hear in uV e.g. 5.00V is 5000000UL
#define REF_EXTERN_AVCC 4995900UL
// I am not sure how to measure the 1V1 bandgap, this is just a holding place
#define REF_INTERN_1V1 1100000UL

#define R1 47.0
#define R2 47.0
#define ICP1_TERM 100.0
#define ICP3_TERM 100.0

static unsigned long blink_started_at;
static unsigned long blink_delay;
static char rpu_addr;
static uint8_t passing;

void setup(void) 
{
    // Turn Off Curr Sources 
    pinMode(CURR_SOUR_EN,OUTPUT);
    digitalWrite(CURR_SOUR_EN,LOW);

    // Turn Off VOUT to shield (e.g. disconnect VIN from shield)
    pinMode(SHLD_VIN_EN,OUTPUT);
    digitalWrite(SHLD_VIN_EN,LOW);

    // Battery disconnect (don't let the pin glitch with a high)
    digitalWrite(BAT_DISCONNECT,LOW);
    pinMode(BAT_DISCONNECT,OUTPUT);

    // Charge control (don't let the pin glitch with a high)
    digitalWrite(CC_SHUTDOWN,LOW);
    pinMode(CC_SHUTDOWN,OUTPUT);

    // To Check CC fault state drive pin with a weak pull-up
    digitalWrite(CC_nFAULT,HIGH);
    pinMode(CC_nFAULT,INPUT);

    // Set plugable DIO to shunt current sources, each has 182 Ohm. (e.g. turn LED's off) 
    pinMode(DIO2,OUTPUT);
    digitalWrite(DIO2,LOW);
    pinMode(DIO3,OUTPUT);
    digitalWrite(DIO3,LOW);
    pinMode(DIO4,OUTPUT);
    digitalWrite(DIO4,LOW);
    pinMode(DIO10,OUTPUT);
    digitalWrite(DIO10,LOW);
    pinMode(DIO11,OUTPUT);
    digitalWrite(DIO11,LOW);
    pinMode(DIO12,OUTPUT);
    digitalWrite(DIO12,LOW);
    pinMode(DIO13,OUTPUT);
    digitalWrite(DIO13,LOW);

    // Initialize Timers, ADC, and clear bootloader, Arduino does these with init() in wiring.c
    initTimers(); //Timer0 Fast PWM mode, Timer1 & Timer2 Phase Correct PWM mode.
    init_ADC_single_conversion(EXTERNAL_AVCC); // warning AREF must not be connected to anything
    init_uart0_after_bootloader(); // bootloader may have the UART setup

    /* Initialize UART, it returns a pointer to FILE so redirect of stdin and stdout works*/
    stdout = stdin = uartstream0_init(BAUD);

    /* Initialize I2C, with the internal pull-up*/
    twi_init(TWI_PULLUP);

    // Enable global interrupts to start TIMER0 and UART
    sei(); 

    blink_started_at = millis();

    rpu_addr = get_Rpu_address();
    blink_delay = BLINK_DELAY;

    // blink fast if a default address from RPU manager not found
    if (rpu_addr == 0)
    {
        rpu_addr = '0';
        blink_delay = BLINK_DELAY/4;
    }
    
    // set the referances and save them in EEPROM
    ref_extern_avcc_uV = REF_EXTERN_AVCC;
    ref_intern_1v1_uV = REF_INTERN_1V1;
    while ( !WriteEeReferenceId() ) {};
    while ( !WriteEeReferenceAvcc() ) {};
    while ( !WriteEeReference1V1() ) {};
}

void test(void)
{
    // Info
    printf_P(PSTR("Irrigate7 DIO and ADC Self Test date: %s\r\n"), __DATE__);
    
    // I2C is used to read RPU bus manager address
    if (rpu_addr == '1')
    {
        printf_P(PSTR("I2C provided address 0x31 from RPU bus manager\r\n"));
    } 
    else  
    { 
        passing = 0; 
        printf_P(PSTR(">>> I2C failed, or address not 0x31 from RPU bus manager\r\n"));
        return;
    }

    // +5V is used as the ADC reference
    printf_P(PSTR("+5V needs measured and then set as REF_EXTERN_AVCC: %1.3f V\r\n"), (ref_extern_avcc_uV/1.0E6));

    // Current sources are off
    // Charge rate OK for 150mA input?
    // note the 22mA@DIO2 and 22mA@DIO10 are still on
    _delay_ms(100) ; // busy-wait delay
    float chrg_i = analogRead(CHRG_I)*((ref_extern_avcc_uV/1.0E6)/1024.0)/(0.068*50.0);
    printf_P(PSTR("Charging with CURR_SOUR_EN==low: %1.3f A\r\n"), chrg_i);
    if (chrg_i > 0.15) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> Charge rate is to high, is PV supply CC set at 150mA?\r\n"));
        return;
    }
    if (chrg_i < 0.01) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> Charge rate is to low, is PV supply CC set at 150mA?\r\n"));
        return;
    }

    // Battery voltage
    float battery_v = analogRead(PWR_V)*((ref_extern_avcc_uV/1.0E6)/1024.0)*(3.0/1.0);
    printf_P(PSTR("PWR (Battery) at: %1.3f V\r\n"), battery_v);
    if (battery_v > 14.0) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> Battery is to high, verify with DMM, turn off the PV supply if correct.\r\n"));
        return;
    }
    if (battery_v < 12.0) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> Battery is to low, verify with DMM, and charge it if correct.\r\n"));
        return;
    }
    
    // MPPT voltage should be on the the supply (assuming battery is not at float voltage)
    float mppt_v = analogRead(PV_V)*((ref_extern_avcc_uV/1.0E6)/1024.0)*(532.0/100.0);
    printf_P(PSTR("MPPT at: %1.3f V\r\n"), mppt_v);
    if (mppt_v > 17.2) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> MPPT is to high, is battery at float voltage?\r\n"));
        return;
    }
    if (mppt_v < 16.4) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> MPPT is to low.\r\n"));
        return;
    }

    // ADC0 and ADC1 with current sources off
    float adc0_v = analogRead(ADC0)*((ref_extern_avcc_uV/1.0E6)/1024.0);
    printf_P(PSTR("ADC0 at: %1.3f V\r\n"), adc0_v);
    float adc1_v = analogRead(ADC1)*((ref_extern_avcc_uV/1.0E6)/1024.0);
    printf_P(PSTR("ADC1 at: %1.3f V\r\n"), adc1_v);
    float adc4_v = analogRead(ADC4)*((ref_extern_avcc_uV/1.0E6)/1024.0);
    printf_P(PSTR("ADC4 at: %1.3f V\r\n"), adc0_v);
    float adc5_v = analogRead(ADC5)*((ref_extern_avcc_uV/1.0E6)/1024.0);
    printf_P(PSTR("ADC5 at: %1.3f V\r\n"), adc1_v);
    if ( (adc0_v > 0.01)  || (adc1_v > 0.01) || (adc4_v > 0.01)  || (adc5_v > 0.01))
    { 
        passing = 0; 
        printf_P(PSTR(">>> ADC is to high, is the self-test wiring right?\r\n"));
        return;
    }

    // ICP1 pin is inverted from to the plug interface, which should have zero mA on its 100 Ohm Termination now
    printf_P(PSTR("ICP1 /w 0mA on plug termination reads: %d \r\n"), digitalRead(ICP1));
    if (!digitalRead(ICP1)) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> ICP1 should be high.\r\n"));
    }

    // ICP3 pin is inverted from to the plug interface, which should have zero mA on its 100 Ohm Termination now
    printf_P(PSTR("ICP3 /w 0mA on plug termination reads: %d \r\n"), digitalRead(ICP3));
    if (!digitalRead(ICP3)) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> ICP3 should be high.\r\n"));
    }
    
    // CC nFAULT with weak pull-up
    printf_P(PSTR("CC_nFAULT measured with a weak pull-up: %d \r\n"), digitalRead(CC_nFAULT));
    if (!digitalRead(CC_nFAULT)) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> CC_nFAULT should be high\r\n"));
    }
    
    // 22MA@DIO10 feeds R1 if DIO13 is high-z
    pinMode(DIO13,INPUT);
    _delay_ms(50) ; // busy-wait delay
    float dio10at22mA_i = analogRead(ADC0)*((ref_extern_avcc_uV/1.0E6)/1024.0) / R1;
    printf_P(PSTR("22MA@DIO10 curr source on R1: %1.3f A\r\n"), dio10at22mA_i);
    if (dio10at22mA_i < 0.017) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22MA@DIO10 curr is to low\r\n"));
    }
    if (dio10at22mA_i > 0.023) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22MA@DIO10 curr is to high\r\n"));
    }
    pinMode(DIO13,OUTPUT);
    
    // 22MA@DIO2 feeds R2 if DIO10 is high-z
    pinMode(DIO10,INPUT);
    _delay_ms(50) ; // busy-wait delay
    float dio2at22mA_i = analogRead(ADC5)*((ref_extern_avcc_uV/1.0E6)/1024.0) / R1;
    printf_P(PSTR("22MA@DIO2 curr source on R2: %1.3f A\r\n"), dio2at22mA_i);
    if (dio2at22mA_i < 0.017) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22MA@DIO2 curr is to low\r\n"));
    }
    if (dio2at22mA_i > 0.023) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22MA@DIO2 curr is to high\r\n"));
    }
    pinMode(DIO10,OUTPUT);
    
    // enable the current sources, note the DIO are set to sink current so this is some MCU stress
    digitalWrite(CURR_SOUR_EN,HIGH);
    _delay_ms(500) ; // busy-wait delay
    float dischrg_i = analogRead(DISCHRG_I)*((ref_extern_avcc_uV/1.0E6)/1024.0)/(0.068*50.0);
    printf_P(PSTR("Dischrg with CURR_SOUR_EN==high: %1.3f A\r\n"), dischrg_i);
    float chrg_delta_i = chrg_i + dischrg_i;
    printf_P(PSTR("Chrg delta with CURR_SOUR_EN==high: %1.3f A\r\n"), chrg_delta_i);
    
    // 22MA@ADC0 is feeding R1  (DIO12 is sinking 22MA@ADC1, DIO13 is sinking 22MA@DIO10)
    float adc0at22mA_i = analogRead(ADC0)*((ref_extern_avcc_uV/1.0E6)/1024.0) / R1;
    printf_P(PSTR("22MA@ADC0 on R1: %1.3f A\r\n"), adc0at22mA_i);
    if (adc0at22mA_i < 0.017) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22MA@ADC0 curr is to low\r\n"));
    }
    if (adc0at22mA_i > 0.023) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22MA@ADC0 curr is to high\r\n"));
    }
    
    // 22MA@ADC5 is feeding R2  (DIO11 is sinking 22MA@ADC4, DIO10 is sinking 22MA@DIO2)
    float adc5at22mA_i = analogRead(ADC5)*((ref_extern_avcc_uV/1.0E6)/1024.0) / R1;
    printf_P(PSTR("22MA@ADC5 on R2: %1.3f A\r\n"), adc5at22mA_i);
    if (adc5at22mA_i < 0.017) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22MA@ADC5 curr is to low\r\n"));
    }
    if (adc5at22mA_i > 0.023) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22MA@ADC5 curr is to high\r\n"));
    }
    
    // ADC1 can read ICP1's 10mA source using the on board 100 Ohm termination resistor from its PL input
    float icp1at10mA_i = analogRead(ADC1)*((ref_extern_avcc_uV/1.0E6)/1024.0) / ICP1_TERM;
    printf_P(PSTR("10MA@ICP1 with ICP1 PL input: %1.3f A\r\n"), icp1at10mA_i);
    if (icp1at10mA_i < 0.007) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 10MA@ICP1 curr is to low\r\n"));
    }
    if (icp1at10mA_i > 0.013) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 10MA@ICP1 curr is to high\r\n"));
    }

    // ICP1 pin is inverted from to the plug interface, which should have 10 mA on its 100 Ohm Termination now
    printf_P(PSTR("ICP1 /w 10mA on plug termination reads: %d \r\n"), digitalRead(ICP1));
    if (digitalRead(ICP1)) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> ICP1 should be low with 10mA.\r\n"));
    }

    // ADC4 can read ICP3's 10mA source using the on board 100 Ohm termination resistor from its PL input
    float icp3at10mA_i = analogRead(ADC4)*((ref_extern_avcc_uV/1.0E6)/1024.0) / ICP3_TERM;
    printf_P(PSTR("10MA@ICP3 with ICP3 PL input: %1.3f A\r\n"), icp3at10mA_i);
    if (icp3at10mA_i < 0.007) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 10MA@ICP3 curr is to low\r\n"));
    }
    if (icp3at10mA_i > 0.013) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 10MA@ICP3 curr is to high\r\n"));
    }
    
    // ICP1 pin is inverted from to the plug interface, which should have 10 mA on its 100 Ohm Termination now
    printf_P(PSTR("ICP3 /w 10mA on plug termination reads: %d \r\n"), digitalRead(ICP3));
    if (digitalRead(ICP3)) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> ICP3 should be low with 10mA.\r\n"));
    }
    
    // charge control shutdown
    digitalWrite(CC_SHUTDOWN,HIGH);
    _delay_ms(200) ; // busy-wait delay. It needs some some time for my supply to switch from CC mode to CV mode.
    float dischrg_ccoff_i = analogRead(DISCHRG_I)*((ref_extern_avcc_uV/1.0E6)/1024.0)/(0.068*50.0);
    printf_P(PSTR("Dischrging with CC_SHUTDOWN==high : %1.3f A\r\n"), dischrg_ccoff_i);
    if (dischrg_ccoff_i < 0.05) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> Discharging is to low.\r\n"));
    }

    // PV open circuit voltage (LT3652 is off)
    float pvoc_v = analogRead(PV_V)*((ref_extern_avcc_uV/1.0E6)/1024.0)*(532.0/100.0);
    printf_P(PSTR("PV open circuit (LT3652 off) at: %1.3f V\r\n"), pvoc_v);
    if (pvoc_v < 19.0) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> PV open circuit voltage is low (set CV mode at 21V).\r\n"));
    }
    
    // DIO12 high-z to add 22mA@ADC1 to R1
    pinMode(DIO12,INPUT);
    _delay_ms(50) ; // busy-wait delay
    float adc0at22mA_adc1at22mA_i = analogRead(ADC0)*((ref_extern_avcc_uV/1.0E6)/1024.0) / R1;
    printf_P(PSTR("22mA@ADC0 and 22mA@ADC1 on R1: %1.3f A\r\n"), adc0at22mA_adc1at22mA_i);
    if (adc0at22mA_adc1at22mA_i < 0.040) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22mA@ADC0 and 22mA@ADC1 curr is to low\r\n"));
    }
    if (adc0at22mA_adc1at22mA_i > 0.047) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22mA@ADC0 and 22mA@ADC1 curr is to high\r\n"));
    }

    // DIO13 high-z to add another curr source 22MA@DIO10 to R1
    pinMode(DIO13,INPUT);
    _delay_ms(50) ; // busy-wait delay
    float dio10at22mA_adc0at22mA_adc1at22mA_i = analogRead(ADC0)*((ref_extern_avcc_uV/1.0E6)/1024.0) / R1;
    printf_P(PSTR("22MA@DIO10, 22mA@ADC0, 22mA@ADC1 on R1: %1.3f A\r\n"), dio10at22mA_adc0at22mA_adc1at22mA_i);
    if (dio10at22mA_adc0at22mA_adc1at22mA_i < 0.060) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22MA@DIO10, 22mA@ADC0, 22mA@ADC1 curr is to low\r\n"));
    }
    if (dio10at22mA_adc0at22mA_adc1at22mA_i > 0.070) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22MA@DIO10, 22mA@ADC0, 22mA@ADC1 curr is to high.\r\n"));
    }
    pinMode(DIO12,OUTPUT);
    pinMode(DIO13,OUTPUT);    

    // DIO11 high-z to add 22mA@ADC4 to R2
    pinMode(DIO11,INPUT);
    _delay_ms(50) ; // busy-wait delay
    float adc4at22mA_adc5at22mA_i = analogRead(ADC5)*((ref_extern_avcc_uV/1.0E6)/1024.0) / R1;
    printf_P(PSTR("22mA@ADC4 and 22mA@ADC5 on R2: %1.3f A\r\n"), adc4at22mA_adc5at22mA_i);
    if (adc4at22mA_adc5at22mA_i < 0.040) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22mA@ADC4 and 22mA@ADC5 curr is to low\r\n"));
    }
    if (adc4at22mA_adc5at22mA_i > 0.047) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22mA@ADC4 and 22mA@ADC5 curr is to high\r\n"));
    }

    // DIO10 high-z to add another curr source 22MA@DIO2 to R2
    pinMode(DIO10,INPUT);
    _delay_ms(50) ; // busy-wait delay
    float dio2at22mA_adc4at22mA_adc4at22mA_i = analogRead(ADC5)*((ref_extern_avcc_uV/1.0E6)/1024.0) / R1;
    printf_P(PSTR("22MA@DIO2, 22mA@ADC4, 22mA@ADC5 on R2: %1.3f A\r\n"), dio2at22mA_adc4at22mA_adc4at22mA_i);
    if (dio2at22mA_adc4at22mA_adc4at22mA_i < 0.060) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22MA@DIO2, 22mA@ADC4, 22mA@ADC5 curr is to low\r\n"));
    }
    if (dio2at22mA_adc4at22mA_adc4at22mA_i > 0.070) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> 22MA@DIO2, 22mA@ADC4, 22mA@ADC5 curr is to high.\r\n"));
    }
    pinMode(DIO11,OUTPUT);
    pinMode(DIO10,OUTPUT);  

    // DIO3 and DIO4 high-z to add icp1's 17mA curr source to the 10mA on the 100 Ohm termination with PL input
    pinMode(DIO3,INPUT);
    pinMode(DIO4,INPUT);
    _delay_ms(50) ; // busy-wait delay
    float icp1at17mA_icp1at10mA_i = analogRead(ADC1)*((ref_extern_avcc_uV/1.0E6)/1024.0) / ICP1_TERM;
    printf_P(PSTR("ICP1 10mA + 17mA curr source on ICP1_TERM: %1.3f A\r\n"), icp1at17mA_icp1at10mA_i);
    if (icp1at17mA_icp1at10mA_i < 0.025) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> ICP1 10mA and 17mA curr source is to low\r\n"));
    }
    if (icp1at17mA_icp1at10mA_i > 0.031) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> ICP1 10mA and 17mA curr source is to high.\r\n"));
    }

    // DIO3 high-z alone 
    pinMode(DIO4,OUTPUT);
    _delay_ms(50) ; // busy-wait delay
    float icp1at17mA_icp1at10mA_dio4sink_i = analogRead(ADC1)*((ref_extern_avcc_uV/1.0E6)/1024.0) / ICP1_TERM;
    printf_P(PSTR("ICP1 10mA with DIO4 sinking 17mA: %1.3f A\r\n"), icp1at17mA_icp1at10mA_dio4sink_i);
    if (icp1at17mA_icp1at10mA_dio4sink_i > 0.018) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> DIO4 is not sinking.\r\n"));
    }

    // DIO4 high-z alone
    pinMode(DIO4,INPUT);
    pinMode(DIO3,OUTPUT);
    _delay_ms(50) ; // busy-wait delay
    float icp1at17mA_icp1at10mA_dio3sink_i = analogRead(ADC0)*((ref_extern_avcc_uV/1.0E6)/1024.0) / ICP1_TERM;
    printf_P(PSTR("ICP1 10mA with DIO3 sinking 17mA: %1.3f A\r\n"), icp1at17mA_icp1at10mA_dio3sink_i);
    if (icp1at17mA_icp1at10mA_dio3sink_i > 0.018) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> DIO3 is not sinking.\r\n"));
    }
    pinMode(DIO3,INPUT);

    // DIO2 high-z to add icp3's 17mA curr source to the 10mA on the 100 Ohm termination with PL input
    pinMode(DIO2,INPUT);
    _delay_ms(50) ; // busy-wait delay
    float icp3at17mA_icp3at10mA_i = analogRead(ADC4)*((ref_extern_avcc_uV/1.0E6)/1024.0) / ICP1_TERM;
    printf_P(PSTR("ICP3 10mA + 17mA curr source on ICP3_TERM: %1.3f A\r\n"), icp3at17mA_icp3at10mA_i);
    if (icp3at17mA_icp3at10mA_i < 0.025) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> ICP3 10mA and 17mA curr source is to low\r\n"));
    }
    if (icp3at17mA_icp3at10mA_i > 0.031) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> ICP3 10mA and 17mA curr source is to high.\r\n"));
    }

    // DIO2 high-z alone 
    pinMode(DIO2,OUTPUT);
    _delay_ms(50) ; // busy-wait delay
    float icp3at17mA_icp3at10mA_dio2sink_i = analogRead(ADC4)*((ref_extern_avcc_uV/1.0E6)/1024.0) / ICP1_TERM;
    printf_P(PSTR("ICP1 10mA with DIO4 sinking 17mA: %1.3f A\r\n"), icp3at17mA_icp3at10mA_dio2sink_i);
    if (icp3at17mA_icp3at10mA_dio2sink_i > 0.018) 
    { 
        passing = 0; 
        printf_P(PSTR(">>> DIO2 is not sinking.\r\n"));
    }
    
    // Disconnect message
    printf_P(PSTR("To disconnect battery turn off the PV supply and LED should stop blinking\r\n"));

    // charge control startup
    digitalWrite(CC_SHUTDOWN,LOW);
    
    // final test status
    if (passing)
    {
        printf_P(PSTR("[PASS]\r\n"));
    }
    else
    {
        printf_P(PSTR("[FAIL]\r\n"));
    }
    printf_P(PSTR("\r\n\r\n\r\n"));
    
    // ESD packaging note
    printf_P(PSTR("Only open the ESD shield bag in an ESD safe area.\r\n"));
    printf_P(PSTR("Remove the shipping box and this paper from the area \r\n"));
    printf_P(PSTR("before opening the ESD shield bag. \r\n"));
}


void blink(void)
{
    unsigned long kRuntime = millis() - blink_started_at;
    if ( kRuntime > blink_delay)
    {
        // turn off the current sources
        digitalWrite(CURR_SOUR_EN,LOW);
        
        if (passing)
        {
            pinMode(DIO13,OUTPUT);
            digitalWrite(DIO13,LOW);
            pinMode(DIO10,OUTPUT);
            digitalToggle(DIO10);
        }
        else
        {
            pinMode(DIO10,OUTPUT);
            digitalWrite(DIO10,LOW);
            pinMode(DIO13,OUTPUT);
            digitalToggle(DIO13);
         }

        // next toggle 
        blink_started_at += blink_delay; 
    }
}

int main(void)
{
    setup(); 
    
    passing = 1;
    test();
    
    while (1) 
    {
        blink();
        
        // PV input voltage
        float pv_v = analogRead(PV_V)*((ref_extern_avcc_uV/1.0E6)/1024.0)*(532.0/100.0);
        if (pv_v < 5.0) 
        { 
            digitalWrite(BAT_DISCONNECT,HIGH);
            while (1) {}; // loop until the power is gone
        }
    }    
}

