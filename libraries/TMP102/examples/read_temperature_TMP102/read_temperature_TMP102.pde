#include <Wire.h>       // I2C protocol for Temperature sensor

#include <TMP102.h>
 
/* I2C interface is provided on pins:                                          */
/*         1 = Power +5V                                                       */
/*         2 = SCL1 in standard but SCL2 on MAX32 following change made by EDH */
/*         3 = SDA  in standard but SDA2 on MAX32 following change made by EDH */
/*         9 = Ground                                                          */



TMP102Class TMP102;     /* The Temperature sensor class */
double temperature;     /* temperature */

void setup()
{
  
  Serial.begin(9600); // initialize serial port
  TMP102.TMP102_init();
  
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

