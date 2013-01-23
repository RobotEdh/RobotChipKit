#include <GP2Y0A21YK.h>
#include <wiring.h>

/* Analogic interface pin A0 is provided on pin V0 */
/* Power +3V is set on pin VCC        */
/* Ground    is set on pin GND        */



#define GP2Y0A21YK_Pin 14   // IR sensor GP2Y0A21YK analogic pin J7-01 A0 (C2IN-/AN2/SS1/CN4/RB2)   Use ADC module channel 2
/* Analogic interface is provided on pin V0 */
/* Power +3V is set on pin VCC              */
/* Ground    is set on pin GND              */

void setup()
{
  
  Serial.begin(9600); // initialize serial port
  GP2Y0A21YK_init(GP2Y0A21YK_Pin); // initialize the pin connecter to the sensor
 
}


void loop()
{
  int  distance;
  long startTime, stopTime, elapsedTime;
  
  Serial.print(" --> read distance from GP2Y0A21K"); 
  
  startTime = micros();
  distance = GP2Y0A21YK_getDistanceCentimeter(GP2Y0A21YK_Pin);
  stopTime = micros();
  elapsedTime = stopTime - startTime; // take 26 us
  
  if (distance == -1) {
    Serial.print("\nDistance outside range: < 15 cm or > 70 cm"); 
    Serial.print(" - elapsedTime: ");
    Serial.print(elapsedTime);   
  }
  else
  {  
    Serial.print("\nDistance in cm: ");
    Serial.print(distance); 
    Serial.print(" - startTime: ");
    Serial.print(startTime); 
    Serial.print(" - stopTime: ");
    Serial.print(stopTime); 
    Serial.print(" - elapsedTime: ");
    Serial.print(elapsedTime); 
  }
  delay(500); //make it readable
  
  return;
}