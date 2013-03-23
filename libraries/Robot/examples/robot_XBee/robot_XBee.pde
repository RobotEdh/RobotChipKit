#include <robot.h>
#include <motor.h>
#include <GP2Y0A21YK.h> // IR sensor
#include <CMPS03.h>     // Compas
#include <Servo.h>      // Servo
#include <TiltPan.h>   // Tilt&Pan
#include <LSY201.h>     // Camera
#include <SD.h>         // SD-Card
#include <sdcard.h>     // SD-Card
#include <XBee.h>       // XBee
#include <XBeeTools.h>  // XBee tools
#include <XBeeCmdRobot.h>
void setup()
{ 
  Serial.begin(9600); // initialize serial port
  robot_begin(); 
  motor_begin(); 
  
}

void loop()
{
  int ret;
  
  ret = XBeeCmdRobot();
}