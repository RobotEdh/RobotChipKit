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
}