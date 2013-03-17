/*
  GP2Y0A21YK.h - Library for commmunicationg with IR sensor GP2Y0A21YK
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/


#ifndef GP2Y0A21YK_h
#define GP2Y0A21YK_h

#include <wiring.h> // used for analogic read function (core lib)

void GP2Y0A21YK_init(int pin);
/* Description: Initialize the IR sensor GP2Y0A21YK                           */                                            
/* input:       pin                                                           */ 
/*                  = pin connected to the IR sensor GP2Y0A21YK               */                       
/* output:      none                                                          */
/* lib:         none                                                          */

int GP2Y0A21YK_getDistanceCentimeter(int pin);
/* Description: Get distance in centimeter using the IR sensor GP2Y0A21YK     */
/*              connected to 3V                                               */
/* input:       pin                                                           */ 
/*                  = pin connected to the IR sensor GP2Y0A21YK               */    
/* output:      return                                                        */                            
/*                  = -1 if distance not determined                           */ 
/*                  = distance in centimeter otherwise                        */
/* lib:         analogRead                                                    */

#endif
