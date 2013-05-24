#ifndef ROBOT_h
#define ROBOT_h

#include <inttypes.h> // used for uint8_t type
#include <motor.h>

int robot_begin();     
/* Description: initialize everything, must be called during setup            */                                            
/* input:       none                                                          */
/* output:      return                                                        */                             
/*                  = SUCCESS always even if error during initialization      */                                
/* lib:         JPEGCamera.begin                                              */ 

int CmdRobot (uint8_t cmd[3], uint8_t *resp, int *presp_len);
/* Description: command the robot                                             */                                            
/* input:       cmd                                                           */
/*                  = command and the related parameters                      */
/* output:      resp                                                          */
/*                  = response                                                */
/*              presp_len                                                     */
/*                  = response lenght                                         */
/*              return                                                        */
/*                  = SUCCESS is no error otherwise error code                */                               
/* lib:         stop                                                          */ 
/*              start_forward                                                 */                            
/*              start_forward_test                                            */ 
/*              check_around                                                  */
/*              TiltPan_move                                                  */
/*              turn                                                          */
/*              get_SpeedMotorRight                                           */
/*              get_SpeedMotorLeft                                            */
/*              CMPS03_read                                                   */
/*              GP2Y0A21YK_getDistanceCentimeter                              */
/*              makePicture                                                   */   
/*              go                                                            */  

#endif
