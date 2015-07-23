/*
  TEMT6000.cpp - Library for communicating with Ambient Light Sensor TEMT6000
  Created by EDH, December 30, 2014.
  Released into the public domain.
*/

#include <TEMT6000.h>

// Constructor
TEMT6000Class::TEMT6000Class ()
{
}
 
void TEMT6000Class::TEMT6000_init ()
{

 TEMT6000_init(TEMT6000_PIN_DEFAULT); 
}
 
void TEMT6000Class::TEMT6000_init (int pin)
{

 this->_TEMT6000_Pin = pin;
 pinMode(this->_TEMT6000_Pin, INPUT);   // define pin as input
 
 return;
}
  
int TEMT6000Class::TEMT6000_getLight()
{
 if (!this->_TEMT6000_Pin) return -1;            // pin is not initialized
 return (analogRead(this->_TEMT6000_Pin));       // read analog input pin 
}