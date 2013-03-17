#ifndef XBEECMDROBOT_h
#define XBEECMDROBOT_h

#define SUCCESS 0
#define FILE_OPEN_ERROR -1000
#define FILE_CLOSE_ERROR -1001

int XBeeSendPicture (int n);
/* Description: send using XBee interface a picture stored                    */
/*              in a file PICTxx.jpg on a SD card                             */
/* input:       n                                                             */ 
/*                  = picture number xx                                       */     
/* output:      return                                                        */                            
/*                  = FILE_OPEN_ERROR if an error occurs during file opening  */
/*                  = FILE_CLOSE_ERROR if an error occurs during file closing */
/*                  = XBEE_ERROR if an error occurs with the XBee interface   */
/*                  = SUCCESS otherwise                                       */ 
/* lib:         sprintf                                                       */                                
/*              open (file)                                                   */
/*              read (file)                                                   */
/*              close (file)                                                  */                                  
/*              XBTsendXbee                                                   */




int XBeeCmdRobot (void);

#endif