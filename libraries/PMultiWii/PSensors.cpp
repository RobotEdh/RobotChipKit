#include "WProgram.h"
#include "Wire.h" // used for I2C protocol (lib)

#include "config.h"
#include "def.h"
#include "types.h"
#include "PMultiWii.h"
#include "PSensors.h"


/*** I2C address ***/
 #define MPU6050_ADDRESS     0x68 // address pin AD0 low (GND)
//#define MPU6050_ADDRESS     0x69 // address pin AD0 high (VCC)

uint8_t rawADC[6];
  
// ************************************************************************************************************
// I2C general functions
// ************************************************************************************************************
void i2c_init() {

#if defined(TRACE)	
  Serial.println(">i2c_init");
#endif

  Wire.begin(); // setup I2C
}


size_t i2c_read_reg_to_buf(uint8_t devAddr, uint8_t regAddr, void *buf, size_t size) {
  
  Wire.beginTransmission(devAddr);
  Wire.send(regAddr);
                
  uint8_t ret = Wire.endTransmission();
#if defined(TRACE)  
  if (ret != 0) {
      Serial.print("i2c_read_reg_to_buf ret: ");Serial.println((int)ret);
      Serial.print("devAddr: ");Serial.println((int)devAddr,HEX);
  }
#endif                          
  delay(1);
  
  Wire.requestFrom(devAddr, (uint8_t)size);
  size_t bytes_read = 0;
  uint8_t *b = (uint8_t*)buf;
  
  while (size--) {
    *b++ = Wire.receive();
    bytes_read++;
  }
  return bytes_read;	
}	


void i2c_getSixRawADC(uint8_t add, uint8_t reg) {
  i2c_read_reg_to_buf(add, reg, &rawADC, 6);
}


uint8_t i2c_writeReg(uint8_t devAddr, uint8_t regAddr, uint8_t val) {
    Wire.beginTransmission(devAddr);
    Wire.send(regAddr); // send address
    Wire.send(val);     // send value
 
    uint8_t ret = Wire.endTransmission();
#if defined(TRACE)      
    if (ret != 0) {
        Serial.print("i2c_writeReg ret: ");Serial.println((int)ret);
        Serial.print("devAddr: ");Serial.println((int)devAddr,HEX);
     }   
#endif     
    return ret; 
}



// ************************************************************************************************************
// I2C Gyroscope and Accelerometer MPU6050
// ************************************************************************************************************

bool MPU_init() {
  uint8_t ret=0;
    
#if defined(TRACE)	
  Serial.println(">MPU_init");
#endif    
  // FIX ME change the I2C clock rate to 400kHz like multiwii ?
  ret=i2c_writeReg(MPU6050_ADDRESS, 0x6B, 0x80); //PWR_MGMT_1    -- DEVICE_RESET 1
  if (ret> 0) return false;
  delay(5);
  ret=i2c_writeReg(MPU6050_ADDRESS, 0x6B, 0x03); //PWR_MGMT_1    -- SLEEP 0; CYCLE 0; TEMP_DIS 0; CLKSEL 3 (PLL with Z Gyro reference)
  if (ret> 0) return false;
  ret=i2c_writeReg(MPU6050_ADDRESS, 0x1A, 0x00); //CONFIG        -- EXT_SYNC_SET 0 (disable input pin for data sync) ; DLPF_CFG = 0 => ACC bandwidth = 260Hz  GYRO bandwidth = 256Hz)
  if (ret> 0) return false;

  return true;
}


bool Gyro_init() {
  uint8_t ret=0;
  
  ret=i2c_writeReg(MPU6050_ADDRESS, 0x1B, 0x18); //GYRO_CONFIG   -- FS_SEL = 3: Full scale set to 2000 deg/sec
  if (ret> 0) return false;
 
  return true;
}


bool ACC_init () {
#if defined(TRACE)	
  Serial.println(">ACC_init");
#endif    
  i2c_writeReg(MPU6050_ADDRESS, 0x1C, 0x10);             //ACCEL_CONFIG  -- AFS_SEL=2 (Full Scale = +/-8G)  ; ACCELL_HPF=0   //note something is wrong in the spec.
  //note: something seems to be wrong in the spec here. With AFS=2 1G = 4096 but according to my measurement: 1G=2048 (and 2048/8 = 256)
  //confirmed here: http://www.multiwii.com/forum/viewtopic.php?f=8&t=1080&start=10#p7480
  
  return true;
}


