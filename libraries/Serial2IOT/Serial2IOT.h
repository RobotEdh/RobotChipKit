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

class IOTClass
{
	public:
		IOTClass();
			
		void begin(void);
        /* Description: Initialize the serial2 port                                   */                                            
        /* input:       none                                                          */                      
        /* output:      none                                                          */ 
        /* lib:       	Serial2.begin                                                 */
			
		int ping(void);
        /* Description: Send a ping HELLO to IOT and wait for pong HELLO              */                                            
        /* input:       none                                                          */                      
        /*              return                                                        */                            
        /*                  = -2 if bad answer from the IO                            */
        /*                  = -10 if error reading the serial buffer                  */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */
  		
	private:
		
		int sendCommand(const uint8_t *command, uint8_t *response, int wlen, int rlen);
        /* Description: Send a basic command to the IOT and get the response          */                                            
        /* input:       command                                                       */ 
        /*                  = command to send                                         */ 
        /* input:       wlen                                                          */
        /*                  = command length                                          */ 
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
        /*              millis                                                        */
        
		int uint8Compare(const uint8_t *a1, const uint8_t *a2, int len);
        /* Description: Compare 2 uint8_t arrays of same size                         */                                            
        /* input:       a1                                                            */ 
        /*                  = first uint8_t to compare                                */ 
        /* input:       a2                                                            */
        /*                  = second uint8_t to compare                               */ 
        /* input:       len                                                           */
        /*                  = length of the 2 arrays                                  */                               
        /* output:      return                                                        */                       
        /*                  = 0 if the 2 arrays are equal                             */
        /*                  = -1 otherwise                                            */ 
        /* lib:         none                                                          */
		
};

#endif
