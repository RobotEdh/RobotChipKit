/*
  PID.cpp - Library to compute the PID value for asservissement
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/

#include <PID.h>

int computePID (int error)
{

 static int sumError = 0; 	//sumError is initialized only once across several calls of computePID()
 static int prevError = 0;	//prevError is initialized only once across several calls of computePID()
 
 double pid = 0.0;
 
 sumError += error;
 pid = (kP*error) + (kD*(error-prevError)) + (kI*sumError);
 prevError = error;
 
 pid = pid/(kP+kD+kI);
 
 return (int)pid;
 
} 