bool initSensors() {
#if defined(TRACE)	
  Serial.println(">Start initSensors"); 
#endif 

  i2c_init();
  delay(100);
  
  if (!MPU_init()) return false;
  if (!Gyro_init()) return false ;
  if (!ACC_init()) return false;
  
#if defined(TRACE)	  
  Serial.println("<End OK initSensors");
#endif 
  return true;
}


// ************************************************************************
// GYRO common part
// adjust imu.gyroADC according gyroZero and 
// limit the variation between two consecutive readings (anti gyro glitch)
// ************************************************************************
void GYRO_Common() {
  static int16_t previousGyroADC[3] = {0,0,0};
  static int32_t g[3];
  uint8_t axis;
  double e_pitch, e_roll;
  double i_pitch, i_roll;
  static double prev_c_pitch = 0.0;
  static double prev_c_roll  = 0.0;
  double a = 0.98;
  uint32_t currentTime;
  static uint32_t previousTime = 0;
  uint32_t dt;

  if (calibratingG>0) {
    /* Calbrating phase */
    for (axis = 0; axis < 3; axis++) {
      // Reset g[axis] at start of calibration
      if (calibratingG == 512) {
        g[axis]=0;
      }
      // Sum up 512 readings
      g[axis] +=imu.gyroADC[axis];
      // Clear global variables for next reading
      imu.gyroADC[axis]=0;
      gyroZero[axis]=0;
      
      // Calculate average on 512 readings   
      if (calibratingG == 1) {
        gyroZero[axis]=g[axis]>>9;
      }
    }
    calibratingG--;
    
#if defined(TRACE)  
    Serial.print("gyroZero[");Serial.print((int)axis);Serial.print("]:");Serial.println((int)gyroZero[axis]);
#endif    
  }
  else
  {
    /* Flying phase */
    for (axis = 0; axis < 3; axis++) {
      imu.gyroADC[axis]  -= gyroZero[axis];  
#if defined(TRACE)  
      Serial.print("1-imu.gyroADC[");Serial.print((int)axis);Serial.print("]:");Serial.println((int)imu.gyroADC[axis]);
#endif    
      //anti gyro glitch, limit the variation between two consecutive readings
      if (previousGyroADC[axis] != 0) imu.gyroADC[axis] = constrain(imu.gyroADC[axis],previousGyroADC[axis]-800,previousGyroADC[axis]+800);  
#if defined(TRACE)  
      Serial.print("2-imu.gyroADC[");Serial.print((int)axis);Serial.print("]:");Serial.println(imu.gyroADC[axis]);
#endif      
      previousGyroADC[axis] = imu.gyroADC[axis];
    }
    
    // compute Euler angles between [-PI;+PI]
	e_pitch  = atan2((double)imu.accADC[PITCH],  pow(pow((double)imu.accADC[YAW] + 1.0, 2.0) + pow((double)imu.accADC[ROLL],  2), 0.5));
	e_roll   = atan2((double)imu.accADC[ROLL],   pow(pow((double)imu.accADC[YAW] + 1.0, 2.0) + pow((double)imu.accADC[PITCH], 2), 0.5));

#if defined(TRACE)  
      Serial.print("e_pitch:");Serial.println(e_pitch);
      Serial.print("e_roll:");Serial.println(e_roll);
#endif   
    // compute delta time DT for gyro integration
    currentTime = millis();
    dt = currentTime-previousTime;
    previousTime = currentTime;
  
    // integrate the gyros angular velocity to determine angles
    i_pitch = imu.gyroData[0] * dt;
    i_roll =  imu.gyroData[1] * dt;
  
    // adjust angles Pitch & Roll using complementary filter between [-PI;+PI]
	if (prev_c_pitch == 0) prev_c_pitch = e_pitch;
	c_angle[0] = a * (prev_c_pitch + i_pitch) + (1 - a) * e_pitch;
	prev_c_pitch = c_angle[0];
    
    if (prev_c_roll == 0) prev_c_roll = e_roll;
	c_angle[1]  = a * (prev_c_roll  + i_roll)  + (1 - a) * e_roll;
	prev_c_roll = c_angle[1];

#if defined(TRACE)  
      Serial.print("c_pitch:");Serial.println(c_pitch);
      Serial.print("c_roll:");Serial.println(c_roll);
#endif  
  }

}


/*******************************************************************************/
/*                    Gyro_getADC                                              */
/*  - call i2c_getSixRawADC to get raw values for:                             */
/*                            imu.gyroADC[axis]                                */
/*  - call GYRO_Common to adjust imu.gyroADC[axis] with gyroZero and           */
/*    limit the variation between two consecutive readings (anti gyro glitch)  */
/*    interval [-8192;+8192]                                                   */
/*******************************************************************************/

