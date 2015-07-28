#ifndef WiFiCmdRobot_h
#define WiFiCmdRobot_h

#include <WiFiShieldOrPmodWiFi.h>
#include <DNETcK.h>
#include <DWIFIcK.h>
#include <sdcard.h>
#include <SD.h> 

#define SUCCESS              0
#define FILE_OPEN_ERROR  -1000
#define FILE_CLOSE_ERROR -1001


#define NO_ALERT          0
#define ALERT_MOTION      1
#define ALERT_LUX         2
#define ALERT_TEMPERATURE 3
#define ALERT_NOISE       4
#define ALERT_ACC_Z       5

#define NB_LUX 6
#define VAR_LUX 100
#define NB_TEMPERATURE 6
#define VAR_TEMPERATURE 2
#define THR_NOISE 520
#define THR_ACC_Z 2

#define PAYLOAD_SIZE 80

#define MOTION_PIN 28   // Infrared motion sensor digital pin 28


class WiFiCmdRobot
{
  private:
  void printNumb(byte * rgb, int cb, char chDelim);
  void printMAC(MAC& mac);
  void printIP(IPv4& ip);
  int WiFiSendPicture (int16_t n);
  int WiFiSendFastPicture (int16_t n);
  int WiFiSendInfos (void);  
  int Cmd (String s); 
  int ReplyOK (void); 
  int ReplyKO (void);
  int Check_Alert (void);
     
  public:
  int WiFiCmdRobot_init_Alerts (void);
  int WiFiCmdRobot_init_SDCard (void);
  int WiFiCmdRobot_scan (void); 
  int WiFiCmdRobot_init_TCPIP (void);  
  int WiFiCmdRobot_begin (void);   
  int WiFiCmdRobot_main (void);
  
};

#endif
