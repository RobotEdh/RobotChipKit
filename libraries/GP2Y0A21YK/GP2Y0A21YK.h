/*
  GP2Y0A21YK.h - Library for commmunicationg with IR sensor GP2Y0A21YK
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/


#ifndef GP2Y0A21YK_h
#define GP2Y0A21YK_h

#include <wiring.h> // used for analogic read function (core lib)

void GP2Y0A21YK_init(int pin);  //pin connected to the sensor;
int GP2Y0A21YK_getDistanceCentimeter(int pin); //pin connected to the sensor;

#endif
