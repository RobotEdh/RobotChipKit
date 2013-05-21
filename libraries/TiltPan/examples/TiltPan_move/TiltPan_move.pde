#include <Servo.h>      // Servo

#include <TiltPan.h>    // Tilt&Pan
 

void setup()
{
  
  Serial.begin(9600); // initialize serial port

  // initialize the Tilt&Pan servos  
  TiltPan_begin(HSERVO_Pin, VSERVO_Pin);
  Serial.println("Init Tilt&Pan servos OK");
  
}


void loop()
{
  int x;
  int y;
  
  Serial.println("Neutral position");
  TiltPan_move(90, 90);
  delay(5000);
  
  Serial.println(" --> Start move Tilt&Pan Y and X"); 
  for (x=0;x<=180;x=x+10)
  {
     for (y=0;y<=180;y=y+10)
     {
        Serial.print("X: "); 
        Serial.print(x);
        Serial.print("\tY: "); 
        Serial.println(y);
        TiltPan_move(x, y);
        delay(1000);
     }
  } 

  Serial.println(" --> Start move Tilt&Pan X and Y");  
  for (y=0;y<=180;y=y+10)
  {
     for (x=0;x<=180;x=x+10)
     {
        Serial.print("X: "); 
        Serial.print(x);
        Serial.print("\tY: "); 
        Serial.println(y);
        TiltPan_move(x, y);
        delay(1000);
     }
  } 
}

