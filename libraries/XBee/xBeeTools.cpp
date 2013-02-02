//************************************************************************
//*	Print.cpp - Tools class tfor Xbee
//*	Copyright (c) 2013 EDH.  All right reserved.
//*	
//*	This library is free software; you can redistribute it and/or
//*	modify it under the terms of the GNU Lesser General Public
//*	License as published by the Free Software Foundation; either
//*	version 2.1 of the License, or (at your option) any later version.
//*	
//*	This library is distributed in the hope that it will be useful,
//*	but WITHOUT ANY WARRANTY; without even the implied warranty of
//*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//*	Lesser General Public License for more details.
//*	
//*	You should have received a copy of the GNU Lesser General Public
//*	License along with this library; if not, write to the Free Software
//*	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//*	
//************************************************************************


#include <stdio.h>
#include <string.h>
#include <math.h>
#include "wiring.h"

#include "xBeeTools.h"
#include <XBee.h>


int xBeeTools::xBTprint(const char *str, int size)
{
unsigned int idx = 0;
int ret = SUCCESS;

	for (unsigned int i = 0; i < size; i++)
	{
		s_buffer[idx++] = str[i];
	}
	
	ret = xBTsendbufferXbee(s_buffer, idx);
	
	return ret;
}


int xBeeTools::xBTprint(const uint8_t *buffer, size_t size)
{
unsigned int idx = 0;
int ret = SUCCESS;

	for (unsigned int i = 0; i < size; i++)
	{
		s_buffer[idx++] = buffer[i];
	}
	
	ret = xBTsendbufferXbee(s_buffer, idx);
	
	return ret;
}
//************************************************************************
int xBeeTools::xBTprint(const String &argString)
{
unsigned int idx = 0;
int ret = SUCCESS;

	for (unsigned int i = 0; i < argString.length(); i++)
	{
		s_buffer[idx++] = argString[i];
	}
	
	ret = xBTsendbufferXbee(s_buffer, idx);
	
	return ret;
}

//************************************************************************
int xBeeTools::xBTprint(char c, int base)
{
int ret = SUCCESS;
	
	ret = xBTprint((long) c, base);
	return ret;
}

//************************************************************************
int xBeeTools::xBTprint(unsigned char b, int base)
{
int ret = SUCCESS;
    
	ret = xBTprint((unsigned long) b, base);
	return ret;
}

//************************************************************************
int xBeeTools::xBTprint(int n, int base)
{
int ret = SUCCESS;

	ret = xBTprint((long) n, base);
	return ret;
}

//************************************************************************
int xBeeTools::xBTprint(unsigned int n, int base)
{
int ret = SUCCESS;

	ret = xBTprint((unsigned long) n, base);
	return ret;
}

//************************************************************************
int xBeeTools::xBTprint(long n, int base)
{
int ret = SUCCESS;

	ret = xBTprintNumber(n, base);
	return ret;
}

//************************************************************************
int xBeeTools::xBTprint(unsigned long n, int base)
{
int ret = SUCCESS;

	ret = xBTprintNumber(n, base);
	return ret;
}

//************************************************************************
int xBeeTools::xBTprint(double n, int digits)
{
int ret = SUCCESS;

	ret = xBTprintFloat(n, digits);
	return ret;
}

//************************************************************************
int xBeeTools::xBTprintNumber(long n, uint8_t base)
{

unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars.
unsigned int i = 0;
unsigned int idx = 0;
int ret = SUCCESS;

	if (n < 0)
	{;
			s_buffer[idx++] = '-';
			n = -n;
	}
	
	if (n == 0)
	{
		s_buffer[idx++] = '0';
		return SUCCESS;
	}

	while (n > 0)
	{
		buf[i++] = n % base;
		n /= base;
	}

	for (; i > 0; i--)
	{
		s_buffer[idx++] = ((char) (buf[i - 1] < 10 ?
					'0' + buf[i - 1] :
					'A' + buf[i - 1] - 10));					
					
	}

	ret = xBTsendbufferXbee(s_buffer, idx);
	
	return ret;
}

//************************************************************************
int xBeeTools::xBTprintFloat(double number, uint8_t digits)
{
unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars.
unsigned int i = 0; 
unsigned int idx = 0;       
int ret = SUCCESS;
        
        	
	// Handle negative numbers
	if (number < 0.0)
	{
		s_buffer[idx++] = '-';
		number = -number;
	}

	// Round correctly so that print(1.999, 2) prints as "2.00"
	double rounding = 0.5;
	for (uint8_t i=0; i<digits; ++i)
		rounding /= 10.0;

	number += rounding;

	// Extract the integer part of the number and print it
	unsigned long int_part = (unsigned long)number;
	double remainder = number - (double)int_part;
	i = 0;
	while (int_part > 0)
	{
		buf[i++] = int_part % 10;
		int_part /= 10;
	}

	for (; i > 0; i--)
	{
		s_buffer[idx++] = ((char) '0' + buf[i - 1]);					
	}
	
	

	// Print the decimal point, but only if there are digits beyond
	if (digits > 0)
		s_buffer[idx++] = '.';		

	// Extract digits from the remainder one at a time
	while (digits-- > 0)
	{
		remainder *= 10.0;
		int toPrint = int(remainder);
		i = 0;
	    while (toPrint > 0)
	    {
	        	buf[i++] = toPrint % 10;
		        toPrint /= 10;
	    }

	    for (; i > 0; i--)
	    {
		        s_buffer[idx++] = ((char) '0' + buf[i - 1]);					
	    }		
		remainder -= toPrint;
	}
	
	ret = xBTsendbufferXbee(s_buffer, idx);
	
	return ret;
}


