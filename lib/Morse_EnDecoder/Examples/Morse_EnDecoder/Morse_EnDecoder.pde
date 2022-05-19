/*
               MORSE ENDECODER

  Morse encoder / decoder classes for the Arduino.


  This example decodes Morse signals present on digital input 7.
  (active low, and then also using the internal pullup resistor)

  It also encodes Morse sent via the serial interface to the Arduino, on
  digital output pin 13.
  
  It can also decode audible signals, if using the constant MORSE_AUDIO
  instead of MORSE_KEYER, but then it is important to note that the
  input pin nr. will be for ANALOG inputs (0-5 on Atmega 168 - 328),
  and not the digital inputs.



  Copyright (C) 2010 raron
  
  GNU GPLv3 license:
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
   
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
   
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
   
  
  Contact: raronzen@gmail.com  (not checked too often..)
*/


#include <MorseEnDecoder.h>


int morseInPin = 7;      
int morseOutPin = 13;

morseDecoder morseInput(morseInPin, MORSE_KEYER, MORSE_ACTIVE_LOW);
morseEncoder morseOutput(morseOutPin);


void setup()
{
  Serial.begin(9600);
  Serial.println("Morse EnDecoder demo");
  
  // Setting Morse speed in wpm - words per minute
  // If not set, 13 wpm is default
  morseInput.setspeed(13);
  morseOutput.setspeed(13);
}



void loop()
{
  // Needs to call these once per loop
  morseInput.decode();
  morseOutput.encode();


  // Morse output:
  // Encoding text received from the serial input
  if (Serial.available() && morseOutput.available())
  {
    char sendMorse = Serial.read();
    morseOutput.write(sendMorse);
    
    // Also write it back to serial
    morseOutput.encode();    // This is just to get morseSignalString before it is destroyed
    Serial.write(' ');
    Serial.write(sendMorse);
    Serial.write(morseOutput.morseSignalString);
  }

  // Morse input:
  // If a character is decoded from the input, write it to serial output
  if (morseInput.available())
  {
    char receivedMorse = morseInput.read();
    Serial.print(receivedMorse);
    
    // A little error checking    
    if (receivedMorse == '#') Serial.println("< ERROR:too many morse signals! >");
  }


  // Morse feedback from input if not sending Morse.
  if (morseOutput.available()) digitalWrite(morseOutPin, morseInput.morseSignalState);
}

