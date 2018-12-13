/*
  TMP102.cpp - Library for commmunicating with temperature sensor TMP102
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/

#include <Arduino.h> // used for delay function (core lib)
#include <Wire.h>     // used for I2C protocol (lib)

#include <TMP102.h>

// Constructor
TMP102Class::TMP102Class ()
{
}
 
void TMP102Class::TMP102_init()
{
    Wire.begin(); // setup I2C
} 
 

double TMP102Class::TMP102_read ()
{   
    uint8_t ret = 0;
    
    Wire.beginTransmission(TMP102_ADDRESS);  
	Wire.write((uint8_t)0);                       // use register 0 : temperature
	
	ret = Wire.endTransmission(); 
	
	delay(1);
	
	ret = Wire.requestFrom(TMP102_ADDRESS, (int)2);     // request 2 bytes
	uint8_t msb = Wire.read(); // receive high byte (full degrees)
	uint8_t lsb = Wire.read(); // receive low byte (fraction degrees) 
    int Temperature = ((msb << 8) | lsb) >> 4; 
    double celsius = (double)Temperature*0.0625;
    return celsius;
}