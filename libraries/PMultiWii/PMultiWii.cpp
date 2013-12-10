#include "WProgram.h"

#include "config.h"
#include "def.h"
#include "types.h"
#include "PMultiWii.h"
#include "PEEPROM.h"
#include "PIMU.h"
#include "POutput.h"
#include "PRX.h"
#include "PSensors.h"
#include "PSerial.h"


/*********** RC alias *****************/

const char pidnames[] PROGMEM =
  "ROLL;"
  "PITCH;"
  "YAW;"
  "ALT;"
  "Pos;"
  "PosR;"
  "NavR;"
  "LEVEL;"
  "MAG;"
  "VEL;"
;

const char boxnames[] PROGMEM = // names for dynamic generation of config GUI
    "ARM;"
    "ANGLE;"
    "HORIZON;"
;

const uint8_t boxids[] PROGMEM = {// permanent IDs associated to boxes. This way, you can rely on an ID number to identify a BOX function.
    0, //"ARM;"
    1, //"ANGLE;"
    2, //"HORIZON;"
};


uint32_t currentTime = 0;
uint16_t previousTime = 0;
uint16_t cycleTime = 0;     // this is the number in micro second to achieve a full loop, it can differ a little and is taken into account in the PID loop
uint16_t calibratingA = 0;  // the calibration is done in the main loop. Calibrating decreases at each cycle down to 0, then we enter in a normal mode.
uint16_t calibratingG;
int16_t  magHold,headFreeModeHold; // [-180;+180]
uint8_t  vbatMin = VBATNOMINAL;  // lowest battery voltage in 0.1V steps
uint8_t  rcOptions[CHECKBOXITEMS];
int32_t  BaroAlt,AltHold; // in cm
int16_t  BaroPID = 0;
int16_t  errorAltitudeI = 0;


int16_t gyroZero[3] = {0,0,0};
int16_t accZero[3] = {0,0,0};
imu_t imu;

analog_t analog;
alt_t alt;
att_t att;
flags_struct_t f;

int16_t  i2c_errors_count = 0;

// ******************
// rc functions
// ******************

#define ROL_LO  (1<<(2*ROLL))
#define ROL_CE  (3<<(2*ROLL))
#define ROL_HI  (2<<(2*ROLL))
#define PIT_LO  (1<<(2*PITCH))
#define PIT_CE  (3<<(2*PITCH))
#define PIT_HI  (2<<(2*PITCH))
#define YAW_LO  (1<<(2*YAW))
#define YAW_CE  (3<<(2*YAW))
#define YAW_HI  (2<<(2*YAW))
#define THR_LO  (1<<(2*THROTTLE))
#define THR_CE  (3<<(2*THROTTLE))
#define THR_HI  (2<<(2*THROTTLE))


int16_t rcData[RC_CHANS];    // interval [1000;2000]
int16_t rcSerial[8];         // interval [1000;2000] - is rcData coming from MSP
int16_t rcCommand[4];        // interval [1000;2000] for THROTTLE and [-500;+500] for ROLL/PITCH/YAW
uint8_t rcSerialCount = 0;   // a counter to select legacy RX when there is no more MSP rc serial data
int16_t lookupPitchRollRC[5];// lookup table for expo & RC rate PITCH+ROLL
int16_t lookupThrottleRC[11];// lookup table for expo & mid THROTTLE


int16_t axisPID[3];
int16_t motor[8];


uint8_t dynP8[2], dynD8[2];
global_conf_t global_conf;
conf_t conf;





void MultiWii_setup() {
    
  SerialOpen(SERIAL0_COM_SPEED);

#if defined(TRACE)
  Serial.println("Start MultiWii_setup");
#endif 
  
  initOutput();

  LoadDefaults();
       
  configureReceiver();

  initSensors();

  previousTime = micros();

  calibratingA = 0;
  calibratingG = 512;

#if defined(TRACE)
  Serial.println("End MultiWii_setup");
#endif 
  
}

// ******** Main Loop *********

