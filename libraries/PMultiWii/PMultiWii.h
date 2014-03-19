#ifndef MULTIWII_H_
#define MULTIWII_H_

#define  VERSION  230

#include "types.h"


#define MINCHECK 1200
#define MAXCHECK 1900


extern uint16_t calibratingA;
extern uint16_t calibratingG;

extern imu_t imu;

extern int16_t gyroZero[3];
extern int16_t accZero[3];

extern double axisPID[3];

extern int16_t rcData[RC_CHANS];
extern int16_t rcSerial[8];
extern int16_t rcCommand[4];


bool MultiWii_setup();
void MultiWii_loop ();


#endif /* MULTIWII_H_ */
