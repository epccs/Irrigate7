/* PwrDown
  Author: rsutherland
  License: GNU General Public License version 2
  Part of Irrigate7 project http://epccs.org/indexes/Board/Irrigate7/ 
 
Power Down so I can check the current draw.
http://forum.arduino.cc/index.php?topic=53821.15
 */
 
#include <avr/power.h>
#include <avr/sleep.h>

void setup()
{               
  byte i;
 
  // Ensure no floating pins
  for(i=0; i<20 ; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
 
  // Power-down board
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
 
  sleep_enable();
 
  // Disable ADC
  ADCSRA &= ~(1 << ADEN);

  // Power down functions
  PRR = 0xFF;
 
  // Enter sleep mode
  sleep_mode();
}

void loop()
{
}