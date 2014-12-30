/*
  TEMT6000.h -  Library for communicating with Ambient Light Sensor TEMT6000
  Created by EDH, December 30, 2014.
  Released into the public domain.
*/


#ifndef TEMT6000_h
#define TEMT6000_h

#include <wiring.h> // used for analogic read function (core lib)

#define TEMT6000_Pin A0   // Ambient Light Sensor TEMT6000 analogic pin J5-01 A0 (PGED1/AN0/CN2/RB0)   Use ADC module channel 2
/* Analogic interface is provided on pin A0 */
/* Power +5V is set on pin VCC              */
/* Ground    is set on pin GND              */


void TEMT6000_init(int pin);
/* Description: Initialize the Ambient Light Sensor TEMT6000                  */                                            
/* input:       pin                                                           */ 
/*                  = pin connected to the Ambient Light Sensor TEMT6000      */                       
/* output:      none                                                          */
/* lib:         none                                                          */

int TEMT6000_getLight(int pin);
/* Description: Get light using Ambient Light Sensor TEMT6000                 */
/*              connected to 5V                                               */
/* input:       pin                                                           */ 
/*                  = pin connected to the Ambient Light Sensor TEMT6000      */    
/* output:      return                                                        */                            
/*                  = value between 0 (very dark ) and 1023 (very bright)     */
/* lib:         analogRead                                                    */

#endif
