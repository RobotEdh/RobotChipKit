#ifndef WiFiCmdRobot_h
#define WiFiCmdRobot_h

#include <WiFiShieldOrPmodWiFi.h>
#include <DNETcK.h>
#include <DWIFIcK.h>
#include <sdcard.h>
#include <SD.h> 

#define SUCCESS 0
#define FILE_OPEN_ERROR -1000
#define FILE_CLOSE_ERROR -1001

#define PAYLOAD_SIZE 80

#define WiFiConnectMacro() DWIFIcK::connect(szSsid, keySet, iWEPKey, &status)


class WiFiCmdRobot
{
  private:
  void printNumb(byte * rgb, int cb, char chDelim);
  void printMAC(MAC& mac);
  void printIP(IPv4& ip);
  int WiFiSendPicture (int16_t n); 
  int Cmd (String s); 
  int ReplyOK (void); 
  int ReplyKO (void); 
     
  public:
  int WiFiCmdRobot_begin (void);   
  int WiFiCmdRobot_main (void);
  
};



#endif
