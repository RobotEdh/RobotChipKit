/*
  motor.h - Library for motor control
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/


#ifndef MOTOR_h
#define MOTOR_h

#include <wiring.h> // used for analogic read function (core lib)
#include <PID.h>


#define CMD_START   0x01
#define CMD_STOP    0x02
#define CMD_INFOS   0x03
#define CMD_PICTURE 0x04

#define RESP_SIZE 8
#define RESP_INFOS 0x01

#define STATE_STOP 0x00
#define STATE_GO   0x01

#define SPEEDNOMINAL 150
#define SPEEDMAX 255     //(255=PWM max)
#define SPEEDDELTA 50     

#define CENTER_DIRECTION 0
#define LEFT_DIRECTION 1
#define RIGHT_DIRECTION 2

#define OBSTACLE -2
#define TIMEOUT -3

#define BOTH_MOTOR 0
#define LEFT_MOTOR 1
#define RIGHT_MOTOR 2

#define DISTANCE_MIN 50 // 50 cm before stopping, must be > 20cm which is lower range of IR sensor

#define In1MotorRight1Pin  26     // In1 pin of L293D #1 for motor right #1 connected to digital pin J6-02(PMD0/RE0)
#define In2MotorRight1Pin  27     // In2 pin of L293D #1 for motor right #1 connected to digital pin J6-04(PMD1/RE1)
#define EnableMotorRight1Pin 3    // Enable pin of L293D #1 for motor right #1 connected to PWM pin J6-07(OC1/RD0)   Use TIMER_OC1
    
#define In1MotorRight2Pin  28     // In1 pin of L293D #2 for motor right #2 connected to digital pin J6-06(PMD2/RE2)
#define In2MotorRight2Pin  29     // In2 pin of L293D #2 for motor right #2 connected to digital pin J6-08(PMD3/RE3)
#define EnableMotorRight2Pin 5    // Enable pin of L293D #2 for motor right #2 connected to PWM pin J6-11(OC2/RD1)   Use TIMER_OC2
    
#define In1MotorLeft1Pin  30      // In1 pin of L293D #1 for motor left #1 connected to digital pin J6-10(PMD4/RE4)
#define In2MotorLeft1Pin  31      // In2 pin of L293D #1 for motor left #1 connected to digital pin J6-12(PMD5/RE5)
#define EnableMotorLeft1Pin 6     // Enable pin of L293D #1 for motor left #1 connected to PWM pin J6-13(OC3/RD2)    Use TIMER_OC3
    
#define In1MotorLeft2Pin  32      // In1 pin of L293D #2 for motor left #2 connected to digital pin J6-14(PMD6/RE6)
#define In2MotorLeft2Pin  33      // In2 pin of L293D #2 for motor left #2 connected to digital pin J6-16(PMD7/RE7)
#define EnableMotorLeft2Pin 9     // Enable pin of L293D #2 for motor left #2 connected to PWM pin J5-03(OC4/RD3)    Use TIMER_OC4
    
#define EncodeurTickRightINT  1  // INT used by the encodeur for motor right connected to interrupt pin INT1 J6-05(INT1) Use INT1
#define EncodeurTickLeftINT   2  // INT used by the encodeur for motor left  connected to interrupt pin INT2 J6-15(INT2)  Use INT2


#define GP2Y0A21YK_Pin 14   // IR sensor GP2Y0A21YK analogic pin J7-01 A0 (C2IN-/AN2/SS1/CN4/RB2)   Use ADC module channel 2
/* Analogic interface is provided on pin V0 */
/* Power +3V is set on pin VCC              */
/* Ground    is set on pin GND              */

#define SERVO_Pin  34   // Servo pin connected to digital pin J5-02 (PMRD/CN14/RD5)
/* Power +5V */
/* Ground    */


/* Compas                                                           */
/* I2C interface is provided on pins:                               */
/*         1 = Power +5V                                            */                     
/*         2 = SCL connected to A5/J7-11(SCL1/RG2) selected by JP8  */
/*         3 = SDA connected to A4/J7-09(SDA1/RG3) selected by JP6  */
/*         9 = Ground                                               */


#define SS_CS_Pin  4   // Serial Select pin connected to digital pin J6-09 (RF1)
/* SD-Card                                                                                          */
/* SPI interface is provided on pins:                                                               */                    
/*         SPI/SD-Card Master Output (MOSI) connected to J5-07(SDO2/PMA3/CN10/RG8) selected by JP5  */
/*         SPI/SD-Card Master Input  (MISO) connected to J5-09(SDI2/PMA5/CN8/RG7) selected by JP7   */
/*         SPI/SD-Card Serial Clock  (SCLK) connected to J5-11 (SCK2/PMA5/CN8/RG6)                  */                                            


int motor_init();     // initialize everything
int mainRobot ();
void IntrTickRight();  // interrupt handler encodeur right
void IntrTickLeft();   // interrupt handler encodeur right

void forward(); // set IN1 and IN2 of the 4 motors in order to run clockwise (refer truth table LM293D)
void forward_test(int num); // set IN1 and IN2 of the motor num , used for testing
void backward(); // set IN1 and IN2 of the 4 motors in order to run anti-clockwise (refer truth table LM293D)

void start_forward(); // call forward + set enable pin of the 4 motors to SPEEDNOMINAL
void start_forward_test(int num); // call forward (num) + set enable pin of the motor num to SPEEDNOMINAL, used for testing
void start_backward(); // call backward + set enable pin of the 4 motors to SPEEDNOMINAL

void stop(); // set IN1 and IN2 of the 4 motors in order to stop (refer truth table LM293D) and reset enable pin of the 4 motors 

int accelerate (int motor); // set enable pin of the corresponding motors to an higher value (one increment)
                            // motor = LEFT_MOTOR, RIGHT_MOTOR or BOTH_MOTOR
                            // return -1 if already SPEEDMAX
int deccelerate(int motor); // set enable pin of the corresponding motors to an lowest value (one decrement)
                            // motor = LEFT_MOTOR, RIGHT_MOTOR or BOTH_MOTOR
                            // return -1 if already SPEEDMAX
                            
int accelerate_n (int motor, int n); // set enable pin of the corresponding motors to an higher value (n increments)
                                     // motor = LEFT_MOTOR, RIGHT_MOTOR or BOTH_MOTOR
                                     // return -1 if already SPEEDMAX
int deccelerate_n(int motor, int n); // set enable pin of the corresponding motors to an lowest value (n decrements)
                                     // motor = LEFT_MOTOR, RIGHT_MOTOR or BOTH_MOTOR
                                     // return -1 if already SPEEDMAX
                                     
int go(int d, int pid_ind);           // go during d ticks using PID if pid_ind = 1
                                      // Control motors speed between left and right using a tick counter provided by an encoder
                                      // adjust (calling adjustMotor) the motor speed if necessary using a PID method
                                      // return -1 if cant adjust motor speed
                                      // return -2 is something is detected by IR sensor at less than DISTANCE_MIN
                                      // else returns direction using a compass
int adjustMotor (int motor, int pid); // function called by the Go function in order to adjust
                                       // motor = LEFT_MOTOR, RIGHT_MOTOR or BOTH_MOTOR
                                       // with the value PID computed by the PID library
                                       // return -1 if cant adjust motor speed
int check_around();   // move the servo used by the IR sensor in order to determine the direction without obstacle
                      // returns the direction to go
int turn(double alpha); // turns with an angle of alpha degrees (-180 < alplha < +180) and alpha <> 0

int makePicture (int n);

int sendPicture (int n);

#endif
