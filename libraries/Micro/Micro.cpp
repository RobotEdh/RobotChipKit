/*
  Micro.cpp - Library for communicating with electret Micro
  Created by EDH, July 24, 2015.
  Released into the public domain.
*/

#include <Micro.h>

// Constructor
MicroClass::MicroClass ()
{
}
 
void MicroClass::Micro_init ()
{

 Micro_init(MICRO_PIN_DEFAULT); 
}
 
void MicroClass::Micro_init (int pin)
{

 this->_Micro_Pin = pin;
 pinMode(this->_Micro_Pin, INPUT);   // define pin as input
 
 return;
}
  
int MicroClass::Micro_getNoise()
{
 if (!this->_Micro_Pin) return -1;            // pin is not initialized
 return (analogRead(this->_Micro_Pin));       // read analog input pin 
}