/*
    PRIMARY TEST RIG for multiple Multiplexers and Responsive Analog Read

*/
#include "Arduino.h"
#include <ResponsiveAnalogRead.h>

// Methods
void selectMuxPin(byte pin);

// Consts
const byte MUX_PINS = 16; // number of MUX Channnels

// Mux Select Pins array
const byte selectPins[4] = {29, 30, 31, 32}; // S0~2, S1~3, S2~4, S3~5
const byte selectPinsSize = (sizeof(selectPins)/sizeof(byte));

// Mux Signal Pins arrray
// Start with this to prove it works, but we will add the pin to the pot array
const byte muxSignal[3] = {A0, A1, A2};
const byte muxSignalSize = (sizeof(muxSignal)/sizeof(byte));

// Vars
ResponsiveAnalogRead muxPots[muxSignalSize][MUX_PINS]; // empty MD array of muxPots - populated in setup
int muxData[muxSignalSize][MUX_PINS] = {}; // MD array for the RAR data


void setup()
{
  // Initialise the select pins, as outputs
  for (byte pin = 0; pin < selectPinsSize; pin++)
  {
    pinMode(selectPins[pin], OUTPUT);
    digitalWrite(selectPins[pin], LOW);
  }

  // Populate the ResponsiveAnalogRead array
  for (byte mux = 0; mux < muxSignalSize; mux++)
  {
    for (byte pin = 0; pin < MUX_PINS; pin++) 
    {
      muxPots[mux][pin] = ResponsiveAnalogRead(muxSignal[mux], true);  // Init RAR POT, sleep enabled
    }
  }

  // Initialise the Analog Input pins
  // accomodates weak pullup/down circuit on Teensy 4.x which interferes with RAR
  // needs to be done AFTER the RAR constructor (above)
  for (byte mux = 0; mux < muxSignalSize; mux++)
  {
    pinMode(muxSignal[mux], INPUT_DISABLE);
  }
}


void loop()
{
  // for each mux
  for (byte mux = 0; mux < muxSignalSize; mux++)
  {
    Serial.print("MUX");
    Serial.print(mux);
    Serial.print(" :");

    // for each pin on MUX - i.e. for each pot
    for (byte pin = 0; pin < MUX_PINS; pin++)
    {
      // set the mux Signal Pins
      selectMuxPin(pin);

      // print the value (Analog Read)
      //Serial.print(analogRead(muxSignal[mux]));
      
      // update the RAR object for the pin
      muxPots[mux][pin].update(); 

      // check if it's changed
      if(muxPots[mux][pin].hasChanged()) {

        // Analog Range
        muxData[mux][pin] = muxPots[mux][pin].getValue();

        // MIDI range
        //muxData[mux][pin] = muxPots[mux][pin].getValue() >> 3;
        
      }

      // Output to serial
      Serial.print(muxData[mux][pin]);
    

      if (pin==15){Serial.println("");}
      else {Serial.print(",");}
      
      delay(50);
    }

    if (mux==2){Serial.println("");}
  }
}





// The selectMuxPin function sets the S0, S1, S2 and S3 pins
void selectMuxPin(byte pin)
{
  for (byte i = 0; i < selectPinsSize; i++)
  {
    if (pin & (1 << i)) {
      digitalWrite(selectPins[i], HIGH);
    }
    else {
      digitalWrite(selectPins[i], LOW);
    }
  }
  delay(5); // added this due to speed of teensy
}
