#include "WProgram.h"

#include "config.h"
#include "def.h"
#include "types.h"
#include "PMultiWii.h"
#include "PIMU.h"
#include "POutput.h"
#include "PRX.h"
#include "PSensors.h"



uint32_t currentTime = 0;
uint16_t calibratingA = 0;  // the calibration is done in the main loop. Calibrating decreases at each cycle down to 0, then we enter in a normal mode.
uint16_t calibratingG = 0;

int16_t PTerm = 0,ITerm = 0,DTerm = 0;
static int16_t last_error[3] = {0,0,0};
static int16_t sum_error[3] = {0,0,0};
int16_t Ki[3] = {0,0,0};
int16_t Kp[3] = {0,0,0};
int16_t Kd[3] = {0,0,0};
int16_t gyroZero[3] = {0,0,0};
int16_t accZero[3] = {0,0,0};
imu_t imu;

int16_t rcData[RC_CHANS];    // interval [1000;2000]
int16_t rcSerial[8];         // interval [1000;2000] - is rcData coming from MSP
int16_t rcCommand[4];        // interval [1000;2000] for THROTTLE and [-500;+500] for ROLL/PITCH/YAW
uint8_t rcSerialCount = 0;   // a counter to select legacy RX when there is no more MSP rc serial data


int16_t axisPID[3];
int16_t motor[8];
double c_angle[2];



void MultiWii_setup() {
    
  Serial.begin(9600); // initialize serial 
  
#if defined(TRACE)
  Serial.println("Start MultiWii_setup");
#endif 
  
  initOutput();
       
  configureReceiver();

  initSensors();

#if defined(TRACE)
  Serial.println("End MultiWii_setup");
#endif 
  
/* START TESTCASE 1 CODE: spin up each blade individually for 10s each and check they all turn the right way  */
  for(int i=0; i< 4; i++)
  {
      writeOneMotor(i, MINCOMMAND);
      delay(10*1000);
  }   
/* END TESTCASE 1 CODE */


/* START TESTCASE 2 CODE: Spin all the motors together for 10s judging how much lift is provided  */
  writeAllMotors(MINCOMMAND);
  delay(10*1000);  
/* END TESTCASE 2 CODE */

/* CALIBRATE */  
  calibratingG=512; 
  calibratingA=512; 

  while (calibratingG > 0 || calibratingA > 0)
  { 
    computeIMU();
  }
  
}


// ******** Main Loop *********

void MultiWii_loop () {
 
  uint8_t axis;
  int16_t error= 0;
  int16_t PTerm = 0,ITerm = 0,DTerm;
  static uint32_t rcTime  = 0;
  
  
  if (currentTime > rcTime ) { // 50Hz: PPM frequency of the RC, no change happen within 20ms
    rcTime = currentTime + 20000;
    
    computeRC();
  }

#if !defined(IMU)   
  computeIMU();
  //**** PITCH & ROLL & YAW PID ****
   
  // PITCH & ROLL
  for(axis=0;axis<2;axis++) {
  	
  	//error
    error =  rcCommand[axis] - (int16_t)(c_angle[axis]*159.0); // convert c_angle from -pi;+pi to -500;+500
#if defined(TRACE)  
    Serial.print("c_angle[");Serial.print((int)axis);Serial.print("]:");Serial.println(c_angle[axis]);
    Serial.print("rcCommand[");Serial.print((int)axis);Serial.print("]:");Serial.println(rcCommand[axis]);
    Serial.print("error:");Serial.println(error);
#endif   

    //I term
    sum_error[axis] += error;
    ITerm = Ki[axis]*sum_error[axis];
    

    //P term
    PTerm = Kp[axis]*error;
        
    //D term
    DTerm = Kd[axis]*(error - last_error[axis]); 
    last_error[axis] = error;

    //PID
    axisPID[axis] =  PTerm + ITerm - DTerm;
    
#if defined(TRACE)  
    Serial.print("axisPID[");Serial.print((int)axis);Serial.print("]:");Serial.println(axisPID[axis]);
#endif     
  }

  //YAW
    axisPID[YAW] = 0;
  
#if defined(TRACE)  
    Serial.print("axisPID[");Serial.print((int)YAW);Serial.print("]:");Serial.println(axisPID[YAW]);
#endif

#else
  axisPID[PITCH] = 0;
  axisPID[ROLL] = 0;
  axisPID[YAW] = 0;
#endif   

  RunMotors();
}
