/*
  PID_h - Library to compute the PID value for asservissement
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/


#ifndef PID_h
#define PID_h

static int kP=100; // Proportionnal coefficient
static int kI=10;  // Integral coefficient
static int kD=50;  // Derivative coefficient

int computePID (int error);
/* Description: Compute the PID value based on the error values               */
/* input:       error                                                         */
/*                  = last error value measured                               */  
/* output:      return                                                        */                            
/*                  = PID value computed                                      */                                       
/* lib:         none                                                          */ 

#endif
