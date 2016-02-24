/*
  Motion.h - Library for communicating with infrared motion sensor 
  Created by EDH, December 30, 2014.
  Released into the public domain.
*/


#ifndef Motion_h
#define Motion_h
#include "WProgram.h"
#include <wiring.h> // used for digital read function (core lib)


#define MOTION_PIN_DEFAULT 3   // Default Infrared motion sensor digital pin 3
/* Digital interface is provided on pin   3 */
/* Power +5V is set on pin VCC              */
/* Ground    is set on pin GND              */

class MotionClass
{  
  public:
    
  MotionClass();

  void Motion_init();
  void Motion_init(int pin);
  /* Description: Initialize the Infrared motion sensor                         */                                            
  /* input:       pin                                                           */ 
  /*                  = pin connected to the Infrared motion sensor             */                       
  /* output:      none                                                          */
  /* lib:         none                                                          */

  int Motion_status();
  /* Description: Get light using Infrared motion sensor                        */
  /*              connected to 5V                                               */  
  /* output:      return                                                        */                            
  /*                 0 = no motion                                              */
  /*                 1 = motion                                                 */
  /*                -1 = pin not initialized                                    */
  /* lib:         digitalRead                                                   */

 private:
 int _Motion_Pin;
 
};

#endif
