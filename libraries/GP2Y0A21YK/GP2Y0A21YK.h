/*
  GP2Y0A21YK.h - Library for commmunicationg with IR sensor GP2Y0A21YK
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/


#ifndef GP2Y0A21YK_h
#define GP2Y0A21YK_h

#include <WProgram.h> // used for pin definition
#include <wiring.h>   // used for analogic read function (core lib)

#define GP2Y0A21YK_PIN_DEFAULT A0   // IR sensor GP2Y0A21YK analogic pin J5-01 A0 (PGED1/AN0/CN2/RB0)   Use ADC module channel 2
/* Analogic interface is provided on pin V0 */
/* Power +3V is set on pin VCC              */
/* Ground    is set on pin GND              */


class GP2Y0A21YKClass
{
  public:
    
  GP2Y0A21YKClass();
 
  void GP2Y0A21YK_init();
  void GP2Y0A21YK_init(int pin);
  /* Description: Initialize the IR sensor GP2Y0A21YK                           */                                            
  /* input:       pin                                                           */ 
  /*                  = pin connected to the IR sensor GP2Y0A21YK               */                       
  /* output:      none                                                          */
  /* lib:         none                                                          */

  int GP2Y0A21YK_getDistanceCentimeter();
  /* Description: Get distance in centimeter using the IR sensor GP2Y0A21YK     */
  /*              connected to 3V                                               */
  /* input:       none                                                          */ 
  /* output:      return                                                        */ 
  /*                  = -1 if pin is not initialized                            */         
  /*                  = -2 if distance not determined                           */ 
  /*                  = distance in centimeter otherwise                        */
  /* lib:         analogRead                                                    */
 
 private:
 int _GP2Y0A21YK_Pin;
  
};

#endif
