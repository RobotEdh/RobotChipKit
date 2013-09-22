#include "Wire.h" // used for I2C protocol (lib)
#include "config.h"
#include "def.h"
#include "PMultiWii.h"



void setup()
{
  
  MultiWii_setup();

}


void loop()
{
  MultiWii_loop ();
  
 
  
  return;
}