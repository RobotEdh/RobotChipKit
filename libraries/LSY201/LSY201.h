/*
  LSY201.h - Library for commmunicating with JPEG serial camera LYS201
  Created by EDH, July 26, 2012.
  Thanks to Ryan Owens
  Released into the public domain.
*/

#ifndef LSY201_h
#define LSY201_h

#include <inttypes.h>

class JPEGCameraClass
{
	public:
		JPEGCameraClass();
		
		int reset(void);
        /* Description: Reset the camera                                              */                                            
        /* input:       none                                                          */                      
        /* output:      return                                                        */                            
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -5 if bad init message received from the camera         */
        /*                  = -10 if error reading the serial buffer or sendCommand   */
        /*                  = 0 otherwise                                             */ 
        /* lib:         sendCommand                                                   */
        /*              uint8Compare                                                  */
        /*              Serial1.flush                                                 */
        /*              Serial1.available                                             */
        /*              Serial1.read                                                  */
        		
		int begin(void);
        /* Description: Initialize the serial1 port and call reset ()                 */                                            
        /* input:       none                                                          */                      
        /* output:      return                                                        */                            
        /*                  = return of the reset() function                          */ 
        /* lib:         reset                                                         */		
		
		
		int getSize(int * size);
		int takePicture(void);
		int stopPictures(void);
		int readData(long int address, uint8_t *buf, int *count, int * eof);
		int compress( int ratio);
		int imageSizeSmall(void);
		int imageSizeMedium(void);
	    int imageSizeLarge(void);
		int imageSizeSmallSave(void);
		int imageSizeMediumSave(void);
		int imageSizeLargeSave(void);		
	private:
		
		int sendCommand(const uint8_t *command, uint8_t *response, int wlen, int rlen);
        /* Description: Send a basic command to the camera and get the response       */                                            
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
        /* lib:         Serial1.write                                                 */
        /*              Serial1.available                                             */
        /*              Serial1.read                                                  */
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