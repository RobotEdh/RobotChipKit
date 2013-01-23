#include <motor.h>
#include <GP2Y0A21YK.h> // IR sensor
#include <CMPS03.h>     // Compas
#include <Servo.h>      // Servo
#include <LSY201.h>     // Camera
#include <SD.h>         // SD-Card
#include <XBee.h>       // XBee
#include <xBeeTools.h>  //XBee tools

void setup()
{ 
  Serial.begin(9600); // initialize serial port
  motor_init(); 
}

void loop()
{
  int ret;
 
  Serial.print(" \n--> stop\n");  
  stop();
  delay(5000); //make it readable
        
  Serial.print("--> check_around\n");  
  ret = check_around(); 
  Serial.print("--> LEFT_DIRECTION 1, RIGHT_DIRECTION 2, ret: "); 
  Serial.print(ret);  
  delay(500); //make it readable
      
  Serial.print("\n--> start_forward\n"); 
  start_forward();

  
  Serial.print("--> go 1000 ticks\n"); 
  delay(500); //make it readable   
  ret = go(100,0);
  
 
  if (ret == OBSTACLE ) {
      Serial.print("--> 0bstacle"); 
  }
  else
  {
      Serial.print(" --> No obstacle, direction: "); 
      Serial.print(ret);  
  }

  ret = turn (30);
  Serial.print(" \n--> Turn 30, ret: "); 
  Serial.print(ret);  
  delay(5000); //make it readable
    
  ret = turn (-50);
  Serial.print(" \n--> Turn -50, ret: "); 
  Serial.print(ret);  
  delay(5000); //make it readable
}