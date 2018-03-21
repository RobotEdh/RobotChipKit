/*
  Serial2IOT.cpp - Library for commmunicating with serial IOT Photon Particle IO
  Created by EDH, Feb 20, 2016.
  Released into the public domain.
*/

#include <WProgram.h> // used for delay function (core lib)
#include <Serial2IOT.h>



      
// Constructor
RIOTClass::RIOTClass()
{
}

// Initialize the serial2 port
void RIOTClass::begin(void)
{
	// baud rate is 38400
	Serial2.begin(38400);
}

void RIOTClass::RIOTgetTags(uint8_t *buf, uint8_t *tag, uint16_t *value, uint8_t *nbtags)
{
   uint8_t i=0;
   uint8_t p=0;

   while ((buf[i] == TAGSYMBOL) && (p < MAX_TAGS))
   {
       tag[p]   = buf[i+1];
       value[p] = (uint16_t)(buf[i+2]<<8) | buf[i+3];
       p++;
       i=i+4;
   }
   *nbtags = p;
   
}    

// Read a message
int RIOTClass::RIOTread(uint8_t *msg, uint8_t *msglen)
{
	int ibuf =-1;
	uint8_t istart = 0;
	uint8_t istop  = 0;
	uint8_t i = 0;
	unsigned long previousTime;

	//Get the response from the IOT and add it to the response string
	while ((istop != 2) && (i < MSG_SIZE_MAX))
	{
	    previousTime = millis();
		while((Serial2.available() == 0) && ( millis()< previousTime+ (30*1000))); // waiting for data until 30 seconds in the serial buffer
		Serial.print("RIOTread, read ");
 
		ibuf = Serial2.read();
        Serial.print("ibuf: "); Serial.print(ibuf,HEX); Serial.print("/"); Serial.println((isalnum(ibuf))?((char)ibuf):(' '));

		if (ibuf == -1) return -10; // serial buffer empty, should not happen as we wait before
		
		if      (ibuf == SBN1) istart = 1;
		else if (ibuf == SBN2) istart++;
		else if (ibuf == EBN1) istop = 1;
		else if (ibuf == EBN2) istop++;    
        else if (istart == 2) msg[i++] = (uint8_t)ibuf;
	}
    
    if (i == MSG_SIZE_MAX) return -11;
    
    *msglen = i;
	return 0;
}


// Send a message
void RIOTClass::RIOTsend(uint8_t msgtype, uint16_t *param, uint8_t paramlen)
{
    RIOTsend(msgtype, 0, param, paramlen, 0);
}

void RIOTClass::RIOTsend(uint8_t msgtype, uint8_t value1, uint16_t *param, uint8_t paramlen)
{
    RIOTsend(msgtype, value1, param, paramlen, 0);
}

void RIOTClass::RIOTsend(uint8_t msgtype, uint16_t *param, uint8_t paramlen, uint8_t wcmdId)
{
    RIOTsend(msgtype, 0, param, paramlen, wcmdId);
}

void RIOTClass::RIOTsend(uint8_t msgtype, uint8_t value1, uint16_t *param, uint8_t paramlen, uint8_t wcmdId)
{
    Serial2.write(SBN1);
    Serial2.write(SBN2);
    
    Serial2.write(TAGSYMBOL);
    Serial2.write(TAG_CMDID);
    Serial2.write(uint8_t(0));
    Serial2.write(wcmdId);
    
    Serial2.write(TAGSYMBOL);
    
    switch (msgtype) {
        
    case RESP_OK:
    case RESP_KO:
       Serial2.write(TAG_RESP);
       Serial2.write(uint8_t(0));
       Serial2.write(msgtype);
 
       break; 
   
    case CMD:
       Serial2.write(TAG_CMD);
       Serial2.write(uint8_t(0));
       Serial2.write(value1);
 
       break;         
    }     
        
    for (uint8_t j=0; j<paramlen; j++)
    {
       Serial2.write(TAGSYMBOL);
       Serial2.write(TAG_PARAM);
       Serial2.write((uint8_t)(param[j]>>8));
       Serial2.write((uint8_t)(param[j]));
    }
        
    Serial2.write(EBN1);
    Serial2.write(EBN2);

}


