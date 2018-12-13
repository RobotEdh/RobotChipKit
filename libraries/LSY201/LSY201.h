/*
  LSY201.h - Library for communicating with JPEG serial camera LYS201
  Created by EDH, July 26, 2012.
  Thanks to Ryan Owens
  Released into the public domain.
*/

#ifndef LSY201_h
#define LSY201_h

/* Camera                                                                */
/* Serial interface is provided on pins:                                 */
/*        1 = Power +5V                                                  */                
/*        2 = TXD connected to RX                                        */
/*        3 = RXD connected to TX                                        */
/*        4 = Ground                                                     */


#include <inttypes.h>

#define SUCCESS 0
#define CAMERA_ERROR -6
#define FILE_OPEN_ERROR -1000
#define FILE_CLOSE_ERROR -1001


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
        /*              Serial.flush                                                 */
        /*              Serial.available                                             */
        /*              Serial.read                                                  */
        		
		int begin(void);
        /* Description: Initialize the Serial port and call reset ()                 */                                            
        /* input:       none                                                          */                      
        /* output:      return                                                        */ 
        /*                  = return of the reset() function otherwise                */ 
        /* lib:       	Serial.begin                                                 */
        /*              reset                                                         */	
			
		int getSize(int * size);
        /* Description: Get the size of the image currently stored in the camera      */                                            
        /* input:       none                                                          */                      
        /* output:      size                                                          */
        /*                  = size of the image                                       */
        /*              return                                                        */                            
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error reading the serial buffer                  */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              Serial.flush                                                 */
        /*              Serial.available                                             */
        /*              Serial.read                                                  */
        /*              Serial.write                                                 */       		
		
		int takePicture(void);
        /* Description: Take a picture                                                */                                            
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */
 		
		
		int stopPictures(void);
        /* Description: Stop taking pictures                                          */                                            
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */
		
		int readData(long int address, uint8_t *buf, int *count, int * eof);
        /* Description: Get the picture data from the camera                          */                                            
        /* input:       address                                                       */
        /*                  = address of the image, must start at 0                   */
        /* output:      buf                                                           */
        /*                  = buffer read from camera but without headers             */
        /*              count                                                         */
        /*                  = number of bytes read                                    */                
        /*              eof                                                           */
        /*                  = 1 if end of picture else = 0                            */ 
        /*              return                                                        */                            
        /*                  = -2, -20 if bad answer from the camera                   */
        /*                  = -10, -100, -1000 if error reading the serial buffer     */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              Serial.available                                             */
        /*              Serial.read                                                  */
        /*              Serial.write                                                 */ 
        		
		int compress(int ratio);
        /* Description: Compress the picture according the ratio                      */                                            
        /* input:       ratio                                                         */
        /*                  = ratio for compression (0X00 < ratio < 0XFF, usually 36) */                     
        /* output:      return                                                        */                           
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -3 if invalid ratio                                     */
        /*                  = -10 if error reading the serial buffer                  */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              Serial.flush                                                 */
        /*              Serial.available                                             */
        /*              Serial.read                                                  */
        /*              Serial.write                                                 */       		
		
		int imageSizeSmall(void);
        /* Description: Change the size of the image is Small format (160*120)        */
        /*              but without saving the configuration                          */
        /*              so it will turn back to 320*240 after disconnect or reset     */
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */		

		int imageSizeMedium(void);
        /* Description: Change the size of the image is Medium format (320*240)       */
        /*              but without saving the configuration                          */
        /*              so it will turn back to 320*240 after disconnect or reset     */
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */			
	    
	    int imageSizeLarge(void);
        /* Description: Change the size of the image is Large format (640*480)        */
        /*              but without saving the configuration                          */
        /*              so it will turn back to 320*240 after disconnect or reset     */
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */	
        
		int imageSizeSmallSave(void);
        /* Description: Change the size of the image is Small format (160*120)        */
        /*              with saving the configuration, then reset                     */
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = return of the reset() function otherwise                */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */			
		
		int imageSizeMediumSave(void);
        /* Description: Change the size of the image is Medium format (320*240)       */
        /*              with saving the configuration, then reset                     */
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = return of the reset() function otherwise                */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */
        		
		int imageSizeLargeSave(void);
        /* Description: Change the size of the image is Large format (640*480)        */
        /*              with saving the configuration, then reset                     */
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = return of the reset() function otherwise                */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */
                
        int makePicture (int n);
        /* Description: make a picture using the JPEGCamera lib                       */
        /*              and save it on a SD card in a file PICTxx.jpg                 */
        /* input:       n                                                             */ 
        /*                  = picture number xx                                       */     
        /* output:      return                                                        */                            
        /*                  = FILE_OPEN_ERROR if an error occurs during file opening  */
        /*                  = FILE_CLOSE_ERROR if an error occurs during file closing */
        /*                  = CAMERA_ERROR if an error occurs with the camera         */
        /*                  = SUCCESS otherwise                                       */  
        /* lib:         sprintf                                                       */                                
        /*              open (file)                                                   */
        /*              write (file)                                                  */
        /*              close (file)                                                  */                                  
        /*              JPEGCamera.takePicture                                        */
        /*              JPEGCamera.getSize                                            */  
        /*              JPEGCamera.readData                                           */
        /*              JPEGCamera.stopPictures                                       */				
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
        /* lib:         Serial.write                                                 */
        /*              Serial.available                                             */
        /*              Serial.read                                                  */
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
