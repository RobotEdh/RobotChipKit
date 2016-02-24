/*
  Serial2IOT.cpp - Library for commmunicating with serial IOT Photon Particle IO
  Created by EDH, Feb 20, 2016.
  Released into the public domain.
*/

#include <WProgram.h> // used for delay function (core lib)
#include <Serial2IOT.h>


//Commands for the IOT
const uint8_t STX = 0x02;
const uint8_t ETX = 0x03;
const uint8_t HELLO[5] =        {'H', 'E', 'L', 'L', 'O'};
const uint8_t HELLO_OK[5] =     {'H', 'E', 'L', 'L', 'O'};
      
// Constructor
IOTClass::IOTClass()
{
}

//Initialize the serial2 port
void IOTClass::begin(void)
{
	//IOT baud rate is 38400
	Serial2.begin(38400);
}


//Compare 2 uint8_t arrays
int IOTClass::uint8Compare(const uint8_t *a1, const uint8_t *a2, int len) 
{
  for(int i=0; i<len; i++)
      if(a1[i]!=a2[i])
          return -1;

  return 0;
}

//Send a basic command to the camera and get the response 
int IOTClass::sendCommand(const uint8_t * command, uint8_t* response, int wlen, int rlen)
{
	int ibuf=-1;

	while(Serial2.available() > 0) {
	    ibuf = Serial2.read();
    } // waiting for data in the serial buffer
 
	//Send each character in the command string to the IOT through the serial port
	Serial2.write(STX);
	for(int i=0; i<wlen; i++){
		Serial2.write(*command++);
	}
    Serial2.write(ETX);
    
	//Get the response from the IOT and add it to the response string
	for(int i=0; i<rlen; i++)
	{
		while(Serial2.available() == 0); // waiting for data in the serial buffer
		ibuf = Serial2.read();
		if (ibuf == -1) return -10; // serial buffer empty, should not happen as we wait before
		response[i] = (uint8_t)ibuf;
	}
 
	return 0;
}


//Ping Hello
int IOTClass::ping()
{
    int ret=0;
    uint8_t response[5];
		
    ret = sendCommand(HELLO, response, 5, 5);
    if (ret != 0) return -10;
    
    if (uint8Compare(response,HELLO_OK,5) != 0) return -2;
	    
    return 0;      
}

