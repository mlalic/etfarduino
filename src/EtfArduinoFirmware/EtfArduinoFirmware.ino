// EtfArduinoFirmware.ino
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

union IntWrapper {
  unsigned int value;
  byte buffer[2];
};
union LongWrapper {
  unsigned long value;
  byte buffer[4];
};
// Global initialization
// Wraps the analog value acquired in a union which
// allows each byte to be easily accessed so it can be
// sent over the serial interface.
volatile IntWrapper analogSample;
// Analog acquisition active
volatile boolean start = false;
// Should a value be sent on the serial port
volatile boolean toSend = false;
// Period of sampling the analog input
LongWrapper period;
// A bitmask used for changing the input channel
char channelMask = 0x0;
// The digital output pin
int const digitalOutPin = 13;
// PWM output
int const analogOutPin = 3;

void setup() {      
  analogSample.value = 0;
  start = false;
  period.value = 1000;    // microseconds
  
  // --------  
  // ADC
  // --------  
  // Setup registers
  // Voltage reference AVcc
  ADMUX = 0;
  ADMUX |= (1 << REFS0);
  // Enable the ADC
  ADCSRA |= (1 << ADEN);
  // Set the prescale factor to 16 (100 == ADPS bits)
  ADCSRA |= (1 << ADPS2);
  ADCSRA &= ~(1 << ADPS1);
  ADCSRA &= ~(1 << ADPS0);

  // ------------
  // Timer
  // ------------
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = (period.value >> 2) - 1;
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS11) | (1 << CS10);    // 64 prescaler (str. 135)
  TIMSK1 &= ~(1 << OCIE1A);  // disable timer interrupt
  sei();
  
  pinMode(digitalOutPin, OUTPUT);
  pinMode(analogOutPin, OUTPUT);
  Serial.begin(256000);
}
// the loop routine runs over and over again forever
void loop() {
  if (toSend) {
    toSend = false;
    Serial.write(analogSample.buffer[0]);
    Serial.write(analogSample.buffer[1]);
  }
  // peek() je mnogo brza funkcija od available!
  if (Serial.peek() != -1) {
    char code = Serial.read();
    switch (code) {
      case '1': {
        TCNT1 = 0;
        ADMUX &= ~(1 << MUX0);    // set the first channel to A0
        TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt        
        return;
      }
      case '2': {
        TIMSK1 &= ~(1 << OCIE1A);  // disable timer compare interrupt
        return;
      }
      case '3': {
        if (TIMSK1 & (1 << OCIE1A)) {
          // Acquisition active... problem!
        } else {
          while (Serial.available() < 4);
          // The driver makes sure to send a period which is
          // a multiple of 4!
          period.buffer[0] = Serial.read();
          period.buffer[1] = Serial.read();
          period.buffer[2] = Serial.read();
          period.buffer[3] = Serial.read();
          // period >> 2 == period / 4, only faster
          OCR1A = (period.value >> 2) - 1;
        }
        return;
      }
      case '4': {
        // GET_SINGLE_VALUE
        while (Serial.available() < 1) ;
        channelMask = Serial.read();
        digitalWrite(13, channelMask);
        ADMUX &= ~(1 << MUX0);
        ADMUX |= (channelMask << MUX0);
        // Do a conversion
        ADCSRA |= (1 << ADSC);    // Start conversion
        // This blocks for 13us = > 208 cycles wasted
        while (bit_is_set(ADCSRA, ADSC)) ;
        analogSample.buffer[0] = ADCL;
        analogSample.buffer[1] = ADCH;
        // --
        Serial.write(analogSample.buffer[0]);
        Serial.write(analogSample.buffer[1]);
        return;
      }
      case '5': {
        // the read method does not block, so have to manually wait
        // for all the parameters to arrive
        while (Serial.available() < 2) ;
        // Ignoring the line parameter for now
        Serial.read();
        unsigned char const val = Serial.read();
        // Send the second parameter directly to the digitalWrite function
        digitalWrite(digitalOutPin, val);
        return;
      }
      case '6': {
        // the read method does not block, so have to manually wait
        // for all the parameters to arrive
        while (Serial.available() < 2) ;
        // Ignoring the channel parameter for now
        Serial.read();
        unsigned char const val = Serial.read();
        // Send the second parameter directly to the digitalWrite function
        analogWrite(analogOutPin, val);
        return;
      }
      case '7': {
        // the read method does not block, so have to manually wait
        // for all the parameters to arrive
        while (Serial.available() < 3) ;
        unsigned char const channels = Serial.read();
        // This works only for the two channel case, if we were to add
        // even more input channels, further thought into the bitmask
        // would have to be given.
        channelMask = channels - 1;
        // Ignore the channel ordering for now.
        Serial.read(); Serial.read();
      }
    }
  }
}

ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
    // Do a conversion
    ADCSRA |= (1 << ADSC);    // Start conversion
    // This blocks for 13us
    while (bit_is_set(ADCSRA, ADSC)) ;
    analogSample.buffer[0] = ADCL;
    analogSample.buffer[1] = ADCH;
    // Change the input channel for the next cycle
    // channelMask is 1 for the two channel case so the
    // input is toggled between ADC0 and ADC1.
    // channelMask is 0 for the one channel case so the
    // input is constant at ADC0.
    ADMUX ^= (channelMask << MUX0);
    toSend = true;
}
