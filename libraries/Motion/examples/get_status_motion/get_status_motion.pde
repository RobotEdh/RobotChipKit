#include <Motion.h>

MotionClass Motion;

void setup()
{
  
  Serial.begin(9600); // initialize serial port
  
  Motion.Motion_init(); // initialize the default pin 3 connected to the sensor
 
}


void loop()
{
  int  status;
  long startTime, stopTime, elapsedTime;
  
  Serial.print(" --> get status of infrared motion sensor: "); 
  
  startTime = micros();
  status = Motion.Motion_status();
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