void MultiWii_loop () {
 
  static uint8_t rcDelayCommand; // this indicates the number of time (multiple of RC measurement at 50Hz) the sticks must be maintained to run or switch off motors
  static uint8_t rcSticks;       // this hold sticks position for command combos
  uint8_t axis,i;
  int16_t error= 0;
  int16_t delta;
  int16_t PTerm = 0,ITerm = 0,DTerm, PTermACC, ITermACC;
  static int16_t lastGyro[2] = {0,0};

  static int16_t delta1[2],delta2[2];
  static int16_t errorGyroI[2] = {0,0};
  static int32_t errorGyroI_YAW =0;

  static uint32_t rcTime  = 0;
  static int16_t initialThrottleHold;
  static uint32_t timestamp_fixated = 0;
  int16_t rc;
  int32_t prop = 0;

  
  if (currentTime > rcTime ) { // 50Hz: PPM frequency of the RC, no change happen within 20ms
    rcTime = currentTime + 20000;
    
    computeRC();

#if !defined(TEST)    
    serialCom();

    // ------------------ STICKS COMMAND HANDLER --------------------
    // checking sticks positions
    uint8_t stTmp = 0;
    for(i=0;i<4;i++) {
      stTmp >>= 2;
      if(rcData[i] > MINCHECK) stTmp |= 0x80;      // check for MIN
      if(rcData[i] < MAXCHECK) stTmp |= 0x40;      // check for MAX
      //Serial.print("i: "); Serial.println((int)i);  Serial.print("rcData[i]: ");Serial.println(rcData[i]);   

    }
    if(stTmp == rcSticks) {
      if(rcDelayCommand<250) rcDelayCommand++;
    } else rcDelayCommand = 0;
    rcSticks = stTmp;
    
    // perform actions    
    if (rcData[THROTTLE] <= MINCHECK) {            // THROTTLE at minimum
      errorGyroI[ROLL] = 0; errorGyroI[PITCH] = 0; errorGyroI_YAW = 0; // reset I sum
    }
    
    // calibrate online
    if(rcDelayCommand == 20) {
        if      (rcSticks == THR_LO + YAW_LO + PIT_LO + ROL_CE) { // throttle=low, yaw=low, pitch=min, roll=center =>GYRO calibration
            calibratingG=512; 
        }
        else if (rcSticks == THR_HI + YAW_LO + PIT_LO + ROL_CE) { // throttle=max, yaw=low, pitch=min, roll=center => ACC calibration
             calibratingA=512;     
        }       
        rcDelayCommand = 0;   
    }
#endif    
  } /* end currentTime > rcTime */

#if !defined(TEST)   
  computeIMU();
#endif    
  // Measure loop rate just afer reading the sensors
  currentTime = micros();
  cycleTime = currentTime - previousTime;
  previousTime = currentTime;
 
#if !defined(TEST) 
  //**** PITCH & ROLL & YAW PID ****
   
  // PITCH & ROLL
  for(axis=0;axis<2;axis++) {
  	
  	//error
    rc = rcCommand[axis]<<1; // [#-500;#+500] translated to [#-1000;#+1000]
    error = rc - imu.gyroData[axis];
#if defined(TRACE)  
    Serial.print("imu.gyroData[");Serial.print((int)axis);Serial.print("]:");Serial.println(imu.gyroData[axis]);
    Serial.print("rcCommand[");Serial.print((int)axis);Serial.print("]:");Serial.println(rcCommand[axis]);
    Serial.print("error:");Serial.println(error);
#endif   

    //I term
    if (abs(imu.gyroData[axis])>640) errorGyroI[axis] = 0;                     // WindUp, ignore startup or big errors
    errorGyroI[axis]  = constrain(errorGyroI[axis]+error,-16000,+16000);       // WindUp   16 bits is ok here
    ITerm = (errorGyroI[axis]>>7)*conf.pid[axis].I8>>6;                        // 16 bits is ok here 16000/125 = 128 ; 128*250 = 32000

    //P term
    PTerm = (int32_t)rc*conf.pid[axis].P8>>6;
    PTerm -= ((int32_t)imu.gyroData[axis]*dynP8[axis])>>6; // 32 bits is needed for calculation   
    
    //D term
    delta          = imu.gyroData[axis] - lastGyro[axis];  // 16 bits is ok here, the dif between 2 consecutive gyro reads is limited to 800
    lastGyro[axis] = imu.gyroData[axis];
    DTerm          = delta+delta1[axis]+delta2[axis];
    delta2[axis]   = delta1[axis];
    delta1[axis]   = delta;
 
    DTerm = ((int32_t)DTerm*dynD8[axis])>>5;        // 32 bits is needed for calculation

    axisPID[axis] =  PTerm + ITerm - DTerm;
#if defined(TRACE)  
    Serial.print("axisPID[");Serial.print((int)axis);Serial.print("]:");Serial.println(axisPID[axis]);
#endif     
  }

  //YAW
  #define GYRO_P_MAX 300
  #define GYRO_I_MAX 250
  
  //error
  rc = (int32_t)rcCommand[YAW] * (2*conf.yawRate + 32)/32;
  error = rc - imu.gyroData[YAW];
#if defined(TRACE)  
  Serial.print("imu.gyroData[");Serial.print((int)YAW);Serial.print("]:");Serial.println(imu.gyroData[YAW]);
  Serial.print("rcCommand[");Serial.print((int)YAW);Serial.print("]:");Serial.println(rcCommand[YAW]);
  Serial.print("error:");Serial.println(error);
#endif  
  
  //I term
  if (abs(rc) > 50) errorGyroI_YAW = 0;
  errorGyroI_YAW  += (int32_t)error*conf.pid[YAW].I8;
  errorGyroI_YAW  = constrain(errorGyroI_YAW, 2-((int32_t)1<<28), -2+((int32_t)1<<28));
  ITerm = constrain((int16_t)(errorGyroI_YAW>>13),-GYRO_I_MAX,+GYRO_I_MAX);
   
  //P term
  PTerm = (int32_t)error*conf.pid[YAW].P8>>6;
  PTerm = constrain(PTerm,-GYRO_P_MAX,+GYRO_P_MAX);
  
   
  axisPID[YAW] =  PTerm + ITerm; // No D term
  
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
