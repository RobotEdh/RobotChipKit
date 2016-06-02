#ifndef WiFiRobot_h
#define WiFiRobot_h

#include <WiFiShieldOrPmodWiFi.h>
#include <DNETcK.h>
#include <DWIFIcK.h>

#include <sdcard.h>
#include <SD.h> 

#define SUCCESS              0
#define FILE_OPEN_ERROR  -1000
#define FILE_CLOSE_ERROR -1001


#define PAYLOAD_SIZE 80

class WiFiRobotClass
{
  private:
  void printNumb(byte * rgb, int cb, char chDelim);  
  void printIP(IPv4& ip);
  void printMAC(MAC& mac);
  int WiFiRobot_init_TCPIP(void);
  int WiFiRobot_scan (void);
  int WiFiSendFastPicture (int16_t n);
     
  public:
  int WiFiRobot_begin (void);
  int WiFiRobot_SendAlert (char *infos, int n);
  int WiFiSendInfos (char *infos);
};

#endif
