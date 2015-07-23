#include <Wire.h>       // I2C protocol for Temperature sensor
#include <VirtualWire.h> // RF transmission library

#include <TMP102.h>
 
/* I2C interface is provided on pins:                                   */
/*         1 = Power +5V                                                */
/*         2 = SCL connected to J3-09 (SCL2/RA2)                        */
/*         3 = SDA connected to J3-11 (SDA2/RA3)                        */
/*         9 = Ground                                                   */

TMP102Class TMP102;     /* The Temperature sensor class */

void setup()
{ 
  Serial.begin(9600); // initialize serial port 9600 bits/secs
  vw_setup(2000);	 // initialize Virtual wire 2000 bits/secs
  
  TMP102.TMP102_init();  // Init I2C for the Temperature sensor
}


void loop()
{
  double temperature;     /* temperature */
  
  Serial.println(" --> read temperature in Celsius of TMP102: "); 

  temperature = TMP102.TMP102_read();
  
  Serial.print("value: ");
  Serial.println(value); 
 
  vw_send_float(temperature, 2, VW_TEMPERATURE_DATA_TYPE, 1); // source 1
  delay(5000); //make it readable
  
  return;
}

