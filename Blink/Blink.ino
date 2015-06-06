/*
 * Blink is a very simple sketch and is used to check the toolchain.
 *
 *  Author: rsutherland
 *  License: GNU General Public License version 2
 * 
 * B7 is mapped to IO13 on Irrigate7. e.g. port B bit 7 which is on pin3 of the ATMEGA1284P-AU 
 * Set pin 13 as output and connected an LED and resistor to the output to blink it.
 * Irrigate7 does not have an onboard LED it would waste battery power
 */ 

void setup() {
  pinMode(13, OUTPUT); 

}

void loop() {
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000); 
}
