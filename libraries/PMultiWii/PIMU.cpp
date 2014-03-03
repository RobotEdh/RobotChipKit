#include "WProgram.h"

#include "config.h"
#include "def.h"
#include "types.h"
#include "PMultiWii.h"
#include "PIMU.h"
#include "PSensors.h"


/****************************************************************/
/*                    computeIMU                                */
/*  - call ACC_getADC  to get:                                  */
/*                            imu.accADC[axis]                  */
/*  - call Gyro_getADC 2times and take a weighted value to get: */
/*                            imu.gyroADC[axis]                 */
/****************************************************************/

void computeIMU () {
  uint8_t axis;
  static int16_t gyroADCprevious[3] = {0,0,0};
  int16_t gyroADCp[3];
  int16_t gyroADCinter[3];
  static uint32_t timeInterleave = 0;
  
#if defined(TRACE)
  Serial.println(">>Start computeIMU");
#endif  

  ACC_getADC();
  
  Gyro_getADC();
    
  for (axis = 0; axis < 3; axis++)
      gyroADCp[axis] =  imu.gyroADC[axis];
  
  timeInterleave=micros();
   
  while((uint16_t)(micros()-timeInterleave)<650) ; //empirical, interleaving delay 650 micros between 2 consecutive reads
                                                   // we could use interrupt or FIFO register to known 
                                                   // when a new data is available in MPU6050
     
  Gyro_getADC();  // second read

  // empirical, we take a weighted value of the current and the previous values    
  for (axis = 0; axis < 3; axis++) {
      gyroADCinter[axis] =  imu.gyroADC[axis]+gyroADCp[axis];
      if(gyroADCprevious[axis]==0) gyroADCprevious[axis] = imu.gyroADC[axis]; // init previous the first time      
      imu.gyroData[axis] = (gyroADCinter[axis]+gyroADCprevious[axis])/3;
#if defined(TRACE)  
      Serial.print("imu.gyroData[");Serial.print((int)axis);Serial.print("]:");Serial.println(imu.gyroData[axis]);
#endif
      gyroADCprevious[axis] = gyroADCinter[axis]>>1;
  }
  
#if defined(TRACE)  
  Serial.println("<<End computeIMU");
#endif  
}
