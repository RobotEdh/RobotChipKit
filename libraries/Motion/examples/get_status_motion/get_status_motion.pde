#include <Motion.h>


void setup()
{
  
  Serial.begin(9600); // initialize serial port
  
  Motion_init(Motion_Pin); // initialize the pin connected to the sensor
 
}


void loop()
{
  int  status;
  long startTime, stopTime, elapsedTime;
  
  Serial.print(" --> get status of infrared motion sensor: "); 
  
  startTime = micros();
  status = Motion_status(Motion_Pin);
  stopTime = micros();
  elapsedTime = stopTime - startTime; // take 1 us
  
  Serial.print("status 0 (no motion) / 1 (motion): ");
  Serial.print(status); 
  Serial.print(" - startTime: ");
  Serial.print(startTime); 
  Serial.print(" - stopTime: ");
  Serial.print(stopTime); 
  Serial.print(" - elapsedTime: ");
  Serial.println(elapsedTime); 
 
  delay(500); //make it readable
  
}