int xBeeTools::xBTsendbufferXbee(char *buf, unsigned int buf_len)
{

int count_max = 0;
int start = -1;
int Endofdata = 0;
int ret = SUCCESS;

uint8_t payload[PAYLOAD_SIZE];

while(Endofdata == 0) {
    if (buf_len + 1 > PAYLOAD_SIZE - 1) 
    {
        count_max = PAYLOAD_SIZE - 1;
        payload[0] = 0;
    }
    else
    {
        count_max = buf_len + 1;
        payload[0] = 1;
        Endofdata = 1;
    }                     
    for (int count=1;count<count_max;count++)
    {
        payload [count] = buf[count+start];
    }
    start = count_max;
    buf_len = buf_len - count_max;
 
    ret = xBTsendXbee(payload, count_max);
    if (ret != SUCCESS) {   
        return ret;
    }
  }
  
  return SUCCESS;
}


int xBeeTools::xBTsendXbee(uint8_t* msg, unsigned int msg_len)
 {
  uint8_t payload[PAYLOAD_SIZE];
 
  for (unsigned int i=0;i<msg_len;i++)
  {
          payload [i] = msg[i];
  }
  
  // Create an XBee object
  XBee xbee = XBee();
   
  // Tell XBee to start Serial: 
  // Initialize the UART for use, setting the baud rate to 9600, data size of 8-bits, and no parity.
  xbee.begin(9600);
  
  // Specify the address of the remote XBee (this is the SH + SL)
  XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x407be775);
  
  // Create a TX Request contening payload for the remote XBee, with ACK and frame id = 0x12
  Tx64Request tx = Tx64Request(addr64, ACK_OPTION, payload, msg_len, 0x12); 
 
  
  // Send the request 
  xbee.send(tx);
 
  TxStatusResponse txStatus = TxStatusResponse();
  
  // after sending a tx request, we expect a status response
  // wait up to 5 seconds for the status response
  if (xbee.readPacket(5000)) {
        // got a response!
  //Serial.println("got a response!");
        // should be a tx status            	
    	if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
    	   xbee.getResponse().getTxStatusResponse(txStatus);
    		
    	   // get the delivery status, the fifth byte    	   
           if (txStatus.getStatus() == SUCCESS) {
                // success.  time to celebrate
                return SUCCESS;          	
           }
           else
           {
                // the remote XBee did not receive our packet. is it powered on?
                Serial.println("STATUS_ERROR");
                return ((-100*STATUS_ERROR) - txStatus.getStatus());
           }         
      }
      else if (xbee.getResponse().isError()) {
         Serial.print("RESPONSE_ERROR");
            return ((-100*RESPONSE_ERROR) - xbee.getResponse().getErrorCode()); 
      }
    }
    else {
      // local XBee did not provide a timely TX Status Response.  Radio is not configured properly or connected
      Serial.println("NO_RESPONSE");
      return NO_RESPONSE; 
    }
}



int xBeeTools::xBTreceiveXbee(uint8_t *msg, int timeout) {
     
  // Create an XBee object
  XBee xbee = XBee();
  
  // Tell XBee to start Serial: 
  // Initialize the UART for use, setting the baud rate to 9600, data size of 8-bits, and no parity.
  xbee.begin(9600);

  // Define a RX Response  
  Rx64Response rx64 = Rx64Response();

  // wait up to timeout seconds for the status response     
  if (xbee.readPacket(timeout)) {
        // got a response!

        // should be a rx response
        if (xbee.getResponse().getApiId() == RX_64_RESPONSE) {
           // got a rx response time to celebrate
           xbee.getResponse().getRx64Response(rx64);

           for (unsigned int i=0;i<rx64.getDataLength();i++)
           {
                msg [i] = rx64.getData(i);
           }
           return SUCCESS;
        }
        else
        {
           // not something we were expecting
  	       return UNEXPECTED_APIID;
        }
  }
  else if (xbee.getResponse().isError()) {
           return ((-100*RESPONSE_ERROR) - xbee.getResponse().getErrorCode()); 
  } 
  else {
      // local XBee did not provide a timely TX Status Response.  Radio is not configured properly or connected
      return NO_RESPONSE; 
    }
}