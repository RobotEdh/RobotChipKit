/*
  PID.cpp - Library to compute the PID for asservissement
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/

#include <PID.h>

int computePID (int error)
{
 // returns PID
 static int sumError = 0;
 static int prevError = 0;
 
 double pid;
 
 sumError += error;
 pid = (kP*error) + (kD*(error-prevError)) + (kI*sumError);
 prevError = error;
 
 pid = pid/(kP+kD+kI);
 
 return (int)pid;
 
} 