#ifndef MULTIWII_H_
#define MULTIWII_H_

#define  VERSION  230

#include "types.h"

// default POSHOLD control gains
#define POSHOLD_P              .11
#define POSHOLD_I              0.0
#define POSHOLD_IMAX           20        // degrees

#define POSHOLD_RATE_P         2.0
#define POSHOLD_RATE_I         0.08      // Wind control
#define POSHOLD_RATE_D         0.045     // try 2 or 3 for POSHOLD_RATE 1
#define POSHOLD_RATE_IMAX      20        // degrees

// default Navigation PID gains
#define NAV_P                  1.4
#define NAV_I                  0.20      // Wind control
#define NAV_D                  0.08      //
#define NAV_IMAX               20        // degrees

#define MINCHECK 1100
#define MAXCHECK 1900

extern volatile unsigned long timer0_overflow_count;

extern const char pidnames[];
extern const char boxnames[];
extern const uint8_t boxids[];

extern uint32_t currentTime;
extern uint16_t previousTime;
extern uint16_t cycleTime;
extern uint16_t calibratingA;
extern uint16_t calibratingG;
extern int16_t  magHold,headFreeModeHold;
extern uint8_t  vbatMin;
extern uint8_t  rcOptions[CHECKBOXITEMS];
extern int32_t  BaroAlt,AltHold;
extern int16_t  BaroPID;
extern int16_t  errorAltitudeI;

extern int16_t  i2c_errors_count;
extern uint8_t alarmArray[16];
extern global_conf_t global_conf;
extern uint8_t dynP8[2], dynD8[2];

extern imu_t imu;
extern analog_t analog;
extern alt_t alt;
extern att_t att;

extern int16_t debug[4];

extern conf_t conf;

extern flags_struct_t f;
extern uint16_t intPowerTrigger1;

extern int16_t gyroZero[3];
extern int16_t accZero[3];

extern int16_t axisPID[3];
extern int16_t motor[8];
extern int16_t servo[8];


extern int16_t rcData[RC_CHANS];
extern int16_t rcSerial[8];
extern int16_t rcCommand[4];
extern uint8_t rcSerialCount;
extern int16_t lookupPitchRollRC[5];
extern int16_t lookupThrottleRC[11];

  // **********************
  // GPS common variables
  // **********************
  extern int16_t  GPS_angle[2];                      // the angles that must be applied for GPS correction
  extern int32_t  GPS_coord[2];
  extern int32_t  GPS_home[2];
  extern int32_t  GPS_hold[2];
  extern uint8_t  GPS_numSat;
  extern uint16_t GPS_distanceToHome;                          // distance to home  - unit: meter
  extern int16_t  GPS_directionToHome;                         // direction to home - unit: degree
  extern uint16_t GPS_altitude;                                // GPS altitude      - unit: meter
  extern uint16_t GPS_speed;                                   // GPS speed         - unit: cm/s
  extern uint8_t  GPS_update;                              // a binary toogle to distinct a GPS position update
  extern uint16_t GPS_ground_course;                       //                   - unit: degree*10
  extern uint8_t  GPS_Present;                             // Checksum from Gps serial
  extern uint8_t  GPS_Enable;

  #define LAT  0
  #define LON  1

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Serial GPS only variables
  //navigation mode
  #define NAV_MODE_NONE          0
  #define NAV_MODE_POSHOLD       1
  #define NAV_MODE_WP            2
  extern uint8_t nav_mode; // Navigation mode
  extern int16_t  nav[2];
  extern int16_t  nav_rated[2];    //Adding a rate controller to the navigation to make it smoother

  // default POSHOLD control gains
  #define POSHOLD_P              .11
  #define POSHOLD_I              0.0
  #define POSHOLD_IMAX           20        // degrees

  #define POSHOLD_RATE_P         2.0
  #define POSHOLD_RATE_I         0.08      // Wind control
  #define POSHOLD_RATE_D         0.045     // try 2 or 3 for POSHOLD_RATE 1
  #define POSHOLD_RATE_IMAX      20        // degrees

  // default Navigation PID gains
  #define NAV_P                  1.4
  #define NAV_I                  0.20      // Wind control
  #define NAV_D                  0.08      //
  #define NAV_IMAX               20        // degrees

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Serial GPS only variables
  //navigation mode
  #define NAV_MODE_NONE          0
  #define NAV_MODE_POSHOLD       1
  #define NAV_MODE_WP            2

  extern volatile uint8_t  spekFrameFlags;
  extern volatile uint32_t spekTimeLast;

void annexCode();

  void MultiWii_setup();
  void MultiWii_loop ();


#endif /* MULTIWII_H_ */
