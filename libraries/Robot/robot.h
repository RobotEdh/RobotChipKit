#ifndef ROBOT_h
#define ROBOT_h

#include <Arduino.h> // used for pin definition
#include <inttypes.h> // used for uint8_t type
#include <motor.h>

#define CMD_START         0x01
#define CMD_STOP          0x02
#define CMD_INFOS         0x03
#define CMD_PICTURE       0x04
#define CMD_TURN          0x05
#define CMD_CHECK_AROUND  0x06
#define CMD_MOVE_TILT_PAN 0x07
#define CMD_GO            0x08
#define CMD_ALERT         0x09
#define CMD_CHECK         0x0A
#define CMD_PI            0x0B

#define PI_NO_COMM        0
#define PI_ALERT_ONLY     1
#define PI_ALERT_INFOS    2

#define CMD_SIZE   10

#define PAYLOAD_SIZE 80

#define NO_ALERT          0
#define ALERT_MOTION      1
#define ALERT_LUX         2
#define ALERT_TEMPERATURE 3
#define ALERT_NOISE       4

#define NB_LUX 6
#define MAX_VAR_LUX 100
#define NB_TEMPERATURE 6
#define MAX_VAR_TEMPERATURE 1.5


#define Led_Green   22  
#define Led_Red     23  
#define Led_Blue    24 
#define buzzPin     25  

#define TEMT6000_Pin A2   // Brightness Sensor TEMT6000 analogic pin A2 
#define MICRO_Pin    A3   // Electret micro             
#define MOTION_PIN  19    // Pin used by the Motion sensor connected to interrupt INT4 on MEGA2560

#define IOT_PIN     18    // Pin used by the IOT connected to interrupt INT5 on MEGA2560

void IntrMotion();  // interrupt handler Motion sensor
void IntrIOT();     // interrupt handler IOT


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

int check();
/* Description: check                                                         */                                            
/* input:       none                                                          */
/* output:      alert                                                         */
/*                                                                            */

int infos(uint16_t *resp, uint8_t *resplen);
/* Description: get infos                                                     */                                            
/* input:       none                                                          */
/* output:      resp                                                          */
/*                  = response                                                */
/*              resp_len                                                      */
/*                  = response lenght                                         */
/*              return                                                        */
/*                  = SUCCESS is no error otherwise error code                */                               
/* lib:                                                                       */
/*              get_SpeedMotorRight                                           */
/*              get_SpeedMotorLeft                                            */
/*              CMPS03_read                                                   */
/*              GP2Y0A21YK_getDistanceCentimeter                              */

int robot_begin();     
/* Description: initialize everything, must be called during setup            */                                            
/* input:       none                                                          */
/* output:      return                                                        */                             
/*                  = SUCCESS always even if error during initialization      */                                
/* lib:         JPEGCamera.begin                                              */ 
/*              pinMode                                                       */
/*              digitalWrite                                                  */

int robot_command (uint16_t *cmd, uint16_t *resp, uint8_t *resplen);
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

int robot_main(); 
int robot_IOT();    
int robot_Send_Picture (uint8_t n);
#endif
