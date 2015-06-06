#include <ctype.h>

/* ADCTest 
  Author: rsutherland
  License: GNU General Public License version 2
  Part of Irrigate7 project http://epccs.org/indexes/Board/Irrigate7/ 
 
  Used to verify setup of analog pin mapping for Irrigate7
  https://github.com/epccs/epccs-avr/blob/master/variants/irrigate7/pins_arduino.h
 
  Irrigate7
      ADC0 wired to Arduino pin compatible header and is floating
      ADC1 wired to Arduino pin compatible header and is floating
      ADC2 wired to Arduino pin compatible header and is floating
      ADC3 wired to plugable header with 1kOhm to AGND
      ADC4 wired to plugable header with 1kOhm to AGND
      ADC5 wired to voltage divider (5.62 / 105.62) from BOOST
      ADC6 wired to voltage divider (100 / 532) from PV or the solar input voltage
      ADC7 wired to voltage divider (2 / 3) from PWR or the battery voltage
 
  The ADCs are printed to the serial port with 1s delay between each set of readings. 
  The analogue reference can be changed during operation by sending one of the
  following characters:
      1: Set analog reference to INTERNAL1V1");
      2: Set analog reference to INTERNAL2V56");
      d: Set analog reference to DEFAULT (Vcc)");
      e: Set analog reference to EXTERNAL");
  Sending h (or ?) prints help information about the characters which
  are accepted.
 
  Warning: Do not make assumptions about the use of A0, e.g. 
  I expect analogRead(A1) or analogRead(A0+1) or analogRead(1) should all read the same channel
  but that is often not the case so verify they are working do not just assueme so.
 */

char *str = "DEFAULT (Vcc)";
void setup(void)
{
  Serial.begin(9600);
  analogReference(DEFAULT);
}


void loop(void)
{
  if (Serial.available()) {
    char c = Serial.read();
    switch (tolower(c)) {
    case '1':
      analogReference(INTERNAL1V1);
      str = "INTERNAL1V1";
      break;
    case '2':
      analogReference(INTERNAL2V56);
      str = "INTERNAL2V56";
      break;
    case 'd':
      analogReference(DEFAULT);
      str = "DEFAULT (Vcc)";
      break;
    case 'e':
      analogReference(EXTERNAL);
      str = "EXTERNAL";
      break;
    case 'h':
    case '?':
      Serial.println("Help: ");
      Serial.println("  1: Set analog reference to INTERNAL1V1");
      Serial.println("  2: Set analog reference to INTERNAL2V56");
      Serial.println("  d: Set analog reference to DEFAULT (Vcc)");
      Serial.println("  e: Set analog reference to EXTERNAL");
      break;
      
    };
    
  }
  delay(100);
  Serial.print("Analog reference: ");
  Serial.println(str);
  for (int i = 0; i < 8; ++i) {
    // A0 is defined in irrigate7 variants/pins_arduino.h file with its digital pin number
    // digitalPinToAnalogPin() converts this to the analog pin number, but is not used by 
    // anything in the arduino core at this time. The arduino core does use analogPinToChannel()
    // so it is important that it will cause analogRead() to have the expected behavior.
    // In this for loop "i" is an analog pin number.
    if (i == 0) {
      Serial.print("A0 ");
      Serial.print("\t (D2Ap[A0] ");
      Serial.print(digitalPinToAnalogPin(A0));
      Serial.print(") ");
      Serial.print("\t (Ap2ch[A0] ");
      Serial.print(analogPinToChannel(A0));
      Serial.print(") ");
    }
    if (i == 1) {
      Serial.print("A1 ");
      Serial.print("\t (D2Ap[A1] ");
      Serial.print(digitalPinToAnalogPin(A1));
      Serial.print(") ");
      Serial.print("\t (Ap2ch[A1] ");
      Serial.print(analogPinToChannel(A1));
      Serial.print(") ");
    }
    if (i == 2) {
      Serial.print("A2 ");
      Serial.print("\t (D2Ap[A2] ");
      Serial.print(digitalPinToAnalogPin(A2));
      Serial.print(") ");
      Serial.print("\t (Ap2ch[A2] ");
      Serial.print(analogPinToChannel(A2));
      Serial.print(") ");
    }
    if (i == 3) {
      Serial.print("A3 ");
      Serial.print("\t (D2Ap[A3] ");
      Serial.print(digitalPinToAnalogPin(A3));
      Serial.print(") ");
      Serial.print("\t (Ap2ch[A3] ");
      Serial.print(analogPinToChannel(A3));
      Serial.print(") ");
    }
    if (i == 4) {
      Serial.print("A4 ");
      Serial.print("\t (D2Ap[A4] ");
      Serial.print(digitalPinToAnalogPin(A4));
      Serial.print(") ");
      Serial.print("\t (Ap2ch[A4] ");
      Serial.print(analogPinToChannel(A4));
      Serial.print(") ");
    }
    if (i == 5) {
      Serial.print("A5 ");
      Serial.print("\t (D2Ap[A5] ");
      Serial.print(digitalPinToAnalogPin(A5));
      Serial.print(") ");
      Serial.print("\t (Ap2ch[A5] ");
      Serial.print(analogPinToChannel(A5));
      Serial.print(") ");
    }
    if (i == 6) {
      Serial.print("A6 ");
      Serial.print("\t (D2Ap[A6] ");
      Serial.print(digitalPinToAnalogPin(A6));
      Serial.print(") ");
      Serial.print("\t (Ap2ch[A6] ");
      Serial.print(analogPinToChannel(A6));
      Serial.print(") ");
    }
    if (i == 7) {
      Serial.print("A7 ");
      Serial.print("\t (D2Ap[A7] ");
      Serial.print(digitalPinToAnalogPin(A7));
      Serial.print(") ");
      Serial.print("\t (Ap2ch[A7] ");
      Serial.print(analogPinToChannel(A7));
      Serial.print(") ");
    }
    Serial.print("\t (Ach2D ");
    Serial.print(analogInputToDigitalPin(i));
    Serial.print(") ");
    Serial.print("\t (Ach[");
    Serial.print(i);
    Serial.print("] ");
    Serial.print(analogPinToChannel(i));
    Serial.print(") ");
    // analogRead uses analogPinToChannel(p) function if available from the variants pins_arduino.h file 
    // https://github.com/arduino/Arduino/blob/master/hardware/arduino/avr/cores/arduino/wiring_analog.c
    Serial.print("\t (int ");
    int sensorValue = analogRead(i);
    Serial.print(sensorValue);
    Serial.print (") ");
    Serial.print("\t (float ");
    float voltage = sensorValue * (5.0 / 1023.0);
    if (i == 5) {
      voltage = voltage * (105.62 / 5.62);
    }
    if (i == 6) {
      voltage = voltage * (532.0 / 100.0);
    }
    if (i == 7) {
      voltage = voltage * (3.0 / 2.0);
    }
    Serial.print (voltage);
    Serial.println(") ");
  }
  delay(10000);
}
