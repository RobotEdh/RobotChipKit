/*
  motor.h - Library for motor control
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/


#ifndef MOTOR_h
#define MOTOR_h

#include <Arduino.h> // used for pin definition
#include <PID.h>

#define SUCCESS 0
#define SPEED_ERROR -1
#define OBSTACLE -2
#define OBSTACLE_LEFT -3
#define OBSTACLE_RIGHT -4
#define TIMEOUT -5
#define BAD_ANGLE -6
#define COMPASS_ERROR -7

#define STATE_STOP 0
#define STATE_GO   1

#define SPEEDMAX     255     // 255=PWM max
#define SPEEDNOMINAL 100     // speed at start
#define SPEEDTURN     80     // speed at turn 
#define SPEEDBACK     50     // speed at turn back

#define DIRECTION_LEFT  1
#define DIRECTION_RIGHT 2

#define BOTH_MOTOR  0
#define LEFT_MOTOR  1
#define RIGHT_MOTOR 2

#define DISTANCE_MIN 50 // 50 cm before stopping, must be > 20cm which is lower range of IR sensor and
                        // must be > 30cm which is the distance run by the motor in 1 second between 2 checks

#define InMotorRight1Pin    30    // In pin of Motor controller #1 for motor right #1 connected to digital pin 30
#define EnableMotorRight1Pin 4    // Enable pin of Motor controller #1 for motor right #1 connected to PWM pin 4
    
#define InMotorRight2Pin    31    // In pin of Motor controller #1 for motor right #2 connected to digital pin 31
#define EnableMotorRight2Pin 5    // Enable pin of Motor controller #1 for motor right #2 connected to PWM pin 5
    
#define InMotorLeft1Pin    33     // In pin of Motor controller #2 for motor left #1 connected to digital pin 33
#define EnableMotorLeft1Pin 7     // Enable pin of Motor controller #2 for motor left #1 connected to PWM pin 7
    
#define InMotorLeft2Pin    32     // In pin of Motor controller #2 for motor left #2 connected to digital pin 32
#define EnableMotorLeft2Pin 6     // Enable pin of Motor controller #2 for motor left #2 connected to PWM pin 6

#ifdef PID     
#define EncoderTickRightINT  4   // INT used by the encoder for motor right connected to interrupt pin INT4 J4-02(AETXEN/SDA1/INT4/RA15)  Use INT4
#define EncoderTickRightPin 20
#define EncoderTickLeftINT   3   // INT used by the encoder for motor left  connected to interrupt pin INT3 J4-01(AETXCLK/SCL1/INT3/RA14) Use INT3
#define EncoderTickLeftPin  21
#endif

#define IRSERVO_Pin    46        // IR Servo pin connected to digital PWM 46
#define GP2Y0A21YK_PIN A1        // IR sensor GP2Y0A21YK analogic pin A1

#define ContactLeftPin  26   // Contact sensor Left pin connected to digital 26
#define ContactRightPin 27   // Contact sensor Right pin connected to digital 27

#ifdef PID
void IntrTickRight();  // interrupt handler encoder right
void IntrTickLeft();   // interrupt handler encoder right
#endif

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
/*              CMPS03.CMPS03_init                                            */                           
/*              attachInterrupt                                               */ 
/*              interrupts                                                    */ 
/*              stop                                                          */

#ifdef PID
int get_TickRight();
/* Description: get TickRight                                                 */                                            
/* input:       none                                                          */
/* output:      return                                                        */
/*                  = TickRight                                               */
/* lib:         none                                                          */

int get_TickLeft();
/* Description: get TickLeft                                                  */                                            
/* input:       none                                                          */
/* output:      return                                                        */
/*                  = TickLeft                                                */
/* lib:         none                                                          */

void reset_TickRight();
/* Description: reset TickRight                                               */                                            
/* input:       none                                                          */
/* output:      none                                                          */
/* lib:         none                                                          */

void reset_TickLeft();
/* Description: reset TickLeft                                                */                                            
/* input:       none                                                          */
/* output:      none                                                          */
/* lib:         none                                                          */
#endif

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

void forward(int motor);     
/* Description: set IN1 and IN2 of the corresponding motors                   */
/* in order to run clockwise                                                  */                                            
/*              (refer truth table LM293D)                                    */
/* input:       motor                                                         */ 
/*                  = LEFT_MOTOR                                              */ 
/*                  = RIGHT_MOTOR                                             */ 
/*                  = BOTH_MOTOR                                              */ 
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

void backward(int motor);
/* Description: set IN1 and IN2 of the corresponding motors                   */
/* in order to run anti-clockwise                                             */                                            
/*              (refer truth table LM293D)                                    */
/* input:       motor                                                         */ 
/*                  = LEFT_MOTOR                                              */ 
/*                  = RIGHT_MOTOR                                             */ 
/*                  = BOTH_MOTOR                                              */ 
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

void change_speed(int speed);
/* Description: change the speed of the 4 motors to the corresponding value   */
/* input:       speed                                                         */  
/* output:      return                                                        */                            
/*                  = return number of decrements done                        */                                     
/* lib:         analogWrite                                                   */ 
 
int adjustMotor (int motor);
/* Description: Adjust the speed of the motor according the PID value         */
/* input:       motor                                                         */ 
/*                  = LEFT_MOTOR                                              */ 
/*                  = RIGHT_MOTOR                                             */ 
/* input:       pid                                                           */
/*                  = pid value to adjust                                     */  
/* output:      none                                                          */                                       
/* lib:         analogWrite                                                   */                                        
                                                                      
                                     
int go(unsigned long timeout); 
/* Description: go during timeout seconds or before in case of obstacle       */
/*              detected by IR sensor.                                        */
/*              if pid_ind = 1 then use a PID method (calling adjustMotor)    */
/*              to control motors speed between left and right                */                  
/* input:       timeout                                                       */ 
/*                  = timeout in seconds                                      */   
/* output:      return                                                        */                            
/*                  = SPEED_ERROR if speed computed > SPEEDMAX                */
/*                  = OBSTACLE_LEFT or OBSTACLE_RIGHT if an obstacle is hit   */
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
/*                  = OBSTACLE_LEFT if an obstacle is detected on left side   */
/*                  = OBSTACLE_RIGHT if an obstacle is detected on right side */
/*                  = DIRECTION_LEFT if best direction to go is left side     */ 
/*                  = DIRECTION_RIGHT if best direction to go is right side   */                      
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
/*                  = timeout in seconds                                      */      
/* output:      return                                                        */                            
/*                  = BAD_ANGLE if not (-180 < alplha < +180) and alpha <> 0  */
/*                  = COMPASS_ERROR if an error occurs with the compass       */
/*                  = TIMEOUT if turn is not completed before the delay       */
/*                  = SUCCESS otherwise                                       */  
/* lib:         CMPS03_read()                                                 */                                
/*              accelerate_n                                                  */                                
/*              deccelerate_n                                                 */
/*              millis                                                        */                                

int turnback(unsigned long timeout);
/* Description: turns back before a delay (timeout)                           */
/* input:       timeout                                                       */ 
/*                  = timeout in seconds                                      */      
/* output:      return                                                        */ 
/*                  = turn function error on case of error returned by turn() */
/*                  = TIMEOUT if turn back is not completed before the delay  */
/*                  = SUCCESS otherwise                                       */  
/* lib:         backward()                                                    */
/*              turn()                                                        */
/*              millis                                                        */ 
#endif
