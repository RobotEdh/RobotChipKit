/**
 * Copyright (c) 2009 Andrew Rapp. All rights reserved.
 *
 * This file is part of XBee-Arduino.
 *
 * XBee-Arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XBee-Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XBee-Arduino.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <XBee.h>

#define SUCESS 0
#define UNEXPECTED_APIID -1
#define STATUS_ERROR -2
#define RESPONSE_ERROR -3
#define NO_RESPONSE -4

#define PAYLOAD_SIZE 100





int Xbee_receive(char * msg) {
     
  XBee xbee = XBee();
  xbee.begin(9600);
  
  Rx64Response rx64 = Rx64Response();
     
  Serial.print("\nXbee_receive");
   
  if (xbee.readPacket(5000)) {
        // got a response!
        Serial.print("\ngot a response!");  
        
         // should be a znet rx response    
   
        if (xbee.getResponse().getApiId() == RX_64_RESPONSE) {
        // got a rx packet
        Serial.print("\ngot a rx packet!");
        xbee.getResponse().getRx64Response(rx64);
        Serial.print("\ndata:");
        for (int count=0;count<rx64.getDataLength();count++)
        {
                Serial.print(rx64.getData(count));
                msg [count] = rx64.getData(count);
        }
        return rx64.getDataLength();
      }
      else {
      // not something we were expecting
  	    return UNEXPECTED_APIID;
      }
    }
    else if (xbee.getResponse().isError()) {
            Serial.print("\nError reading packet.  Error code: "); 
            Serial.print(xbee.getResponse().getErrorCode());
            return RESPONSE_ERROR;
    } 
    else {
      // local XBee did not provide a timely TX Status Response.  Radio is not configured properly or connected
      Serial.print("\nNO_RESPONSE");
      return NO_RESPONSE; 
    }
} 

int xbee_send_data(char* msg) {

int ret = 0;
int count_max = 0;
int start = -1;
bool Endofdata = false;

char payload[PAYLOAD_SIZE];
Serial.print("xbee_send_data -");
while(!Endofdata) {
    if (strlen(msg) > PAYLOAD_SIZE - 1) 
    {
        count_max = PAYLOAD_SIZE - 1;
        payload[0] = 0;
    }
    else
    {
        count_max = strlen(msg)+1;
        payload[0] = 1;
        Endofdata= true;
    }                     
    for (int count=1;count<count_max;count++)
    {
        payload [count] = msg[count+start];
    }
    start = count_max;
    Serial.print(" - count_max:");
    Serial.print(count_max);
    Serial.print(" - payload:");
    Serial.print(payload);
    delay(5000);
    ret = xbee_send(payload, count_max);
    if (ret != SUCCESS) return ret;
  }
  
  return SUCCESS;
}

int xbee_send(char* msg, int msg_len) {


  uint8_t payload[PAYLOAD_SIZE];
  
  for (int count=0;count<msg_len;count++)
  {
          payload [count] = msg[count];
  }
  
  XBee xbee = XBee();
  xbee.begin(9600);
  
 

  // 64-bit addressing: This is the SH + SL address of remote XBee
  XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x407be775);
  
  Tx64Request tx = Tx64Request(addr64, ACK_OPTION, payload, sizeof(payload),0x12); 
     

  TxStatusResponse txStatus = TxStatusResponse();
    
  xbee.send(tx);
  Serial.print("\nsendxbee2");
    // after sending a tx request, we expect a status response
    // wait up to 5 seconds for the status response
    if (xbee.readPacket(5000)) {
        // got a response!
        Serial.print("\ngot a response!");

        // should be a tx status            	
    	if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
    	   xbee.getResponse().getZBTxStatusResponse(txStatus);
    		
    	   // get the delivery status, the fifth byte
    	   
           if (txStatus.getStatus() == SUCCESS) {
                // success.  time to celebrate
                Serial.print("\nsuccess");
                return SUCCESS;          	
           }
           else {
                // the remote XBee did not receive our packet. is it powered on?
                Serial.print("\nthe remote XBee did not receive our packet");
                return STATUS_ERROR;
           }         
      }
      else if (xbee.getResponse().isError()) {
            Serial.print("\nError reading packet.  Error code: "); 
            Serial.print(xbee.getResponse().getErrorCode());
            return RESPONSE_ERROR; 
      }
    }
    else {
      // local XBee did not provide a timely TX Status Response.  Radio is not configured properly or connected
      Serial.print("\nNO_RESPONSE");
      return NO_RESPONSE; 
    }
}

void setup() {
  
  Serial.begin(9600); // initialize serial port for debug

}

void loop() {
 char buf [100]; 
 int ret;
 
 ret = xbee_send_data("Hello");
 //ret = Xbee_receive(buf);
 
 
}


