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
/*              infoSDCard                                                    */                            

int CmdRobot (uint8_t cmd[3], uint8_t *resp, int *presp_len);

#endif