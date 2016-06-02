/*
  Serial2IOT.cpp - Library for commmunicating with serial IOT Photon Particle IO
  Created by EDH, Feb 20, 2016.
  Released into the public domain.
*/

#ifndef SERIAL2IOT_h
#define SERIAL2IOT_h

/* IOT                                                                   */
/* UART3 interface is provided on pins:                                  */           
/*        TX connected to J4-04 (AETXD1/SCK1A/U1BTX/U1ARTS/CN21/RD15)    */
/*        RX connected to J4-03 (AETXD0/SS1A/U1BRX/U1ACTS/CN20/RD14)     */

#include <inttypes.h>


#define MSG_SIZE_MAX 255
#define MAX_TAGS     63

const uint8_t CMD          = 1;
const uint8_t RESP_OK      = 2;
const uint8_t RESP_KO      = 3;

const uint8_t TAG_CMD      = 'C';
const uint8_t TAG_PARAM    = 'P';
const uint8_t TAG_RESP     = 'R';
const uint8_t TAGSYMBOL    = '#';
const uint8_t SBN1 = 0xFA;
const uint8_t SBN2 = 0xFB;
const uint8_t EBN1 = 0xFE;
const uint8_t EBN2 = 0xFF;

class RIOTClass
{
	public:
		RIOTClass();
			
		void begin(void);
        /* Description: Initialize the serial2 port                                   */                                            
        /* input:       none                                                          */                      
        /* output:      none                                                          */ 
        /* lib:       	Serial2.begin                                                 */
			
        void RIOTgetTags(uint8_t *buf, uint8_t *tag, uint16_t *value, uint8_t *nbtags);
             		
		int RIOTread(uint8_t *msg, uint8_t *msglen);
        /* Description: Read a message from the IOT                                   */                                            
        /* input:       none                                                          */ 
        /* output:      message                                                       */
        /*                  = message received                                        */
        /*              rlen                                                          */
        /*                  = message length received                                 */                                  
        /*              return                                                        */                            
        /*                  = -10 if error reading the serial buffer                  */
        /*                  = 0 otherwise                                             */ 
        /* lib:         Serial2.available                                             */
        /*              Serial2.read                                                  */
        
        void RIOTsend(uint8_t msgtype, uint8_t value1, uint16_t *param, uint8_t paramlen);
        void RIOTsend(uint8_t msgtype,                 uint16_t *param, uint8_t paramlen);
        /* Description: Send a message to the IOT and get the response                */                                            
        /* input:       message                                                       */ 
        /*                  = message to send                                         */ 
        /* input:       wlen                                                          */
        /*                  = message length                                          */ 
        /* input:       rlen                                                          */
        /*                  = response length expected                                */                               
        /* output:      response                                                      */
        /*                  = reponse received                                        */
        /*              return                                                        */                            
        /*                  = -10 if error reading the serial buffer                  */
        /*                  = 0 otherwise                                             */ 
        /* lib:         Serial2.write                                                 */
        /*              Serial2.available                                             */
        /*              Serial2.read                                                  */
        
	private:
		
};

#endif
