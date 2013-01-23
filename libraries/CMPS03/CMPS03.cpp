/*
  CMPS03.cpp - Library for commmunicating with compass CMPS03
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/

#include <WProgram.h> // used for delay function (core lib)
#include <Wire.h>     // used for I2C protocol (lib)

#include <CMPS03.h>

// Constructor
CMPS03Class::CMPS03Class ()
{
}
 
void CMPS03Class::CMPS03_begin ()
{
	// setup I2C
	
    Wire.begin(); 

	return;
} 
 
    
int CMPS03Class::CMPS03_revision ()
{
	// return Software Revision Number
	
    Wire.beginTransmission(CMPS03_ADDRESS); 
	Wire.send((int)0);                            // use register 0
	
	int ret = Wire.endTransmission();             // 0: success
                                                  // 1: length to long for buffer
                                                  // 2: address send, NACK received -> bad address
                                                  // 3: data send, NACK received -> bad register
                                                  // 4: other error (lost bus arbitration, bus error, ..) -> missing 1Ok pull-down resistor on SDA & SDL pins
	if (ret != 0) return (-1 * ret);
	    
	delay(1);
	Wire.requestFrom(CMPS03_ADDRESS, (int)1);     // request 1 byte
	int value = Wire.receive();
	return value;
}

int CMPS03Class::CMPS03_read ()
{
	// returns 0-254 for a full circle, 0: North
    
    Wire.beginTransmission(CMPS03_ADDRESS);  
	Wire.send((int)1);                            // use register 1
	
	int ret = Wire.endTransmission();             // 0: success
                                                  // 1: length to long for buffer
                                                  // 2: address send, NACK received -> bad address
                                                  // 3: data send, NACK received -> bad register
                                                  // 4: other error (lost bus arbitration, bus error, ..) -> missing 1Ok pull-down resistor on SDA & SDL pins
	if (ret != 0) return (-1 * ret);
	
	delay(1);
	Wire.requestFrom(CMPS03_ADDRESS, (int)1);     // request 1 byte
	int value = Wire.receive();
	return value;
}