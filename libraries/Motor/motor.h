/*
  motor.h - Library for motor control
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/


#ifndef MOTOR_h
#define MOTOR_h

#include <wiring.h> // used for analogic read function (core lib)
#include <PID.h>

#define SUCCESS 0
#define SPEED_ERROR -1
#define OBSTACLE -2
#define TIMEOUT -3
#define BAD_ANGLE -4
#define COMPASS_ERROR -5
#define CAMERA_ERROR -6
#define SDCARD_ERROR -7

#define CMD_SIZE 3
#define RESP_SIZE 8

#define CMD_START         0x01
#define CMD_STOP          0x02
#define CMD_INFOS         0x03
#define CMD_PICTURE       0x04
#define CMD_TURN_RIGHT    0x05
#define CMD_TURN_LEFT     0x06
#define CMD_CHECK_AROUND  0x07
#define CMD_MOVE_TILT_PAN 0x08


#define STATE_STOP 0x00
#define STATE_GO   0x01

#define SPEEDNOMINAL 100
#define SPEEDMAX 255     //(255=PWM max)
#define SPEEDDELTA 40    // used to turn 

#define LEFT_DIRECTION 1
#define RIGHT_DIRECTION 2

#define BOTH_MOTOR 0
#define LEFT_MOTOR 1
#define RIGHT_MOTOR 2

#define DISTANCE_MIN 50 // 50 cm before stopping, must be > 20cm which is lower range of IR sensor

#define InMotorRight1Pin  30      // In pin of Motor controller #1 for motor right #1 connected to digital pin J9-08(PMD7/RE7)
#define EnableMotorRight1Pin 3    // Enable pin of Motor controller #1 for motor right #1 connected to PWM pin J14-07(SDO1/OC1/INT0/RD0)   Use TIMER_OC1
    
#define InMotorRight2Pin  31      // In pin of Motor controller #1 for motor right #2 connected to digital pin J9-07(PMD6/RE6)
#define EnableMotorRight2Pin 5    // Enable pin of Motor controller #1 for motor right #2 connected to PWM pin J14-11(OC2/RD1)   Use TIMER_OC2
    
#define InMotorLeft1Pin  32       // In pin of Motor controller #2 for motor left #1 connected to digital pin J9-06(PMD5/RE5)
#define EnableMotorLeft1Pin 6     // Enable pin of Motor controller #2 for motor left #1 connected to PWM pin J14-13(OC3/RD2)    Use TIMER_OC3
    
#define InMotorLeft2Pin  33       // In pin of Motor controller #2 for motor left #2 connected to digital pin J9-05(PMD4/RE4)
#define EnableMotorLeft2Pin 9     // Enable pin of Motor controller #2 for motor left #2 connected to PWM pin J3-03(OC4/RD3)    Use TIMER_OC4
    
#define EncoderTickRightINT  4   // INT used by the encoder for motor right connected to interrupt pin INT2 J4-02(AETXEN/SDA1/INT4/RA15)  Use INT4
#define EncoderTickRightPin 20
#define EncoderTickLeftINT   3   // INT used by the encoder for motor left  connected to interrupt pin INT3 J4-01(AETXCLK/SCL1/INT3/RA14) Use INT3
#define EncoderTickLeftPin  21

#define HSERVO_Pin  34   // Horizontal Servo pin connected to digital pin J9-04 (PMD3/RE3)
/* Power +5V */
/* Ground    */
#define VSERVO_Pin  35   // Vertical Servo pin connected to digital pin J9-03 (PMD2/RE2)
/* Power +5V */
/* Ground    */

#define IRSERVO_Pin 36   // IR Servo pin connected to digital pin J9-02 (PMD1/RE1)
/* Power +5V */
/* Ground    */


#define GP2Y0A21YK_Pin 54   // IR sensor GP2Y0A21YK analogic pin J5-01 A0 (PGED1/AN0/CN2/RB0)   Use ADC module channel 2
/* Analogic interface is provided on pin V0 */
/* Power +3V is set on pin VCC              */
/* Ground    is set on pin GND              */


void IntrTickRight();  // interrupt handler encoder right
void IntrTickLeft();   // interrupt handler encoder right

int motor_begin();     
/* Description: initialize everything, must be called during setup            */                                            
/* input:       none                                                          */
/* output:      return                                                        */                             
/*                  = SUCCESS always even if error during initialization      */ 
/* lib:         pinMode                                                       */                                
/*              GP2Y0A21YK_init                                               */                                
/*              Servo.attach                                                  */
/*              Servo.write                                                   */                                
/*              delay                                                         */ 
/*              TiltPan_begin                                                 */                               
/*              CMPS03.CMPS03_begin                                           */                           
/*              attachInterrupt                                               */ 
/*              interrupts                                                    */ 


