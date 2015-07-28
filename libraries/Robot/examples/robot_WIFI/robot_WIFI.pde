#include <Wire.h>       // I2C protocol
#include <Servo.h>      // Servo
#include <SD.h>         // SD-Card
#include <sdcard.h>     // SD-Card
#include <WiFiShieldOrPmodWiFi.h>
#include <DNETcK.h>
#include <DWIFIcK.h>

#include <robot.h>
#include <WiFiCmdRobot.h>
#include <motor.h>
#include <GP2Y0A21YK.h>        // IR sensor
#include <CMPS03.h>            // Compass
#include <TMP102.h>            // Temperature sensor
#include <TiltPan.h>           // Tilt&Pan
#include <LSY201.h>            // Camera
#include <LiquidCrystal_I2C.h> // LCD
#include <Motion.h>            // Motion
#include <TEMT6000.h>          // Lux
#include <Micro.h>             // Micro
#include <MPU6050.h>           // MPU6050
 

WiFiCmdRobot Server;    


void setup()
{
  int ret = SUCCESS;
  
  Serial.begin(9600); // initialize serial port
  ret = robot_begin(); 
  if (ret != SUCCESS) exit;
  ret = motor_begin(); 
  if (ret != SUCCESS) exit;
  ret = Server.WiFiCmdRobot_begin();
  if (ret != SUCCESS) exit;

}


void loop()
{
  int ret = SUCCESS;
  
  ret = Server.WiFiCmdRobot_main();
  if (ret != SUCCESS) exit;
  
  return;
}