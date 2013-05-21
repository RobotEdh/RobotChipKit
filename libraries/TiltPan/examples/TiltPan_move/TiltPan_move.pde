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
  
  Serial.println(" --> Start move Tilt&Pan Y and X"); 
  for (x=-180;x<=180;x=x+10)
  {
     for (y=-180;y<=180;y=y+10)
     {
        Serial.print("move Tilt&Pan X Y: "); 
        Serial.print(x);
        Serial.println(y);
        TiltPan_move(x, y);
     }
  } 

  Serial.println(" --> Start move Tilt&Pan X and Y");  
  for (y=-180;y<=180;y=y+10)
  {
     for (x=-180;x<=180;x=x+10)
     {
        Serial.print("move Tilt&Pan X Y: "); 
        Serial.print(x);
        Serial.println(y);
        TiltPan_move(x, y);
     }
  } 
}