int get_TickRight();
/* Description: get TickRight                                                 */                                            
/* input:       none                                                          */
/* output:      return                                                        */
/*                  = TickRight                                               */
/* lib:         non                                                           */

int get_TickLeft();
/* Description: get TickLeft                                                  */                                            
/* input:       none                                                          */
/* output:      return                                                        */
/*                  = TickLeft                                                */
/* lib:         none                                                          */

int get_SpeedMotorRight();
/* Description: get SpeedMotorRight                                           */                                            
/* input:       none                                                          */
/* output:      return                                                        */
/*                  = SpeedMotorRight                                          */
/* lib:         none                                                          */

int get_SpeedMotorLeft();
/* Description: get SpeedMotorLeft                                            */                                            
/* input:       none                                                          */
/* output:      return                                                        */
/*                  = SpeedMotorLeft                                          */
/* lib:         none                                                          */

void forward();     
/* Description: set IN1 and IN2 of the 4 motors in order to run clockwise     */                                            
/*              (refer truth table LM293D)                                    */
/* input:       none                                                          */
/* output:      none                                                          */
/* lib:         digitalWrite                                                  */

void forward_test(int num); 
/* Description: set IN1 and IN2 of the motor num in order to run clockwise    */                                            
/*              (refer truth table LM293D)                                    */
/*              used for testing                                              */
/* input:       num                                                           */ 
/*                  = 1: In1MotorRight1Pin                                    */ 
/*                  = 2: In1MotorRight2Pin                                    */ 
/*                  = 3: In1MotorLeft1Pin                                     */ 
/*                  = 4: In1MotorLeft2Pin                                     */                       
/* output:      none                                                          */
/* lib:         digitalWrite                                                  */

void start_forward();
/* Description: call forward +                                                */ 
/*              set enable pin of the 4 motors to SPEEDNOMINAL                */
/* input:       none                                                          */
/* output:      none                                                          */                       
/* lib:         forward                                                       */
/*              analogWrite                                                   */
                                   
void start_forward_test(int num);
/* Description: call forward +                                                */ 
/*              set enable pin of the motor num to SPEEDNOMINAL               */                                           
/*              (refer truth table LM293D)                                    */
/*              used for testing                                              */
/* input:       num                                                           */ 
/*                  = 1: In1MotorRight1Pin                                    */ 
/*                  = 2: In1MotorRight2Pin                                    */ 
/*                  = 3: In1MotorLeft1Pin                                     */ 
/*                  = 4: In1MotorLeft2Pin                                     */ 
/* output:      none                                                          */
/* lib:         forward                                                       */
/*              analogWrite                                                   */

void backward();
/* Description: set IN1 and IN2 of the 4 motors in order to run anti-clockwise*/                                            
/*              (refer truth table LM293D)                                    */
/* input:       none                                                          */
/* output:      none                                                          */
/* lib:         digitalWrite                                                  */

void start_backward();
/* Description: call backward +                                               */ 
/*              set enable pin of the 4 motors to SPEEDNOMINAL                */
/* input:       none                                                          */
/* output:      none                                                          */
/* lib:         backward                                                      */
/*              analogWrite                                                   */

void stop();
/* Description: set IN1 and IN2 of the 4 motors in order to stop              */
/*              (refer truth table LM293D)                                    */
/*              and reset enable pin of the 4 motors                          */
/* input:       none                                                          */
/* output:      none                                                          */
/* lib:         analogWrite                                                   */
/*              digitalWrite                                                  */


int accelerate (int motor);
/* Description: set enable pin of the corresponding motors to an higher value */
/*              (one increment)                                               */
/* input:       motor                                                         */ 
/*                  = LEFT_MOTOR                                              */ 
/*                  = RIGHT_MOTOR                                             */ 
/*                  = BOTH_MOTOR                                              */ 
/* output:      return                                                        */                            
/*                  = SPEED_ERROR if speed computed > SPEEDMAX                */ 
/*                  = SUCCESS otherwise                                       */ 
/* lib:         analogWrite                                                   */

