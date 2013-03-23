#include <robot.h>
#include <WiFiShieldOrPmodWiFi.h>
#include <DNETcK.h>
#include <DWIFIcK.h>
#include <WiFiCmdRobot.h>
#include <motor.h>
#include <GP2Y0A21YK.h> // IR sensor
#include <CMPS03.h>     // Compas
#include <Servo.h>      // Servo
#include <TiltPan.h>   // Tilt&Pan
#include <LSY201.h>     // Camera
#include <SD.h>         // SD-Card
#include <sdcard.h>     // SD-Card
 

WiFiCmdRobot Server;    


void setup()
{
  
  Serial.begin(9600); // initialize serial port
  robot_begin(); 
  motor_begin(); 
  Server.WiFiCmdRobot_begin();

}


void loop()
{

  
 Server.WiFiCmdRobot_main();
  
  return;
}