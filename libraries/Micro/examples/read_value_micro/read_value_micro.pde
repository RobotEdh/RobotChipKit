#include <Micro.h>

MicroClass Micro;

void setup()
{
  
  Serial.begin(9600); // initialize serial port
  
  Micro.Micro_init(); // initialize the default pin A0 connected to the sensor
 
}


void loop()
{
  int  value;
  long startTime, stopTime, elapsedTime;
  
  Serial.print(" --> read noise value of Micro: "); 
  
  startTime = micros();
  value = Micro.Micro_getNoise();
  stopTime = micros();
  elapsedTime = stopTime - startTime; // take 5 us
  
  Serial.print("value between 0 (no noise) and 1023 (noising): ");
  Serial.print(value); 
  Serial.print(" - startTime: ");
  Serial.print(startTime); 
  Serial.print(" - stopTime: ");
  Serial.print(stopTime); 
  Serial.print(" - elapsedTime: ");
  Serial.println(elapsedTime); 
 
  delay(500); //make it readable
  
}