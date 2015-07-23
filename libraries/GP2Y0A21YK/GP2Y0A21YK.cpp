/*
  GP2Y0A21YK.cpp - Library for communicating with IR sensor GP2Y0A21YK
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/

#include <GP2Y0A21YK.h>

const unsigned char transferFunctionLUT3V[]  =
// return distance in cm for an sensor connected to a power source of 3 volts
	{
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  //16
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  255, //32
	    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  255, //48
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  70,  70,  70,  70,  70,  //64
		 70,  70,  70,  70,  70,  60,  60,  60,  60,  60,  60,  60,  60,  60,  55,  55,  //80
		 55,  55,  55,  55,  55,  50,  50,  50,  50,  50,  50,  50,  50,  50,  45,  45,  //96
		 45,  45,  45,  45,  45,  45,  45,  45,  40,  40,  40,  40,  40,  40,  40,  40,  //112
		 40,  40,  40,  40,  40,  40,  35,  35,  35,  35,  35,  35,  35,  35,  35,  35,  //128
		 35,  35,  35,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  //144
		 30,  30,  30,  30,  30,  30,  25,  25,  25,  25,  25,  25,  25,  25,  25,  25,  //160
		 25,  25,  25,  25,  25,  25,  25,  25,  20,  20,  20,  20,  20,  20,  20,  20,  //176
		 20,  20,  20,  20,  15,  15,  15,  15,  15,  15,  15,  255, 255, 255, 255, 255, //192
		 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, //208
		 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, //224
		 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, //240
		 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 //256
	};

// Constructor
GP2Y0A21YKClass::GP2Y0A21YKClass ()
{
}

void GP2Y0A21YKClass::GP2Y0A21YK_init()
{
 
 return GP2Y0A21YK_init(GP2Y0A21YK_PIN_DEFAULT);
}

void GP2Y0A21YKClass::GP2Y0A21YK_init(int pin)
{
 
 this->_GP2Y0A21YK_Pin = pin;
 pinMode(this->_GP2Y0A21YK_Pin, INPUT);   // define pin as input
 
 return; 
}  

  
int GP2Y0A21YKClass::GP2Y0A21YK_getDistanceCentimeter()
{
// return average distance or -1 if out of range (<15 cm or > 70 cm)

 int average = 5;  // average of 10 measurements
 int sum = 0;
 int nb = 0;
 int val;

 if (!this->_GP2Y0A21YK_Pin) return -1;            // pin is not initialized

 for (int i=0;i<average;i++)
 {
   val = (*((char *) (transferFunctionLUT3V + (int)(analogRead(this->_GP2Y0A21YK_Pin)/4))));
        
   if (val > 0) {
      sum=sum + val;
      nb ++;
   }
 }

 if (nb > 0) return((int)(sum/nb));
 else        return -2;   
}