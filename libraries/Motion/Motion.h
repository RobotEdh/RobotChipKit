/*
  Motion.h - Library for communicating with infrared motion sensor 
  Created by EDH, December 30, 2014.
  Released into the public domain.
*/


#ifndef Motion_h
#define Motion_h

#include <wiring.h> // used for digital read function (core lib)


#define Motion_Pin 7   // Ambient Light Sensor TEMT6000 digital pin 7
/* Digital interface is provided on pin   7 */
/* Power +5V is set on pin VCC              */
/* Ground    is set on pin GND              */


void Motion_init(int pin);
/* Description: Initialize the Infrared motion sensor                         */                                            
/* input:       pin                                                           */ 
/*                  = pin connected to the Infrared motion sensor             */                       
/* output:      none                                                          */
/* lib:         none                                                          */

int Motion_status(int pin);
/* Description: Get light using Infrared motion sensor                        */
/*              connected to 5V                                               */
/* input:       pin                                                           */ 
/*                  = pin connected to the Infrared motion sensor             */    
/* output:      return                                                        */                            
/*                 0 = no motion                                              */
/*                 1 = motion                                                 */
/* lib:         digitalRead                                                   */

#endif
