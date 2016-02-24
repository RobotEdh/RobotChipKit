#include "Wire.h"
#include "MPU6050.h"

MPU6050 MPU6050;             // The accel+gyro class

void setup() {   
  int ret = 0;
   
  // initialize serial communication
  Serial.begin(9600);
  
  // initialize I2C
  Wire.begin();

  // initialize the MPU6050
  ret=MPU6050.initialize();
  if (ret != 0)
  {  
        Serial.print("Error Init MPU6050, error: ");
        Serial.println(ret);
  }        
  else
  {
        Serial.println("Init MPU6050 OK");
  }
  Serial.println(MPU6050.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

}

void loop() {
    
  int16_t acc_z =0; 
  
  acc_z= MPU6050.getAccelerationZ(); 
  Serial.println(acc_z); 
  
  delay(1000);
}
  
