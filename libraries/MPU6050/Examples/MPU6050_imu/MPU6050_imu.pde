#include "Wire.h"
#include "MPU6050.h"
#include "imu.h"

MPU6050 MPU6050;             // The accel+gyro class


void setup() {   
   
  // initialize serial communication
  Serial.begin(9600);
  
 IMUInit();
 
 
}

void loop() {
    
  IMUExecution();
  
}
  
