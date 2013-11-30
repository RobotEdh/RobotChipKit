#include "WProgram.h"

#include "config.h"
#include "def.h"
#include "types.h"
#include "PMultiWii.h"

uint8_t PWM_PIN[4] = {3,5,6,9};      // OC1, 0C2, OC3, OC4 = rear, right, left, front   


/**************************************************************************************/
/************  Writes the Motors values to the PWM compare register  ******************/
/**************************************************************************************/
void writeMotors() { // [936;1875] => [312;625]
  int i=0;
  
#if defined(TRACE)
  Serial.println(">writeMotors");
    
  for (i = 0; i < 4; i++) { 
        Serial.print("rcData[");Serial.print(i);Serial.print("]:");Serial.println(rcData[i]);
        Serial.print("rcCommand[");Serial.print(i);Serial.print("]:");Serial.println(rcCommand[i]); 
  }
 
  Serial.print("axisPID[ROLL]:");Serial.println(axisPID[ROLL]);
  Serial.print("axisPID[PITCH]:");Serial.println(axisPID[PITCH]);
  Serial.print("axisPID[YAW]:");Serial.println(axisPID[YAW]);
   
  for (i = 0; i < 4; i++) { 
        Serial.print("motor[");Serial.print(i);Serial.print("]:");Serial.println(motor[i]);
  }
#endif
   
  OC1RS = motor[0]/3;
  OC2RS = motor[1]/3; 
  OC3RS = motor[2]/3; 
  OC4RS = motor[3]/3;  
}

/**************************************************************************************/
/************          Writes the mincommand to all Motors           ******************/
/**************************************************************************************/
void writeAllMotors(int16_t mc) {   // Sends commands to all motors
  for (uint8_t i =0;i<4;i++) {
    motor[i]=mc;
  }
  writeMotors();
}

