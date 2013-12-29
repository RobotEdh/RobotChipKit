#ifndef ROBOT_h
#define ROBOT_h

#include <inttypes.h> // used for uint8_t type
#include <motor.h>

#define Led_Yellow 16   // Led Yellow connected to digital pin J4-06(SCL3A/SDO3A/U3ATX/PMA8/CN18/RF5)
#define Led_Red    17   // Led Red connected to digital pin J4-05(SDA3A/SDI3A/U3ARX/PMA9/CN17/RF4)

#define buzzPin    39   // Buzzerconnected to digital pin J8-15(PMRD/CN14/RD5)

void blink(int led);
/* Description: blink a led                                                   */                                            
/* input:       led                                                           */
/* output:      none                                                          */
/* lib:         digitalWrite                                                  */
/*              delay                                                         */ 


void buzz(int buzzNb);
/* Description: buzz                                                          */                                            
/* input:       buzzNb                                                        */
/*              Number of times buzzing                                       */
/* output:      none                                                          */
/* lib:         digitalWrite                                                  */
/*              delay                                                         */


int robot_begin();     
/* Description: initialize everything, must be called during setup            */                                            
/* input:       none                                                          */
/* output:      return                                                        */                             
/*                  = SUCCESS always even if error during initialization      */                                
/* lib:         JPEGCamera.begin                                              */ 
/*              pinMode                                                       */
/*              digitalWrite                                                  */

int CmdRobot (uint16_t cmd[3], int16_t *resp, int *presp_len);
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
