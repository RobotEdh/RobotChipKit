#ifndef ROBOT_h
#define ROBOT_h

#include <WProgram.h> // used for pin definition
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

#define CMD_SIZE   10


// 0: motor_state
// 1: SpeedMotorRight
// 2: SpeedMotorLeft    
// 3: direction
// 4: distance
// 5: temperature
// 6: brightness
// 7: noise
// 8 alert status
// 9: picture number
#define RESP_SIZE 10
    
const char szField[RESP_SIZE][30]={    
"motor_state",
"SpeedMotorRight",
"SpeedMotorLeft",
"direction",
"distance",
"temperature",
"brightness",
"noise",
"alert_status",
"no_picture"
};

#define NO_ALERT          0
#define ALERT_MOTION      1
#define ALERT_LUX         2
#define ALERT_TEMPERATURE 3
#define ALERT_NOISE       4

#define NB_LUX 6
#define MAX_VAR_LUX 100
#define NB_TEMPERATURE 6
#define MAX_VAR_TEMPERATURE 1.5


#define Led_Blue   70  // Led Bleue connected to digital pin J14-02 (TMS/RA0)
#define Led_Red    71  // Led Red   connected to digital pin J14-04 (TCK/RA1)
#define buzzPin    39  // Buzzer connected                 to digital pin J8-15 (PMRD/CN14/RD5)

#define TEMT6000_Pin A1   // Brightness Sensor TEMT6000 analogic pin J5-02 A1 (PGEC1/AN1/CN3/RB1)   Use ADC module channel 2
#define MICRO_Pin    A2   // Electret micro             analogic pin J5-03 A1 (AN2/C2IN-/CN4/RB2)   Use ADC module channel 2

#define MOTION_INT   3    // INT used by the Motion sensor connected to interrupt pin INT3 J4-01(AETXCLK/SCL1/INT3/RA14) Use INT3
#define MOTION_PIN  21

#define IOT_INT      4    // INT used by the IOT connected to interrupt pin INT4 J4-02 (AETXEN/SDA1/INT4/RA15) Use INT4
#define IOT_PIN     20

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

#endif
