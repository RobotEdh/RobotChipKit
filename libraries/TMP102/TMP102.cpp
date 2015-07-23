/*
  TMP102.cpp - Library for commmunicating with temperature sensor TMP102
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/

#include <WProgram.h> // used for delay function (core lib)
#include <Wire.h>     // used for I2C protocol (lib)

#include <TMP102.h>

// Constructor
TMP102Class::TMP102Class ()
{
}
 
void TMP102Class::TMP102_init()
{
    Wire.begin(); // setup I2C

	return;
} 
 

double TMP102Class::TMP102_read ()
{   
    Wire.beginTransmission(TMP102_ADDRESS);  
	Wire.send((int)0);                            // use register 0 : temperature
	
	int ret = Wire.endTransmission();             // 0: success
                                                  // 1: length to long for buffer
                                                  // 2: address send, NACK received -> bad address
                                                  // 3: data send, NACK received -> bad register
                                                  // 4: other error (lost bus arbitration, bus error, ..) -> missing 1Ok pull-down resistor on SDA & SDL pins
	if (ret != 0) return (-1 * ret);
	
	delay(1);
	
	Wire.requestFrom(TMP102_ADDRESS, (int)2);     // request 2 bytes
	int msb = Wire.receive(); // receive high byte (full degrees)
	int lsb = Wire.receive(); // receive low byte (fraction degrees) 
    int Temperature = ((msb << 8) | lsb) >> 4; 
    double celsius = (double)Temperature*0.0625;
    return celsius;
}