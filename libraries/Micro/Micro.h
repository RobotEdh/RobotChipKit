/*
  Micro.h - Library for communicating with electret micro 
  Created by EDH, July 24, 2015.
  Released into the public domain.
*/


#ifndef Micro_h
#define Micro_h

#include <Arduino.h> // used for pin definition

#define MICRO_PIN_DEFAULT A0  // Default Micro analogic pin J5-01 A0 (PGED1/AN0/CN2/RB0)   Use ADC module channel 2
/* Analogic interface is provided on pin A0 */
/* Power +5V is set on pin VCC              */
/* Ground    is set on pin GND              */


class MicroClass
{  
  public:
    
  MicroClass();

  void Micro_init();
  void Micro_init(int pin);
  /* Description: Initialize the electret micro                                 */                                            
  /* input:       pin                                                           */ 
  /*                  = pin connected to the electret micro                     */                       
  /* output:      none                                                          */
  /* lib:         none                                                          */

  int Micro_getNoise();
  /* Description: Get noise using electret micro                                */
  /*              connected to 5V                                               */
  /* input:       none                                                          */ 
  /* output:      return                                                        */                            
  /*                  = value between 0 (no noise) and 1023 (noising)           */
  /*                  = -1 if pin is not initialized                            */
  /* lib:         analogRead                                                    */

 private:
 int _Micro_Pin;
 
};

#endif
