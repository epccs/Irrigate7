/*
    Copyright (C) 2014  Ronald Sutherland

    IntDrivenADC is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    IntDrivenADC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with IntDrivenADC.  If not, see <http://www.gnu.org/licenses/>.

    interrupt driven analog conversion
*/

#define ADC_CHANNELS 4
volatile uint16_t adc[ADC_CHANNELS];
volatile uint8_t adc_channel;

// Interrupt service routine for when the ADC completes
ISR(ADC_vect){
 
  
  // ADCL contain lower 8 bits, ADCH upper (two bits)
  // Must read ADCL first
  adc[adc_channel] = ADCL | (ADCH << 8);
  
  ++adc_channel;
  if (adc_channel >= ADC_CHANNELS) {
    adc_channel = 0;
  }
  ADMUX &= ~(1<<MUX3) & ~(1<<MUX2) & ~(1<<MUX1) & ~(1<<MUX0);
  ADMUX = (((ADMUX & ~(1<<REFS1)) | (1<<REFS0)) & ~(1<<ADLAR)) + adc_channel;
 
  // set ADSC in ADCSRA, ADC Start Conversion
  ADCSRA |= (1<<ADSC);
}

void initAnalog(void) {
  adc_channel = 0;
  for(uint8_t i=0; i<ADC_CHANNELS; i++) {
    adc[i] = 0;
  }

  // See ADMUX Register Description 24.9 in ATMega328 datasheet
  // MUX[3:0] default to zero which sellects ADC0
  // ADC_VREF_AREF ((ADMUX & ~(1<<REFS1)) & ~(1<<REFS0)) & ~(1<<ADLAR)
  // ADC_VREF_VCC ((ADMUX & ~(1<<REFS1)) | (1<<REFS0)) & ~(1<<ADLAR)
  // ADC_VREF_BG ((ADMUX | (1<<REFS1)) & ~(1<<REFS0)) & ~(1<<ADLAR)
  // ADC_VREF_INT ((ADMUX | (1<<REFS1)) | (1<<REFS0)) & ~(1<<ADLAR)
  ADMUX = ((ADMUX & ~(1<<REFS1)) | (1<<REFS0)) & ~(1<<ADLAR);
 
  // in ADCSRA:
  // set ADEN, Enable ADC
  // clear ADATE, not Auto Trigger Enable
  // set ADIE, Interrupt Enable
  // set ADPS[2:0], Prescaler division factor (128, thus 16M/128 = 125kHz ADC clock)
  // Note, the first instruction takes 25 ADC clocks to execute, next takes 13 clocks
  ADCSRA = (ADCSRA | (1<<ADEN)) & ~(1<<ADATE) | (1 << ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
 
  // Enable global interrupts
  sei();
 
  // set ADSC in ADCSRA, ADC Start Conversion
  ADCSRA |= (1<<ADSC);
}

void setup() {
  Serial.begin(9600);
  initAnalog();
}

void loop () {
  static uint8_t i = 0;
  if (i<10) {
    Serial.print ("ADMUX :" + String(ADMUX,BIN));
    Serial.print ("  adc_channel :" + String(adc_channel, BIN) );
    Serial.print ("  Ch 0 :" + String(adc[0],DEC));
    Serial.print ("  Ch 1 :" + String(adc[1],DEC));
    Serial.print ("  Ch 2 :" + String(adc[2],DEC));
    Serial.print ("  Ch 3 :" + String(adc[3],DEC) + "\n");
    delay(1000);
    ++i;
  }
}


