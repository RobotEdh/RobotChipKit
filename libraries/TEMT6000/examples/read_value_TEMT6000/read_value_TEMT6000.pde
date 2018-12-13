#include <TEMT6000.h>

TEMT6000Class TEMT6000;

void setup()
{
  
  Serial.begin(9600); // initialize serial port
  
  TEMT6000.TEMT6000_init(); // initialize the default pin A2 connected to the sensor
 
}


void loop()
{
  int  value;
  long startTime, stopTime, elapsedTime;
  
  Serial.print(" --> read light value of TEMT6000: "); 
  
  startTime = micros();
  value = TEMT6000.TEMT6000_getLight();
  stopTime = micros();
  elapsedTime = stopTime - startTime; // take 5 us
  
  Serial.print("value between 0 (dark) and 1023 (bright): ");
  Serial.print(value); 
  Serial.print(" - startTime: ");
  Serial.print(startTime); 
  Serial.print(" - stopTime: ");
  Serial.print(stopTime); 
  Serial.print(" - elapsedTime: ");
  Serial.println(elapsedTime); 
 
  delay(500); //make it readable
  
}