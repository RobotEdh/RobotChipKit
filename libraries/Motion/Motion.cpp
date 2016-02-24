/*
  Motion.cpp - Library for communicating with infrared motion sensor 
  Created by EDH, December 30, 2014.
  Released into the public domain.
*/

#include <Motion.h>

// Constructor
MotionClass::MotionClass ()
{
}

void MotionClass::Motion_init ()
{

 Motion_init(MOTION_PIN_DEFAULT); 
}

void MotionClass::Motion_init (int pin)
{

 this->_Motion_Pin = pin;
 pinMode(this->_Motion_Pin, INPUT);   // define pin as input

 delay (2000);                       // wait 1-2 seconds for the sensor to get a snapshot of the still room
 
 return;
}
  
int MotionClass::Motion_status()
{
  if (!this->_Motion_Pin) return -1;            // pin is not initialized
  return (digitalRead(this->_Motion_Pin));      // read digital input pin 
}