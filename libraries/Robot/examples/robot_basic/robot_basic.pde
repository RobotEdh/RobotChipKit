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
 


uint16_t cmd[CMD_SIZE];
int16_t resp[RESP_SIZE];
int resp_len = 0;
  
void setup()
{
  int ret = SUCCESS;
  
  Serial.begin(9600); // initialize serial port
  ret = robot_begin(); 
  if (ret != SUCCESS) exit;
  ret = motor_begin(); 
  if (ret != SUCCESS) exit;

  Serial.println("Call command INFOS");
  cmd[0] = CMD_INFOS;
  cmd[1] = 0;
  cmd[2] = 0;
  ret = CmdRobot (cmd, resp, &resp_len);
       
  Serial.print("Call CmdRobot, ret: ");
  Serial.println(ret);
  
  
  Serial.println("Call command PICTURE");
  cmd[0] = CMD_PICTURE;
  cmd[1] = 0;
  cmd[2] = 0;
  ret = CmdRobot (cmd, resp, &resp_len);
       
  Serial.print("Call CmdRobot, ret: ");
  Serial.println(ret);
  
  Serial.println("Call command ALERT");
  cmd[0] = CMD_ALERT;
  cmd[1] = 99;
  cmd[2] = 0;
  ret = CmdRobot (cmd, resp, &resp_len);
      
  Serial.print("Call CmdRobot, ret: ");
  Serial.println(ret);

}


void loop()
{
  int ret = SUCCESS;
 
  Serial.println("Call command GO for 30 seconds with no PID");
  cmd[0] = CMD_GO;
  cmd[1] = 30;  // 30 seconds
  cmd[2] = 0;   // no PID   
  ret = CmdRobot (cmd, resp, &resp_len);
  
  Serial.print("Call CmdRobot, ret: ");
  Serial.println(ret);

     
 
}