int accelerate_n (int motor, int n);
/* Description: set enable pin of the corresponding motors to an higher value */
/*              (n increments)                                                */
/* input:       motor                                                         */ 
/*                  = LEFT_MOTOR                                              */ 
/*                  = RIGHT_MOTOR                                             */ 
/*                  = BOTH_MOTOR                                              */ 
/* input:       n                                                             */
/*                  = number of increments                                    */  
/* output:      return                                                        */                            
/*                  = return number of increments done                        */ 
/* lib:         accelerate                                                    */


int deccelerate(int motor);
/* Description: set enable pin of the corresponding motors to a lower value   */
/*              (one decrement)                                               */
/* input:       motor                                                         */ 
/*                  = LEFT_MOTOR                                              */ 
/*                  = RIGHT_MOTOR                                             */ 
/*                  = BOTH_MOTOR                                              */ 
/* output:      return                                                        */                            
/*                  = SPEED_ERROR if speed = 0                                */ 
/*                  = SUCCESS otherwise                                       */                             
/* lib:         analogWrite                                                   */
                                                                   
int deccelerate_n(int motor, int n);
/* Description: set enable pin of the corresponding motors to a lower value   */
/*              (n decrements)                                                */
/* input:       motor                                                         */ 
/*                  = LEFT_MOTOR                                              */ 
/*                  = RIGHT_MOTOR                                             */ 
/*                  = BOTH_MOTOR                                              */ 
/* input:       n                                                             */
/*                  = number of decrements                                    */  
/* output:      return                                                        */                            
/*                  = return number of decrements done                        */                                     
/* lib:         deccelerate                                                   */

 
 
int adjustMotor (int motor, int pid);
/* Description: Adjust the speed of the motor according the PID value         */
/* input:       motor                                                         */ 
/*                  = LEFT_MOTOR                                              */ 
/*                  = RIGHT_MOTOR                                             */ 
/* input:       pid                                                           */
/*                  = pid value to adjust                                     */  
/* output:      return                                                        */                            
/*                  = SPEED_ERROR if speed computed > SPEEDMAX                */ 
/*                  = SUCCESS otherwise                                       */                                        
/* lib:         analogWrite                                                   */                                        
                                                                      
                                     
int go(int d, int pid_ind); 
/* Description: go during d encoder ticks or before in case of obstacle       */
/*              detected by IR sensor.                                        */
/*              if pid_ind = 1 then use a PID method (calling adjustMotor)    */
/*              to control motors speed between left and right                */                  
/* input:       d                                                             */ 
/*                  = number of right+left ticks to go                        */ 
/* input:       pid_ind                                                       */
/*                  = 0: disable PID adjustement                              */
/*                  = 1: enable PID adjustement                               */    
/* output:      return                                                        */                            
/*                  = SPEED_ERROR if speed computed > SPEEDMAX                */
/*                  = OBSTACLE if an obstacle is detected before DISTANCE_MIN */
/*                  = SUCCESS otherwise                                       */                                          
/* lib:         computePID                                                    */                                
/*              adjustMotor                                                   */                                
/*              GP2Y0A21YK_getDistanceCentimeter                              */                                      
                                       
                                       
int check_around();
/* Description: move the servo used by the IR sensor in order to determine    */
/*              the direction without obstacle                                */
/* input:       none                                                          */   
/* output:      return                                                        */                            
/*                  = OBSTACLE if an obstacle is detected for both directions */
/*                  = LEFT_DIRECTION if best direction to go                  */ 
/*                  = RIGHT_DIRECTION if best direction to go                 */                      
/* lib:         Servo.write                                                   */                                                              
/*              delay                                                         */
/*              adjustMotor                                                   */                                
/*              GP2Y0A21YK_getDistanceCentimeter                              */  


                      
int turn(double alpha, unsigned long timeout);
/* Description: turns with an angle of alpha degrees before a delay (timeout) */
/*              using a compass to get the direction                          */
/* input:       alpha                                                         */ 
/*                  = angle to turn (-180 < alplha < +180) and alpha <> 0     */
/*              timeout                                                       */ 
/*                  = timeout in ms                                           */      
/* output:      return                                                        */                            
/*                  = BAD_ANGLE if not (-180 < alplha < +180) and alpha <> 0  */
/*                  = COMPASS_ERROR if an error occurs with the compass       */
/*                  = TIMEOUT if turn is not completed before the delay       */
/*                  = SUCCESS otherwise                                       */  
/* lib:         CMPS03_read()                                                 */                                
/*              accelerate_n                                                  */                                
/*              deccelerate_n                                                 */
/*              millis                                                        */                                

#endif
