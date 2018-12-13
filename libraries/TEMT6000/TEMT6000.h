/*
  TEMT6000.h -  Library for communicating with Ambient Light Sensor TEMT6000
  Created by EDH, December 30, 2014.
  Released into the public domain.
*/


#ifndef TEMT6000_h
#define TEMT6000_h

#include <Arduino.h> // used for pin definition

#define TEMT6000_PIN_DEFAULT A2  // Default Brightness Sensor TEMT6000 analogic pin  A2
/* Analogic interface is provided on pin A2 */
/* Power +5V is set on pin VCC              */
/* Ground    is set on pin GND              */



class TEMT6000Class
{
  public:
    
  TEMT6000Class();
 
  void TEMT6000_init(); 
  void TEMT6000_init(int pin);
  /* Description: Initialize the Ambient Light Sensor TEMT6000                  */                                            
  /* input:       pin                                                           */ 
  /*                  = pin connected to the Ambient Light Sensor TEMT6000      */                       
  /* output:      none                                                          */
  /* lib:         none                                                          */
  
  int TEMT6000_getLight();
  /* Description: Get light using Ambient Light Sensor TEMT6000                 */
  /*              connected to 5V                                               */
  /* input:       none                                                          */ 
  /* output:      return                                                        */                            
  /*                  = value between 0 (very dark ) and 1023 (very bright)     */
  /*                  = -1 if pin is not initialized                            */
  /* lib:         analogRead                                                    */

 private:
 int _TEMT6000_Pin;
  
};

#endif
