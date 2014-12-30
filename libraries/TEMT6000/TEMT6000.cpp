/*
  TEMT6000.cpp - Library for communicating with Ambient Light Sensor TEMT6000
  Created by EDH, December 30, 2014.
  Released into the public domain.
*/

#include <TEMT6000.h>


void TEMT6000_init(int pin)
{
 pinMode(pin, INPUT);   // define pin as input
 
 return; 
}
  
int TEMT6000_getLight(int pin)
{

 return (analogRead(pin));       // read analog input pin 


}