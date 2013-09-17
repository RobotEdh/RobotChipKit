#include "Wire.h" // used for I2C protocol (lib)
#include "config.h"
#include "def.h"
#include "MultiWii.h"



void setup()
{
  
  Serial.begin(115200); // initialize serial port
  MultiWii_setup();

}


void loop()
{
  MultiWii_loop ();
  
 
  
  return;
}