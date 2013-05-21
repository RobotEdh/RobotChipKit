#include <Wire.h>       // I2C protocol for Compass

#include <CMPS03.h>
 
/* I2C interface is provided on pins:                                   */
/*         1 = Power +5V                                                */
/*         2 = SCL connected to J3-09 (SCL2/RA2)                        */
/*         3 = SDA connected to J3-11 (SDA2/RA3)                        */
/*         9 = Ground                                                   */



CMPS03Class CMPS03;     /* The Compass class */
int revision;  /* revison number: 16 during testing this lib */
int direction; /* direction between 0-254, 0: North */

void setup()
{
  
  Serial.begin(9600); // initialize serial port
  CMPS03.CMPS03_begin();
  
  revision = CMPS03.CMPS03_revision();
  if (revision < 0) {
     Serial.print("\nError I2c: ");
     Serial.print(revision);    
  }
  else
  {  
     Serial.print("\nRevision: ");
     Serial.print(revision);
  }  
  delay(500); //make it readable 
}


void loop()
{

  
  Serial.print(" --> read direction from CMPS03"); 

  direction = CMPS03.CMPS03_read();
  if (direction < 0) {
     Serial.print("\nError I2c: ");
     Serial.print(direction);    
  }
  else
  {  
  Serial.print("\nDirection: ");
  Serial.print(direction); 
  }  
 
  delay(500); //make it readable
  
  return;
}

