#ifndef WiFiCmdRobot_h
#define WiFiCmdRobot_h

#include <WiFiShieldOrPmodWiFi.h>
#include <DNETcK.h>
#include <DWIFIcK.h>


#define SUCCESS 0
#define FILE_OPEN_ERROR -1000
#define FILE_CLOSE_ERROR -1001

class WiFiCmdRobot
{
  private:
  void printNumb(byte * rgb, int cb, char chDelim);
  void printMAC(MAC& mac);
  void printIP(IPv4& ip);
  int WiFiSendPicture (int n);  
     
  public:
  void WiFiCmdRobot_begin (void);   
  void WiFiCmdRobot_main (void);
  
};



#endif
