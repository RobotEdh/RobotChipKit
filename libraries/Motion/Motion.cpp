/*
  Motion.cpp - Library for communicating with infrared motion sensor 
  Created by EDH, December 30, 2014.
  Released into the public domain.
*/

#include <Motion.h>

void Motion_init(int pin)
{
 pinMode(pin, INPUT);   // define pin as input
 delay (2000);          // wait 1-2 seconds for the sensor to get a snapshot of the still room
 
 return; 
}
  
int Motion_status(int pin)
{

  return (digitalRead(pin));        // read digital input pin 
}