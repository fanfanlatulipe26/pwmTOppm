
//  pwmTOppm converter
//  version 1.0
//  Yet an other ..... Many thanks to all the implementation of these kind of converter available on the net.
//  Generation of a PPM signal for Flysky receivers that don't have PPM output.
//  OK for 8 channels Flysky FS-R9B receiver (same as Turnigy 9X8C),
//      or 6 channels  FS-R6, or 3 channels  FS-GR3  ..
//
//  version for Arduino Promini 5v 16Mhz  Atmega328 or Atmega128
// 
//  Auto adaptation to the number of channels. No changes in the code needed

// Receiver FS-R9B 8 channels
// - order of PWM pulses in the time:  ch1, ch3, ch2, ch4, ch5, ch6, ch7, ch8
// Receiver FS_R6 6 channels
// - order of PWM pulses in the time: ch1, ch2, ch3, ch4, ch5, CH6
// Receiver FS-GR3 3 channels:
//  - order of PWM pulses in the time:  ch2, ch1, ch3

// The rising edge of the PWM pulse of a channel generates the rising edge of PPM pulse.
// The order of the PPM pulses will not be the order of the port, but:
//  - for 8 channels:  ch1, ch3, ch2, ch4, ch5, ch6, ch7, ch8
//  - for 8 channels:  ch1, ch2, ch3, ch4, ch5, ch6
//  - for 8 channels:  ch2, ch1, ch3
// On the receiver itself (Turnigy 9X8C / Flysky FS-R9B) there is a delay of 2.25 microsec between the
//    falling edge of a PWM channel and the rising edge of the next channel and so the PPM signal for each channel
//    may be a bit too long, exepted for the last channel( but arround 1% negligible ....)
// The delay between rising edge of PWM and rising edge of PPM is arround 0.5 microsec

// In a try to compensate the 2.25microsec increase, we delay the generation of the PPM pulse
//    by  2.25 *(nbr of channel - rank of the channel in ppm)+1.
//    Better results:   ... but it does not matter .....

// Wiring:
//  channel 1 pin D2 port D  bit PD2
//  channel 2 pin D3 port D  bit PD3
//  channel 3 pin D4 port D  bit PD4
//  channel 4 pin D5 port D  bit PD5  unconnected if 3 channels receiver
//  channel 5 pin D6 port D  bit PD6  unconnected if 3 channels receiver
//  channel 6 pin D7 port D  bit PD7  unconnected if 3 channels receiver
//  channel 7 pin D8 port B  bit PB0  unconnected if 3 or 6 channels receiver
//  channel 8 pin D9 port B  bit PB1  unconnected if 3 or 6 channels receiver

//  PPM output pin D12 port B bit PB4


#define nbrMaxChannels 8
#define ppmPulseLength 200   //  length of the PPM pulses in micro seconds
#define pinPPM 12
#define delta 2.25
int  pinChannel[nbrMaxChannels] = {2, 3, 4, 5, 6, 7, 8, 9}; // input pin of each PWM signal from the receiver
int nextChannel = 1;
int nextPPM = 1; // rank of the next PPM pulse in the PPM burst
unsigned int nextDelay ;
int nbrOfChannels ;  // nbr of channels in the receiver

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("Init ...");
  pinMode(pinPPM, OUTPUT);
  digitalWrite(pinPPM, LOW);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  for (int i = 0; i < nbrMaxChannels; i++) {
    pinMode(pinChannel[i], INPUT_PULLUP);
  }
  // wait for some activities on channel 1 pin D2 port D  bit PD2
  //  (in fact the transmitter should be ON before the receiver, an so no real wait ....)
  for (int i = 0; i < 4; i++) {
    while (!(PIND & (1 << PD2)));    //While  input is low.
    while ((PIND & (1 << PD2)));    //While  input is high.
  }
  if (pulseIn(pinChannel[4], HIGH, 100 * 1000UL) == 0)  // timeout 100ms
    nbrOfChannels = 3;
  else if (pulseIn(pinChannel[7], HIGH, 100 * 1000UL) == 0)
    nbrOfChannels = 6;
  else
    nbrOfChannels = 8;
    
  Serial.print("Number of channels: "); Serial.println(nbrOfChannels);
  nextDelay = (delta * (nbrOfChannels - nextPPM));
  Serial.println("Running ...");
}

