#include <Wire.h>       // I2C protocol
#include <Servo.h>      // Servo
#include <SD.h>         // SD-Card
#include <sdcard.h>     // SD-Card

#include <robot.h>
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
#include <WiFiShieldOrPmodWiFi.h>
#include <DNETcK.h>
#include <DWIFIcK.h>

#include <WiFiRobot.h>
 


uint16_t cmd[CMD_SIZE];
uint16_t resp[RESP_SIZE];
uint8_t resp_len = 0;
  
void setup()
{
  int ret = SUCCESS;
  
  Serial.begin(9600); // initialize serial port
  ret = robot_begin(); 
  if (ret != SUCCESS) exit;

  Serial.println("Call command INFOS");
  cmd[0] = CMD_INFOS;
  cmd[1] = 0;
  cmd[2] = 0;
  ret = robot_command (cmd, resp, &resp_len);
  Serial.print("Call robot_command, ret: ");
  Serial.println(ret);
  
  
  Serial.println("Call command PICTURE");
  cmd[0] = CMD_PICTURE;
  cmd[1] = 0;
  cmd[2] = 0;
  ret = robot_command (cmd, resp, &resp_len);
  Serial.print("Call robot_command, ret: ");
  Serial.println(ret);
  
 }


void loop()
{
  int ret = SUCCESS;
 
  ret = robot_main(); 
 
  Serial.print("Call robot_main, ret: ");
  Serial.println(ret);

     
 
}