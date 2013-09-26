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
  #if ACC
    "ANGLE;"
    "HORIZON;"
  #endif
;

const uint8_t boxids[] PROGMEM = {// permanent IDs associated to boxes. This way, you can rely on an ID number to identify a BOX function.
  0, //"ARM;"
  #if ACC
    1, //"ANGLE;"
    2, //"HORIZON;"
  #endif
};


uint32_t currentTime = 0;
uint16_t previousTime = 0;
uint16_t cycleTime = 0;     // this is the number in micro second to achieve a full loop, it can differ a little and is taken into account in the PID loop
uint16_t calibratingA = 0;  // the calibration is done in the main loop. Calibrating decreases at each cycle down to 0, then we enter in a normal mode.
uint16_t calibratingB = 0;  // baro calibration = get new ground pressure value
uint16_t calibratingG;
int16_t  magHold,headFreeModeHold; // [-180;+180]
uint8_t  vbatMin = VBATNOMINAL;  // lowest battery voltage in 0.1V steps
uint8_t  rcOptions[CHECKBOXITEMS];
int32_t  BaroAlt,AltHold; // in cm
int16_t  BaroPID = 0;
int16_t  errorAltitudeI = 0;

// **************
// gyro+acc IMU
// **************
int16_t gyroZero[3] = {0,0,0};
int16_t accZero[3] = {0,0,0};
int16_t angle[2]    = {0,0};  // absolute angle inclination in multiple of 0.1 degree    180 deg = 1800
imu_t imu;

analog_t analog;
alt_t alt;
att_t att;
flags_struct_t f;

int16_t  i2c_errors_count = 0;

#if defined(THROTTLE_ANGLE_CORRECTION)
  int16_t throttleAngleCorrection = 0;	// correction of throttle in lateral wind,
  int8_t  cosZ = 100;					// cos(angleZ)*100
#endif

// ******************
// rc functions
// ******************
#define MINCHECK 1100
#define MAXCHECK 1900

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

int16_t failsafeEvents = 0;
volatile int16_t failsafeCnt = 0;

int16_t rcData[RC_CHANS];    // interval [1000;2000]
int16_t rcSerial[8];         // interval [1000;2000] - is rcData coming from MSP
int16_t rcCommand[4];        // interval [1000;2000] for THROTTLE and [-500;+500] for ROLL/PITCH/YAW
uint8_t rcSerialCount = 0;   // a counter to select legacy RX when there is no more MSP rc serial data
int16_t lookupPitchRollRC[5];// lookup table for expo & RC rate PITCH+ROLL
int16_t lookupThrottleRC[11];// lookup table for expo & mid THROTTLE

// *************************
// motor and servo functions
// *************************
int16_t axisPID[3];
int16_t motor[8];

// ************************
// EEPROM Layout definition
// ************************
static uint8_t dynP8[2], dynD8[2];
global_conf_t global_conf;
conf_t conf;