void loop() {
  switch (nextChannel) {
    case 1:
      //  channel 1 pin D2 port D  bit PD2
      while (!(PIND & (1 << PD2)));    //While  input is low.
      delayMicroseconds(nextDelay);
      PORTB |= (1 << PB4);          // set PPM output high pin D12 port B bit PB4
      delayMicroseconds(ppmPulseLength);
      PORTB &= ~(1 << PB4);         //Set PPM output low.
      if (nbrOfChannels == 6)
        nextChannel = 2;
      else nextChannel = 3;
      nextPPM++;
      // we have time to compute ...
      nextDelay = (delta * (nbrOfChannels - nextPPM));
      break;
    case 2:
      //  channel 2 pin D3 port D  bit PD3
      while (!(PIND & (1 << PD3)));  //While  input D11 is low.
      delayMicroseconds(nextDelay);
      PORTB |= (1 << PB4);          // set PPM output high pin D12 port B bit PB4
      delayMicroseconds(ppmPulseLength);
      PORTB &= ~(1 << PB4);         //Set PPM output low.
      if (nbrOfChannels == 6)
        nextChannel = 3;
      else if (nbrOfChannels == 3)
        nextChannel = 1;
      else
        nextChannel = 4;
      nextPPM++;
      nextDelay = (delta * (nbrOfChannels - nextPPM));
      break;
    case 3:
      //  channel 3 pin D4 port D  bit PD4
      while (!(PIND & (1 << PD4)));  //While  input D10 is low.
      delayMicroseconds(nextDelay);
      PORTB |= (1 << PB4);          // set PPM output high pin D12 port B bit PB4
      delayMicroseconds(ppmPulseLength);
      PORTB &= ~(1 << PB4);         //Set PPM output low.
      // generate now the stop pulse in case we have a 3 channels receiver
      if (nbrOfChannels == 3) {
        // generation of the PPM pulse for the end of PWM pulse on previous channel
        // we generate the stop pulse when channel 3 goes low
        //  we will be a bit "short" for the last channel ppm .... (2.5 micros ...)
        while ((PIND & (1 << PD4)));  // while channel is high.
        delayMicroseconds(2);  // small compensation ++++++++++++++++++++++++++++++++ 
        PORTB |= (1 << PB4);          // set PPM output high pin D12 port B bit PB4
        delayMicroseconds(ppmPulseLength);
        PORTB &= ~(1 << PB4);         //Set PPM output low.
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        nextChannel = 2;  // restart the sequence on channel 2 for 3 channels receiver
        nextPPM = 1;
        nextDelay = (delta * (nbrOfChannels - nextPPM));
        break;
      }
      if (nbrOfChannels == 8)
        nextChannel = 2;
      else
        nextChannel = 4;
      nextPPM++;
      nextDelay = (delta * (nbrOfChannels - nextPPM));
      break;
    case 4:
      //  channel 4 pin D5 port D  bit PD5
      while (!(PIND & (1 << PD5)));
      delayMicroseconds(nextDelay);
      PORTB |= (1 << PB4);          // set PPM output high pin D12 port B bit PB4
      delayMicroseconds(ppmPulseLength);
      PORTB &= ~(1 << PB4);         //Set PPM output low.
      nextChannel = 5;
      nextPPM++;
      nextDelay = (delta * (nbrOfChannels - nextPPM));
      break;
    case 5:
      //  channel 5 pin D6 port D  bit PD6
      while (!(PIND & (1 << PD6)));
      delayMicroseconds(nextDelay);
      PORTB |= (1 << PB4);          // set PPM output high pin D12 port B bit PB4
      delayMicroseconds(ppmPulseLength);
      PORTB &= ~(1 << PB4);         //Set PPM output low.
      nextChannel = 6;
      nextPPM++;
      nextDelay = (delta * (nbrOfChannels - nextPPM));
      break;
    case 6:
      //  channel 6 pin D7 port D  bit PD7
      while (!(PIND & (1 << PD7)));
      delayMicroseconds(nextDelay);
      PORTB |= (1 << PB4);          // set PPM output high pin D12 port B bit PB4
      delayMicroseconds(ppmPulseLength);
      PORTB &= ~(1 << PB4);         //Set PPM output low.
      // generate now the stop pulse in case we have a 6 channels receiver

      if (nbrOfChannels == 6) {
        // generation of the PPM pulse for the end of PWM pulse on previous channel
        // we generate the stop pulse when channel 6 goes low
        //  we will be a bit "short" for the last channel ppm .... (2.5 micros ...)
        while ((PIND & (1 << PD7)));  // channel 6 is high.
        delayMicroseconds(2);  // small compensation ++++++++++++++++++++++++++
        PORTB |= (1 << PB4);          // set PPM output high pin D12 port B bit PB4
        delayMicroseconds(ppmPulseLength);
        PORTB &= ~(1 << PB4);         //Set PPM output low.
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        nextChannel = 1;  // restart the sequence on channel 1 for 6 channels receiver
        nextPPM = 1;
        nextDelay = (delta * (nbrOfChannels - nextPPM));
        break;
      }
      nextChannel = 7;
      nextPPM++;
      nextDelay = (delta * (nbrOfChannels - nextPPM));
      break;
    case 7:
      //  channel 7 pin D8 port B  bit PB0
      while (!(PINB & (1 << PB0)));
      delayMicroseconds(nextDelay);
      PORTB |= (1 << PB4);          // set PPM output high pin D12 port B bit PB4
      delayMicroseconds(ppmPulseLength);
      PORTB &= ~(1 << PB4);         //Set PPM output low.
      nextChannel = 8;
      nextPPM++;
      nextDelay = (delta * (nbrOfChannels - nextPPM));
      break;
    case 8:
      //  channel 8 pin D9 port B  bit PB1
      while (!(PINB & (1 << PB1)));
      delayMicroseconds(nextDelay);
      PORTB |= (1 << PB4);          // set PPM output high pin D12 port B bit PB4
      delayMicroseconds(ppmPulseLength);
      PORTB &= ~(1 << PB4);         //Set PPM output low.
      //  last channel. generate the stop pulse when channel goes low
      //  we will be a bit "short" for the last channel ppm .... (2.5 micros ...)
      while ((PINB & (1 << PB1)));  // channel 8 is high.
      delayMicroseconds(2);  // small compensation ++++++++++++++++++++++++++
      PORTB |= (1 << PB4);          // set PPM output high pin D12 port B bit PB4
      delayMicroseconds(ppmPulseLength);
      PORTB &= ~(1 << PB4);         //Set PPM output low.
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      nextChannel = 1;  // restart the sequence on channel 1 for 8 channels receiver
      nextPPM = 1;
      nextDelay = (delta * (nbrOfChannels - nextPPM));
      break;
    default:
      Serial.print("Error in switch. nextChannel: "); Serial.println(nextChannel);
  }  // switch
}
