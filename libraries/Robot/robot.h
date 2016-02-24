#ifndef ROBOT_h
#define ROBOT_h

#include <WProgram.h> // used for pin definition
#include <inttypes.h> // used for uint8_t type
#include <motor.h>

#define Led_Blue   70  // Led Bleue connected to digital pin J14-02 (TMS/RA0)
#define Led_Red    71  // Led Red   connected to digital pin J14-04 (TCK/RA1)

#define buzzPin      39   // Buzzer connected                 to digital pin J8-15 (PMRD/CN14/RD5)
#define MOTION_PIN   28   // Infrared motion sensor connected to digital pin J9-10 (AERXERR/RG15)

#define TEMT6000_Pin A1   // Brightness Sensor TEMT6000 analogic pin J5-02 A1 (PGEC1/AN1/CN3/RB1)   Use ADC module channel 2
#define MICRO_Pin    A2   // Electret micro             analogic pin J5-03 A1 (AN2/C2IN-/CN4/RB2)   Use ADC module channel 2

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