void Gyro_getADC () {
#if defined(TRACE)	
  Serial.println(">Gyro_getADC");
#endif    
  i2c_getSixRawADC(MPU6050_ADDRESS, 0x43); // range: +/- 8192; +/- 2000 deg/sec
  
  imu.gyroADC[ROLL]  = (rawADC[0] << 8) | rawADC[1];
  imu.gyroADC[PITCH] = (rawADC[2] << 8) | rawADC[3];
  imu.gyroADC[YAW]   = (rawADC[4] << 8) | rawADC[5]; 
  //FIX ME divide by 4 like multiwii ?
#if defined(TRACE)    
  Serial.print("imu.gyroADC[");Serial.print((int)ROLL);Serial.print("]:");Serial.println((int16_t)imu.gyroADC[ROLL],DEC);
  Serial.print("imu.gyroADC[");Serial.print((int)PITCH);Serial.print("]:");Serial.println((int16_t)imu.gyroADC[PITCH],DEC);
  Serial.print("imu.gyroADC[");Serial.print((int)YAW);Serial.print("]:");Serial.println((int16_t)imu.gyroADC[YAW],DEC); 
#endif  

  GYRO_Common();
}

// ************************************************************************
// ACC common part
// adjust imu.accADC according accZero
// ************************************************************************
void ACC_Common() {
  static int32_t a[3];
  uint8_t axis;
  
  if (calibratingA>0) { /* Calibrating phase */
    for (axis = 0; axis < 3; axis++) {
      // Reset a[axis] at start of calibration
      if (calibratingA == 512) a[axis]=0;
      // Sum up 512 readings
      a[axis] +=imu.accADC[axis];
      // Clear global variables for next reading
      imu.accADC[axis]=0;
      accZero[axis]=0;
    }
    // Calculate average on 512 readings , shift Z down by ACC_1G
    if (calibratingA == 1) {
      accZero[ROLL]  = a[ROLL]>>9;
      accZero[PITCH] = a[PITCH]>>9;
      accZero[YAW]   = (a[YAW]>>9)-ACC_1G; // FIX ME: Check value ACC_1G, depends on scale. For +/- 8g => 1g = 4096 => ACC_1G = 4096  or 4096/8 if acc is divded by 8 
    }
    calibratingA--;
#if defined(TRACE)  
    Serial.print("accZero[");Serial.print((int)axis);Serial.print("]:");Serial.println(accZero[axis]);
      
#endif    
  }
  
  else
  
  {                     /* Flying phase */
    for (axis = 0; axis < 3; axis++) {
      imu.accADC[axis]  -= accZero[axis];
#if defined(TRACE)  
      Serial.print("imu.accADC[");Serial.print((int)axis);Serial.print("]:");Serial.println(imu.accADC[axis]);
#endif 
    }
  }
}


/****************************************************************/
/*                    ACC_getADC                                */
/*  - call i2c_getSixRawADC to get raw values for:              */
/*                            imu.accADC[axis]                  */
/*  - call ACC_Common to adjust imu.accADC[axis] with accZero   */
/*    interval [-4096;+4096]                                    */
/*  - call ComputeEulerAngles to get the Euler angles in radians*/
/****************************************************************/

void ACC_getADC () {
#if defined(TRACE)
  Serial.println(">ACC_getADC"); 
#endif    
  i2c_getSixRawADC(MPU6050_ADDRESS, 0x3B);
    
  imu.accADC[ROLL]  = (rawADC[0] << 8) | rawADC[1];
  imu.accADC[PITCH] = (rawADC[2] << 8) | rawADC[3];
  imu.accADC[YAW]   = (rawADC[4] << 8) | rawADC[5]; 
  /* FIX ME divide by 8 like Multiwii ? */
#if defined(TRACE)    
  Serial.print("imu.accADC[");Serial.print((int)ROLL);Serial.print("]:");Serial.println((int16_t)imu.accADC[ROLL],DEC);
  Serial.print("imu.accADC[");Serial.print((int)PITCH);Serial.print("]:");Serial.println((int16_t)imu.accADC[PITCH],DEC);
  Serial.print("imu.accADC[");Serial.print((int)YAW);Serial.print("]:");Serial.println((int16_t)imu.accADC[YAW],DEC); 
#endif      

  ACC_Common();
  
}

