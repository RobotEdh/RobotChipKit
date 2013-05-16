#include <TMP102.h>
 
/* I2C interface is provided on pins                                */
/*         2 = SCL connected to A5/J7-11(SCL1/RG2) selected by JP8  */
/*         3 = SDA connected to A4/J7-09(SDA1/RG3) selected by JP6  */
/* Power +5V is set on pin 1                                        */
/* Ground    is set on pin 9                                        */



TMP102Class TMP102;     /* The Temperature sensor class */
double temperature;     /* temperature */

void setup()
{
  
  Serial.begin(9600); // initialize serial port
  TMP102.TMP102_begin();
  
}


void loop()
{

  
  Serial.print(" --> read temperature in Celsius from TMP102"); 

  temperature = TMP102.TMP102_read();
  
  Serial.print("\ttemperature: ");
  Serial.println(temperature); 
 
  delay(5000); //make it readable
  
  return;
}