void annexCode() { // this code is excetuted at each loop and won't interfere with control loop if it lasts less than 650 microseconds
  static uint32_t calibratedAccTime;
  uint16_t tmp,tmp2;
  uint8_t axis,prop1,prop2;

#if defined(TRACE) 
  Serial.println(">annexCode"); 
#endif 
 
  // PITCH & ROLL only dynamic PID adjustemnt,  depending on throttle value
  prop2 = 128; // prop2 was 100, is 128 now
  if (rcData[THROTTLE]>1500) { // breakpoint is fix: 1500
    if (rcData[THROTTLE]<2000) {
      prop2 -=  ((uint16_t)conf.dynThrPID*(rcData[THROTTLE]-1500)>>9); //  /512 instead of /500
    } else {
      prop2 -=  conf.dynThrPID;
    }
  }

  for(axis=0;axis<3;axis++) {
#if defined(TRACE) 
    Serial.print("rcData[axis");Serial.print((int)axis);Serial.print("]:");Serial.println(rcData[axis]);
#endif    
    tmp = min(abs(rcData[axis]-MIDRC),500);
    if(axis!=2) { //ROLL & PITCH
      tmp2 = tmp>>7; // 500/128 = 3.9  => range [0;3]
      rcCommand[axis] = lookupPitchRollRC[tmp2] + ((tmp-(tmp2<<7)) * (lookupPitchRollRC[tmp2+1]-lookupPitchRollRC[tmp2])>>7);
      prop1 = 128-((uint16_t)conf.rollPitchRate*tmp>>9); // prop1 was 100, is 128 now -- and /512 instead of /500
      prop1 = (uint16_t)prop1*prop2>>7; // prop1: max is 128   prop2: max is 128   result prop1: max is 128
      dynP8[axis] = (uint16_t)conf.pid[axis].P8*prop1>>7; // was /100, is /128 now
      dynD8[axis] = (uint16_t)conf.pid[axis].D8*prop1>>7; // was /100, is /128 now
    } else {      // YAW
      rcCommand[axis] = tmp;
    }
    if (rcData[axis]<MIDRC) rcCommand[axis] = -rcCommand[axis];
#if defined(TRACE) 
    Serial.print("rcCommand[axis");Serial.print((int)axis);Serial.print("]:");Serial.println(rcCommand[axis]);
#endif         
  }
  tmp = constrain(rcData[THROTTLE],MINCHECK,2000);
  tmp = (uint32_t)(tmp-MINCHECK)*1000/(2000-MINCHECK); // [MINCHECK;2000] -> [0;1000]
  tmp2 = tmp/100;
  rcCommand[THROTTLE] = lookupThrottleRC[tmp2] + (tmp-tmp2*100) * (lookupThrottleRC[tmp2+1]-lookupThrottleRC[tmp2]) / 100; // [0;1000] -> expo -> [conf.minthrottle;MAXTHROTTLE]
#if defined(TRACE) 
    Serial.print("rcCommand[THROTTLE");Serial.print((int)THROTTLE);Serial.print("]:");Serial.println(rcCommand[THROTTLE]);
#endif

  if ( currentTime > calibratedAccTime ) {
    if (! f.SMALL_ANGLES_25) {
      // the multi uses ACC and is not calibrated or is too much inclinated
      f.ACC_CALIBRATED = 0;
      calibratedAccTime = currentTime + 100000;
    } else {
      f.ACC_CALIBRATED = 1;
    }
  }

}


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

  f.SMALL_ANGLES_25=1; // important for gyro only conf

#if defined(TRACE)
  Serial.println("End MultiWii_setup");
#endif 
  
}

// ******** Main Loop *********

