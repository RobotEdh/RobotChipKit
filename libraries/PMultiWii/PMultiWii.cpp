#include "WProgram.h"

#include "config.h"
#include "def.h"
#include "types.h"
#include "PMultiWii.h"
#include "POutput.h"
#include "PRX.h"
#include "PSensors.h"


uint32_t currentTime = 0;
uint32_t previousTime = 0;
uint32_t cycleTime = 0;
uint16_t calibratingA = 0;  
uint16_t calibratingG = 0;

static int16_t last_error[3] = {0,0,0};
static int32_t sum_error[3] = {0,0,0};
double Kp[3] = {0.6,0.6,0.6};
double Ki[3] = {0.1,0.1,0.1};
double Kd[3] = {0.3,0.3,0.3};
int16_t gyroZero[3] = {0,0,0};
int16_t accZero[3] = {0,0,0};
imu_t imu;

int16_t rcData[RC_CHANS];    // interval [1000;2000]
int16_t rcSerial[8];         // interval [1000;2000] - is rcData coming from MSP
int16_t rcCommand[4];        // interval [1000;2000] for THROTTLE and [-500;+500] for ROLL/PITCH/YAW
uint8_t rcSerialCount = 0;   // a counter to select legacy RX when there is no more MSP rc serial data


double axisPID[3];
double c_angle[2];

char* sz_blade[] = {"REAR_RIGHT","FRONT_RIGHT","REAR_LEFT","FRONT_LEFT"};
char* sz_axis[] = {"ROLL","PITCH","YAW","THROTTLE","AUX1"};

bool MultiWii_setup() {
    
  Serial.begin(9600); // initialize serial 
  
#if defined(TRACE)
  Serial.println("********************* INIT ***********************");
#endif
  
#if defined(TRACE)
  Serial.println(">Start Init");
#endif 
  
  initOutput();
       
  configureReceiver();

  if (!initSensors())
  {   
#if defined(TRACE)	  
   Serial.println("<End KO initSensors");
#endif    
    return false;
  }

#if defined(TRACE)
  Serial.println("<End OK Init");
#endif 

#if defined(TRACE)
  Serial.println(">Start Init ESC");
  Serial.println(">10 s to connect the ESC to power");
#endif   
  
  writeAllMotors(MINCOMMAND);
  delay(10*1000); /* 10 s to connect the ESC to power */
  writeAllMotors((MINCOMMAND+ MAXCOMMAND)/2);
  delay(5*1000);
  writeAllMotors(MINCOMMAND);
  
#if defined(TRACE)
  Serial.println("<End Init ESC");
#endif  

/* START TESTCASE 1: spin up each blade individually for 10s each and check they all turn the right way  */
#if defined(TRACE)
  Serial.println("START TESTCASE 1: spin up each blade individually for 10s each and check they all turn the right way");
#endif   

  for(int i=0; i< 4; i++)
  {
#if defined(TRACE)
      Serial.println(sz_blade[i]);
#endif 
      writeOneMotor(i, (MINCOMMAND+ MAXCOMMAND)/2);
      delay(5*1000);
  }
  writeAllMotors(MINCOMMAND);
  
#if defined(TRACE)
  Serial.println("END TESTCASE 1");
#endif     
/* END TESTCASE 1 */


/* START TESTCASE 2: Spin all the motors together for 5s judging how much lift is provided  */
#if defined(TRACE)
  Serial.println("START TESTCASE 2: Spin all the motors together for 5s judging how much lift is provided");
#endif  

  writeAllMotors((MINCOMMAND+ MAXCOMMAND)/2);
  delay(5*1000); 
  writeAllMotors(MINCOMMAND);

#if defined(TRACE)
  Serial.println("END TESTCASE 2");
#endif   
/* END TESTCASE 2 */

/* CALIBRATE */  
#if defined(TRACE)
  Serial.println("Start Calibrate MPU");
#endif 

  calibratingG=512; 
  calibratingA=512; 

  while (calibratingG > 0 || calibratingA > 0)
  { 
     ACC_getADC();
  
     Gyro_getADC();
  }

#if defined(TRACE)
  Serial.println("End Calibrate MPU");
#endif  

  for(int k=3; k< 6; k++)
  {
#if defined(TRACE)
     Serial.print("START TESTCASE ");Serial.print(k);Serial.println(": move tick for 5s to compute rcCommand");
#endif 
     delay(10*1000);
     for(int j=0; j< 5; j++)
     {  
       computeRC();
       delay(20); 
     }
     // ROLL & PITCH
     for(int i=0;i<2;i++)
     {
#if defined(TRACE)
       Serial.print("rcCommand[");Serial.print(sz_axis[i]);Serial.print("]:");Serial.println(rcCommand[i]);
#endif        
     }
#if defined(TRACE)
     Serial.print("END TESTCASE ");Serial.println(k);
#endif  
  }


#if defined(TRACE)  
  Serial.println("********************* FLY ***********************");
#endif  

  writeAllMotors(MINCOMMAND);

  return true;
}


// ******** Main Loop *********

void MultiWii_loop () {
 
  uint8_t axis;
  int16_t error = 0;
  int16_t delta_error = 0;
  int16_t PTerm = 0, ITerm = 0, DTerm = 0;
  static uint32_t rcTime  = 0;
  
  
  if ((currentTime > rcTime )|| (rcTime  == 0)) { // 50Hz: PPM frequency of the RC, no change happen within 20ms except first time
    rcTime = currentTime + 20000;
    
    computeRC();
  }
  currentTime = micros();
  cycleTime = currentTime - previousTime;
  previousTime = currentTime ;
  //**** Read IMU ****   
  ACC_getADC();
  
  Gyro_getADC();
  //**** ROLL & PITCH & YAW PID ****
   
  // ROLL & PITCH
  for(axis=0;axis<2;axis++) {
  	
    error =  rcCommand[axis] - (int16_t)(c_angle[axis]*159.0); // convert c_angle from -pi;+pi to -500;+500
    sum_error[axis] += (int32_t) (error * cycleTime);
    
    delta_error = (error - last_error[axis])/cycleTime;
    delta_error = ((int32_t) delta_error * ((uint16_t)0xFFFF / (cycleTime>>4)))>>6;
    
    axisPID[axis] =  (Kp[axis]*error) + (Ki[axis]*sum_error[axis]) + (Kd[axis]*delta_error);
    
    last_error[axis] = error;

#if defined(TRACE)  
    Serial.print(">MultiWii_loop: c_angle[");Serial.print(sz_axis[axis]);Serial.print("]:");Serial.println(c_angle[axis]);
    Serial.print(">MultiWii_loop: rcCommand[");Serial.print(sz_axis[axis]);Serial.print("]:");Serial.println(rcCommand[axis]);
    Serial.print(">MultiWii_loop: sum_error[");Serial.print(sz_axis[axis]);Serial.print("]:");Serial.println(sum_error[axis]);
    Serial.print(">MultiWii_loop: error:");Serial.println(error);
    Serial.print(">MultiWii_loop: axisPID[");Serial.print(sz_axis[axis]);Serial.print("]:");Serial.println(axisPID[axis]);
 #endif     
  }

  //YAW
  axisPID[YAW] = 0;
  
#if defined(TRACE5)  
  Serial.print(">MultiWii_loop: axisPID[");Serial.print((int)YAW);Serial.print("]:");Serial.println(axisPID[YAW]);
#endif

  RunMotors();
}