/**************************************************************************************/
/************        Initialize the PWM Timers and Registers         ******************/
/**************************************************************************************/
void initOutput() {
#if defined(TRACE)
  Serial.println(">>Start initOutput");
#endif

  for(uint8_t i=0;i<4;i++) {
    pinMode(PWM_PIN[i],OUTPUT);
  }
    
  // Initialization TIMER2
  T2CON = 0x0070 ;  // 0x0070=0000000001110000 
  				    // ON=0 (Timer is disable for now)
                    // FRZ=0 (Continue operation even when CPU is in Debug Exception mode)
                    // SIDL=0 (Continue operation even in Idle mode)
                    // TGATE=0, (Gated time accumulation is disabled) 
                    // TCKPS=111 (1:256 prescale value) CPU clock = 80 MHz => Period : 256/80Mhz = 256/80 000 000 = 3,2 us
                    // T32=0 (TMRx and TMRy form separate 16-bit timer)
                    // TCS=0 (Internal peripheral clock) 
                                                      
  TMR2 = 0;    // start TIMER2 from 0...
  PR2 = 6249;  // ...	until 6249 => TIMER2 Period = (6249+1)* 3,2 us = 20 ms = 50Hz
    
  // The ESC generally accepts a nominal 50 Hz PWM servo input signal whose pulse width varies from 1ms to 2ms.
  // When supplied with a 1 ms width pulse at 50Hz(6250/0.02s)*0.001s = 312.5, the ESC responds by turning off the DC motor attached to its output.
  // A 1.5ms (6250/0.02s)*0.0015s = 468.75 pulse-width input signal results in a 50% duty cycle output signal that drives the motor at approximately half-speed.
  // When presented with 2.0ms (6250/0.02s)*0.002s = 625 input signal, the motor runs at full speed due to the 100% duty cycle (on constantly) output.
       
  // Configure the control register OC1CON for the output compare channel 1
  OC1CON = 0; // clear OC1
  OC1CONbits.OCM = 0b110; // OCM=110: PWM mode on OC1, Fault pin disable
  // Configure the compare register OC1R and compare register secondary OC1RS for the output compare channel 1
  OC1RS = MINCOMMAND/3; // set buffered PWM duty cycle in counts,
               // duty cycle is OC1RS/(PR2+1)
  OC1R = MINCOMMAND/3;  // set initial PWM duty cycle in counts
  
  // Configure the control register OC2CON for the output compare channel 2
  OC2CON = 0; // clear OC2
  OC2CONbits.OCM = 0b110; // OCM=110: PWM mode on OC2, Fault pin disable
  // Configure the compare register OC2R and compare register secondary OC1RS for the output compare channel 2
  OC2RS = MINCOMMAND/3; // set buffered PWM duty cycle in counts,
               // duty cycle is OC2RS/(PR2+1)
  OC2R = MINCOMMAND/3;  // set initial PWM duty cycle in counts
  
  // Configure the control register OC3CON for the output compare channel 3
  OC3CON = 0; // clear OC3
  OC3CONbits.OCM = 0b110; // OCM=110: PWM mode on OC3, Fault pin disable
  // Configure the compare register OC3R and compare register secondary OC1RS for the output compare channel 3
  OC3RS = MINCOMMAND/3; // set buffered PWM duty cycle in counts,
                // duty cycle is OC3RS/(PR2+1)
  OC3R = MINCOMMAND/3;  // set initial PWM duty cycle in counts
  
  // Configure the control register OC4CON for the output compare channel 4
  OC4CON = 0; // clear OC4
  OC4CONbits.OCM = 0b110; // OCM=110: PWM mode on OC4, Fault pin disable
  // Configure the compare register OC4R and compare register secondary OC4RS for the output compare channel 4
  OC4RS = MINCOMMAND/3; // set buffered PWM duty cycle in counts,
                // duty cycle is OC4RS/(PR2+1)
  OC4R = MINCOMMAND/3;  // set initial PWM duty cycle in counts
                
  // Enable Timer 2 and OCX              
  T2CONSET =  0x8000; // Enable Timer2
  OC1CONSET = 0x8000; // Enable OC1
  OC2CONSET = 0x8000; // Enable OC2  
  OC3CONSET = 0x8000; // Enable OC3  
  OC4CONSET = 0x8000; // Enable OC4                     
   
  writeAllMotors(MINCOMMAND);
  delay(300);

#if defined(TRACE)
  Serial.println("<<End initOutput");
#endif 
}

/*******************************************************************************/
/*                    RunMotors                                                */
/*  - Compute motor[i] using rcCommand[THROTTLE] and PID coeff                 */
/*                                                                             */
/*  - call writeMotors to run motors using motor[i]                            */                                                 
/*******************************************************************************/

void RunMotors() {
  int16_t maxMotor;
  uint8_t i;
  #define PIDMIX(X,Y,Z) rcCommand[THROTTLE] + axisPID[ROLL]*X + axisPID[PITCH]*Y + YAW_DIRECTION * axisPID[YAW]*Z

    motor[0] = PIDMIX(-1,+1,-1); //REAR_R
    motor[1] = PIDMIX(-1,-1,+1); //FRONT_R
    motor[2] = PIDMIX(+1,+1,+1); //REAR_L
    motor[3] = PIDMIX(+1,-1,-1); //FRONT_L

    maxMotor=motor[0];
    for(i=1; i< 4; i++)
      if (motor[i]>maxMotor) maxMotor=motor[i];
    
    for(i=0; i< 4; i++) {
      if (maxMotor > MAXCOMMAND) // this is a way to still have good gyro corrections if at least one motor reaches its max.
        motor[i] -= maxMotor - MAXCOMMAND;
    
      motor[i] = constrain(motor[i], MINCOMMAND, MAXCOMMAND);
 
      if (rcData[THROTTLE] < MINCHECK) // stop requested
          motor[i] = MINCOMMAND;
    }
    
    writeMotors();
}
