/*
  GP2Y0A21YK.h - Library for communicating with IR sensor GP2Y0A21YK
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/


#ifndef PID_h
#define PID_h

static int kP=100; // Proportioonal coefficient
static int kI=10;  // Integral coefficient
static int kD=50;  // Derivative coefficient

int computePID (int error);  // error detected

#endif