void MultiWii_loop () {
 
  static uint8_t rcDelayCommand; // this indicates the number of time (multiple of RC measurement at 50Hz) the sticks must be maintained to run or switch off motors
  static uint8_t rcSticks;       // this hold sticks position for command combos
  uint8_t axis,i;
  int16_t error,errorAngle;
  int16_t delta;
  int16_t PTerm = 0,ITerm = 0,DTerm, PTermACC, ITermACC;
  static int16_t lastGyro[2] = {0,0};
  static int32_t errorGyroI_YAW;
  static int16_t errorAngleI[2] = {0,0};

  static int16_t delta1[2],delta2[2];
  static int16_t errorGyroI[2] = {0,0};

  static uint32_t rcTime  = 0;
  static int16_t initialThrottleHold;
  static uint32_t timestamp_fixated = 0;
  int16_t rc;
  int32_t prop = 0;

  
  if (currentTime > rcTime ) { // 50Hz: PPM frequency of the RC, no change happen within 20ms
    rcTime = currentTime + 20000;
    
    computeRC();

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
      errorGyroI[ROLL] = 0; errorGyroI[PITCH] = 0; errorGyroI_YAW = 0; 
      errorAngleI[ROLL] = 0; errorAngleI[PITCH] = 0;
    }
    
    // calibrate and configure online
    if(rcDelayCommand == 20) {
        if      (rcSticks == THR_LO + YAW_LO + PIT_LO + ROL_CE) { // throttle=low, yaw=low, pitch=min, roll=center =>GYRO calibration
            calibratingG=512; 
        }
        else if (rcSticks == THR_HI + YAW_LO + PIT_LO + ROL_CE) { // throttle=max, yaw=low, pitch=min, roll=center => ACC calibration
             calibratingA=512;     
        }

        if      (rcSticks == THR_HI + YAW_CE + PIT_HI + ROL_CE) {conf.angleTrim[PITCH]+=2; i=1;}
        else if (rcSticks == THR_HI + YAW_CE + PIT_LO + ROL_CE) {conf.angleTrim[PITCH]-=2; i=1;}
        else if (rcSticks == THR_HI + YAW_CE + PIT_CE + ROL_HI) {conf.angleTrim[ROLL] +=2; i=1;}
        else if (rcSticks == THR_HI + YAW_CE + PIT_CE + ROL_LO) {conf.angleTrim[ROLL] -=2; i=1;}
         
        rcDelayCommand = 0;   
    }
  } /* end currentTime > rcTime */
 
  computeIMU();
  
  // Measure loop rate just afer reading the sensors
  currentTime = micros();
  cycleTime = currentTime - previousTime;
  previousTime = currentTime;
 
  //**** PITCH & ROLL & YAW PID ****
 
  // PITCH & ROLL
  for(axis=0;axis<2;axis++) {
  	
  	//error
    rc = rcCommand[axis]<<1;
    error = rc - imu.gyroData[axis];
#if defined(TRACE)  
    Serial.print("imu.gyroData[");Serial.print((int)axis);Serial.print("]:");Serial.println(imu.gyroData[axis]);
    Serial.print("rcCommand[");Serial.print((int)axis);Serial.print("]:");Serial.println(rcCommand[axis]);
    Serial.print("error:");Serial.println(error);
#endif   

    //I term
    errorGyroI[axis]  = constrain(errorGyroI[axis]+error,-16000,+16000);       // WindUp   16 bits is ok here
    if (abs(imu.gyroData[axis])>640) errorGyroI[axis] = 0;
    ITerm = (errorGyroI[axis]>>7)*conf.pid[axis].I8>>6;                        // 16 bits is ok here 16000/125 = 128 ; 128*250 = 32000

    //P term
    PTerm = (int32_t)rc*conf.pid[axis].P8>>6;
    PTerm -= ((int32_t)imu.gyroData[axis]*dynP8[axis])>>6; // 32 bits is needed for calculation   
    
    //D term
    delta          = imu.gyroData[axis] - lastGyro[axis];  // 16 bits is ok here, the dif between 2 consecutive gyro reads is limited to 800
    lastGyro[axis] = imu.gyroData[axis];
    DTerm          = delta1[axis]+delta2[axis]+delta;
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
  rc = (int32_t)rcCommand[YAW] * (2*conf.yawRate + 30)  >> 5;
  error = rc - imu.gyroData[YAW];
#if defined(TRACE)  
    Serial.print("imu.gyroData[");Serial.print((int)YAW);Serial.print("]:");Serial.println(imu.gyroData[YAW]);
    Serial.print("rcCommand[");Serial.print((int)YAW);Serial.print("]:");Serial.println(rcCommand[YAW]);
    Serial.print("error:");Serial.println(error);
#endif  
  //I term
  errorGyroI_YAW  += (int32_t)error*conf.pid[YAW].I8;
  errorGyroI_YAW  = constrain(errorGyroI_YAW, 2-((int32_t)1<<28), -2+((int32_t)1<<28));
  if (abs(rc) > 50) errorGyroI_YAW = 0;
  
  //P term
  PTerm = (int32_t)error*conf.pid[YAW].P8>>6;
  PTerm = constrain(PTerm,-GYRO_P_MAX,+GYRO_P_MAX);
  
  //I term
  ITerm = constrain((int16_t)(errorGyroI_YAW>>13),-GYRO_I_MAX,+GYRO_I_MAX);
  
  axisPID[YAW] =  PTerm + ITerm; // No D term
#if defined(TRACE)  
    Serial.print("axisPID[");Serial.print((int)YAW);Serial.print("]:");Serial.println(axisPID[YAW]);
#endif

  mixTable();

  writeMotors();
}
