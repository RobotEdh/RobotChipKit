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
  
  ret = mainRobot();

  ret = turn (30);
  Serial.print(" \n--> Turn 30, ret: "); 
  Serial.print(ret);  
  delay(5000); //make it readable
    
  ret = turn (-50);
  Serial.print(" \n--> Turn -50, ret: "); 
  Serial.print(ret);  
  delay(5000); //make it readable
}