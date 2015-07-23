#include <WiFiCmdRobot.h>
#include <robot.h>
#include <LiquidCrystal_I2C.h> // LCD
#include <Motion.h>            // Motion

extern LiquidCrystal_I2C lcd;

// SD variables
extern SdFile root;   // SD Root
SdFile FilePicture;   // SD File


// Specify the SSID
const char * szSsid = "WIFICOTEAU2";

// WPA2 key
//You can find a tool to pre-calculate your PSK key at http://www.wireshark.org/tools/wpa-psk.html.
//kitesurf9397+ WIFICOTEAU2 => e0c1337f19334617316c4abdeed61c47e096e2bfa87cdb3db93864950c4a2295
DWIFIcK::WPA2KEY key = {0xe0 ,0xc1 ,0x33 ,0x7f ,0x19 ,0x33 ,0x46 ,0x17 ,0x31 ,0x6c ,0x4a ,0xbd ,0xee ,0xd6 ,0x1c ,0x47 ,0xe0 ,0x96 ,0xe2 ,0xbf ,0xa8 ,0x7c ,0xdb ,0x3d ,0xb9 ,0x38 ,0x64 ,0x95 ,0x0c ,0x4a ,0x22 ,0x95};

// Specify this IP+Port
IPv4                 this_ipServer   = {192,168,0,15};
const unsigned short this_portServer = 44300;

// Specify external server IP+Port
const char *         external_ipServer   = "192.168.0.18";
const unsigned short external_portServer = 80;

TcpServer tcpServer;
TcpClient tcpClient;   
DNETcK::STATUS status;
    
byte rgbRead[1024];
int cbRead = 0;
int count = 0;
  
String szcmd;
uint16_t cmd[CMD_SIZE];
uint16_t cmd_GO[CMD_SIZE];
uint16_t t_GO = 10;     // 10s max for GO command
int16_t resp[RESP_SIZE];
int resp_len = 0;
   
uint32_t timeout_infos = 10; // 10s
uint32_t t_infos = 0;

MotionClass Motion;   // The Motion class

int temperature = 0;
int previous_temperature = 0;
int tab_temperature[NB_TEMPERATURE] = {0};
unsigned long avg_temperature = 0;

int lux = 0;
int previous_lux = 0;
int tab_lux[NB_LUX] = {0};
unsigned long avg_lux = 0;

int WiFiCmdRobot::WiFiCmdRobot_init_Alerts() {
    int ret=SUCCESS;
 
    Serial.println("Begin WiFiCmdRobot_init_Alerts");
  
    // initialize Motion   
    Motion.Motion_init(MOTION_PIN); // initialize the pin connected to the sensor
    Serial.println("Init Motion OK");

    Serial.println("End WiFiCmdRobot_init_Alerts");
   
    return SUCCESS;   // Continue if error  
}


int WiFiCmdRobot::WiFiCmdRobot_init_SDCard() {
    int ret=SUCCESS;
 
    Serial.println("Begin WiFiCmdRobot_init_SDCard");
  
    // initialize the SD-Card    
    ret = initSDCard();
    if (ret != SUCCESS)
    {  
        Serial.print("Error Init SD-Card, error: ");
        Serial.println(ret);
    }                                                                    
    else
    {
        Serial.println("Init SD-Card OK");
        Serial.println("");
    }
    
    // get infos from SD-Card  
    ret=infoSDCard();
    if (ret != SUCCESS)
    {  
        Serial.print("Error Infos SD-Card, error: ");
        Serial.println(ret);
    }

    Serial.println("End WiFiCmdRobot_init_SDCard");
   
    return SUCCESS;   // Continue if error  
}


int WiFiCmdRobot::WiFiCmdRobot_scan() {
    int cNetworks = 0;
    int iNetwork = 0; 
    int ret=SUCCESS;
    
    Serial.println("Begin WiFiCmdRobot_scan");
      
    // start a scan
    DWIFIcK::beginScan();
    while (1)
    {
            // every pass through loop(), keep the stack alive
            DNETcK::periodicTasks(); 
            if(DWIFIcK::isScanDone(&cNetworks, &status))
            {
                Serial.println("Scan Done");
                break;
            }
            else if(DNETcK::isStatusAnError(status))
            {
                Serial.print("Scan Failed, error: ");
                Serial.println(DNETcK::getError(status));
                return -1;
            }
            
    }        
    
    while (1)
    {            
            // every pass through loop(), keep the stack alive
            DNETcK::periodicTasks();            
            if(iNetwork < cNetworks)
            {
                DWIFIcK::SCANINFO scanInfo;
                int j = 0;

                if(DWIFIcK::getScanInfo(iNetwork, &scanInfo))
                {
                    Serial.print("Scan info for index: ");
                    Serial.println(iNetwork, DEC);

                    Serial.print("SSID: ");
                    Serial.println(scanInfo.szSsid);

                    Serial.print("Secuity type: ");
                    Serial.println(scanInfo.securityType, DEC);
                    switch(scanInfo.securityType)
                    {
                           case DWIFIcK::WF_SECURITY_OPEN:
                                Serial.println("SECURITY OPEN");
                                break;
                           case DWIFIcK::WF_SECURITY_WEP_40:
                                Serial.println("WF SECURITY WEP 40");
                                break;
                           case DWIFIcK::WF_SECURITY_WEP_104:
                                Serial.println("SECURITY WEP 104");
                                break;
                           case DWIFIcK::WF_SECURITY_WPA_WITH_KEY:
                                Serial.println("SECURITY WPA WITH KEY");
                                break;
                           case DWIFIcK::WF_SECURITY_WPA_WITH_PASS_PHRASE:
                                Serial.println("SECURITY WPA WITH PASS PHRASE");
                                break;
                           case DWIFIcK::WF_SECURITY_WPA2_WITH_KEY:
                                Serial.println("SECURITY WPA2 WITH KEY");
                                break;
                           case DWIFIcK::WF_SECURITY_WPA2_WITH_PASS_PHRASE:
                                Serial.println("SECURITY WPA2 WITH PASS PHRASE");
                                break;
                           case DWIFIcK::WF_SECURITY_WPA_AUTO_WITH_KEY:
                                Serial.println("SECURITY WPA AUTO WITH KEY");
                                break; 
                           case DWIFIcK::WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE:
                                Serial.println("SECURITY WPA AUTO WITH PASS PHRASE");
                                break;                                
                    }
                    
                    Serial.print("Channel: ");
                    Serial.println(scanInfo.channel, DEC);    

                    Serial.print("Signal Strength: ");
                    Serial.println(scanInfo.signalStrength, DEC);    

                    Serial.print("Count of supported bit rates: ");
                    Serial.println(scanInfo.cBasicRates, DEC);    

                    for( j= 0; j< scanInfo.cBasicRates; j++)
                    {
                        Serial.print("\tSupported Rate: ");
                        Serial.print(scanInfo.basicRates[j], DEC); 
                        Serial.println(" bps");
                    }

                    Serial.print("SSID MAC: ");
                    for(j=0; j<sizeof(scanInfo.ssidMAC); j++)
                    {
                        if(scanInfo.ssidMAC[j] < 16)
                        {
                            Serial.print(0, HEX);
                        }
                        Serial.print(scanInfo.ssidMAC[j], HEX);
                    }

                    Serial.print("\nBeacon Period: ");
                    Serial.println(scanInfo.beconPeriod, DEC);    

                    Serial.print("dtimPeriod: ");
                    Serial.println(scanInfo.dtimPeriod, DEC);    

                    Serial.print("atimWindow: ");
                    Serial.println(scanInfo.atimWindow, DEC); 
                    
                    Serial.println("");
                }
                else
                {
                    Serial.print("Unable to get scan info for iNetwork: ");
                    Serial.println(iNetwork, DEC);
                }

                iNetwork++;
            }
            else
            {
                break;
            }           
    }
    
    Serial.println("End WiFiCmdRobot_scan");
   
    return SUCCESS;   // Continue if error  
}


int WiFiCmdRobot::WiFiCmdRobot_init_TCPIP() {
  
    int conID = DWIFIcK::INVALID_CONNECTION_ID;
    int ret=SUCCESS;
    
    Serial.println("Begin WiFiCmdRobot_init_TCPIP");
 
    // First disconnect in case of previous connection still up
    DWIFIcK::disconnect(0);        
    
    // then connect
    if(conID = DWIFIcK::connect(szSsid, key, &status) != DWIFIcK::INVALID_CONNECTION_ID)
    {
          Serial.print("Connection Created, ConID = ");
          Serial.println(conID, DEC);
    }
    else
    {
          Serial.print("Unable to connection, status: ");
          Serial.print(status, DEC);
          Serial.print(", error: ");
          Serial.println(DNETcK::getError(status));

          return -2;
    }
 
    while (1)
    { 
           // every pass through loop(), keep the stack alive
           DNETcK::periodicTasks(); 
            
          // initialize the stack with a static IP
          DNETcK::begin(this_ipServer);
          if(DNETcK::isInitialized(&status))
          {
                Serial.println("IP Stack Initialized");
                break;
          }
          else if(DNETcK::isStatusAnError(status))
          {
                Serial.print("Error in initializing, status: ");
                Serial.print(status, DEC);
                Serial.print(", error: ");
                Serial.println(DNETcK::getError(status));
                return -3;
          }
    }

    // Start listening on server port
    tcpServer.startListening(this_portServer);

    Serial.println("End WiFiCmdRobot_init_TCPIP");
   
    return SUCCESS;    
}


int WiFiCmdRobot::WiFiCmdRobot_begin() {
    IPv4 ip;  
    int ret=SUCCESS;
    
    Serial.println("Begin WiFiCmdRobot_begin");
 
    Serial.println("Begin WIFI Init"); 
    lcd.clear();
    lcd.print("Begin WIFI Init");
          
    // Init SD Card
    ret = WiFiCmdRobot::WiFiCmdRobot_init_SDCard();
        
    //Init Alerts
    ret = WiFiCmdRobot::WiFiCmdRobot_init_Alerts();
        
    // Start a WIFI scan
    ret = WiFiCmdRobot::WiFiCmdRobot_scan();
        
    // Init TCP/IP stack
    ret = WiFiCmdRobot::WiFiCmdRobot_init_TCPIP();
    if (ret != SUCCESS)
    {  
        Serial.print("Error Init TCP/IP stack, error: ");
        Serial.println(ret);
        return ret;
    }
            
    // Get IP infos 
    DNETcK::getMyIP(&ip);
    Serial.print("My ");
    printIP(ip);
    Serial.println("");

    DNETcK::getGateway(&ip);
    Serial.print("Gateway ");
    printIP(ip);
    Serial.println("");

    DNETcK::getSubnetMask(&ip);
    Serial.print("Subnet mask: ");
    printNumb(ip.rgbIP, 4, '.');
    Serial.println("");

    DNETcK::getDns1(&ip);
    Serial.print("Dns1 ");
    printIP(ip);
    Serial.println("");

    DNETcK::getDns2(&ip);
    Serial.print("Dns2 ");
    printIP(ip);
    Serial.println("");
       
    // Get WIFI infos    
    DWIFIcK::CONFIGINFO configInfo;
    if(DWIFIcK::getConfigInfo(&configInfo))
    {
        Serial.println("WiFi config information");

        Serial.print("Scan Type: ");
        switch(configInfo.scanType)
        {
                   case DWIFIcK::WF_ACTIVE_SCAN:
                        Serial.println("ACTIVE SCAN");
                        break;
                   case DWIFIcK::WF_PASSIVE_SCAN:
                        Serial.println("PASSIVE SCAN");
                        break;                
        }

        Serial.print("Beacon Timeout: ");
        Serial.println(configInfo.beaconTimeout, DEC);

        Serial.print("Connect Retry Count: ");
        Serial.println(configInfo.connectRetryCount, DEC);

        Serial.print("Scan Count: ");
        Serial.println(configInfo.scanCount, DEC);

        Serial.print("Minimum Signal Strength: ");
        Serial.println(configInfo.minSignalStrength, DEC);

        Serial.print("Minimum Channel Time: ");
        Serial.println(configInfo.minChannelTime, DEC);

        Serial.print("Maximum Channel Time: ");
        Serial.println(configInfo.maxChannelTime, DEC);

        Serial.print("Probe Delay: ");
        Serial.println(configInfo.probeDelay, DEC);

        Serial.print("Polling Interval: ");
        Serial.println(configInfo.pollingInterval, DEC);                                                 
    }
    else
    {
        Serial.println("Unable to get WiFi config data");
        return -4; 
    }                  
   
    lcd.setCursor(0,1); 
    lcd.print("End   WIFI Init");
    
    Serial.println("");
    Serial.println("End WiFiCmdRobot Init");
    Serial.println("*********************");
    Serial.println("");

    return SUCCESS;                        
}

int WiFiCmdRobot::WiFiCmdRobot_main() {
    String stringRead;
    int conx = 0;
    int alert_status = 0;
    unsigned long timeout = 5; // 5s
    unsigned long start = 0;
    int ret=SUCCESS;

    digitalWrite(Led_Yellow, HIGH);               // turn on led yellow

    alert_status = WiFiCmdRobot::Check_Alert();  
    Serial.print("Alert: "); Serial.println(alert_status);
    
    if(alert_status)
    {
       Serial.println("Alert trigerred"); // robot has detected something...
       Serial.println("Call command ALERT");
  	   cmd[0] = CMD_ALERT;
       cmd[1] = alert_status;
       cmd[2] = 0;
       cmd_GO[0]= 0; //reset GO command  
       ret = CmdRobot (cmd, resp, &resp_len);
      
       Serial.print("Call CmdRobot, ret: ");
       Serial.println(ret);	
       
       // Send last 3 pictures
       for(int j=resp[10]-2; j<=resp[10]; j++)
       {
          Serial.println("Call WiFiSendFastPicture");
          ret = WiFiCmdRobot::WiFiSendFastPicture(j);
  
          if (ret != SUCCESS) {
              Serial.print("WiFiSendFastPicture error: "); Serial.println(ret);
              tcpClient.close();
              tcpServer.close();
              delay (5000);  // wait 5s before re-init
              ret = WiFiCmdRobot::WiFiCmdRobot_init_TCPIP();
          }
       }
            
       Serial.println("Call WiFiSendInfos");              
       ret= WiFiSendInfos();    
    
       if (ret != SUCCESS) {   
          Serial.print("WiFiSendInfos error: "); Serial.println(ret);
          tcpClient.close();
          tcpServer.close();
          delay (5000);  // wait 5s before re-init
          ret = WiFiCmdRobot::WiFiCmdRobot_init_TCPIP();
       }                        
    }
    else if(tcpServer.isListening(&status))
    {
       Serial.print("Listening on port: ");
       Serial.println(this_portServer, DEC);
       digitalWrite(Led_Yellow, HIGH);               // turn on led yellow
        
       // wait for a connection until timeout
       conx = 0;
       start = millis();
       while ((millis() - start < timeout*1000) && (conx == 0)) { 
           if((count = tcpServer.availableClients()) > 0)
           {
               Serial.print("Got ");
               Serial.print(count, DEC);
               Serial.println(" clients pending");
               conx = 1;
               // probably unneeded, but just to make sure we have
               // tcpClient in the  "just constructed" state
               tcpClient.close();             
           }
       }

       // accept the client 
       if((conx == 1) && (tcpServer.acceptClient(&tcpClient)))
       {
          Serial.println("Got a Connection");
          lcd.clear();
          lcd.print("Got a Connection");
       
          stringRead = "";
          while (tcpClient.isConnected())
          {
                if (tcpClient.available())
                {
                    char c = tcpClient.readByte();
                    if (c == '\n')
                    {
                          Serial.println (stringRead);
                          if (stringRead.startsWith("GET"))  
                          {
                                Serial.println("GET");
                                ret = Cmd (stringRead);
                                
                                if (ret == SUCCESS)
                                {
                                    ret = ReplyOK ();
                                }
                                else    
                                {
                                    cmd_GO[0] = 0; //reset GO command
                                    ret = ReplyKO ();
                                }
                                break;                             
 
                          }
                          else if (stringRead.startsWith("\r"))
                          {
                                // empty line => end
                                Serial.println("empty line => end");
                                break;
                          }                          
                          else
                          {
                                // no GET
                                Serial.println("no GET => ignore");
                          }
                          stringRead = "";              
                    }
                    else
                    {
                          stringRead += c;
                    }
                }
          } // end while            
       }
    }
    else if(DNETcK::isStatusAnError(status))
    {
       Serial.print("Error Listening: ");
       Serial.println(DNETcK::getError(status));
       Serial.println("Close Client and Server");
       tcpClient.close();
       tcpServer.close();
       delay (5000);  // wait 5s before re-init
       ret = WiFiCmdRobot::WiFiCmdRobot_init_TCPIP();
    }   
    
    if((cmd_GO[0] == CMD_GO) && (cmd_GO[1] > t_GO+(uint16_t)timeout))  // GO ongoing
    {
          Serial.println("Continue command GO");
          cmd_GO[1] = cmd_GO[1] - t_GO - (uint16_t)timeout;
       
          cmd[0] = cmd_GO[0];
          cmd[1] = t_GO;
          cmd[2] = cmd_GO[2];   
          ret = CmdRobot (cmd, resp, &resp_len);
       
          Serial.print("Call CmdRobot, ret: ");
          Serial.print(ret);
          Serial.print(" / resp_len: ");
          Serial.println(resp_len);        
    }
    else if(cmd_GO[0] == CMD_GO) // end GO
    {
          Serial.println("End command GO");
          cmd_GO[0] = 0; //reset GO command
      
          cmd[0] = CMD_STOP;  // Stop after GO
          cmd[1] = 0;
          cmd[2] = 0;   
          ret = CmdRobot (cmd, resp, &resp_len);
       
          Serial.print("Call CmdRobot, ret: ");
          Serial.print(ret);
          Serial.print(" / resp_len: ");
          Serial.println(resp_len);        
    }

    if(millis() > (t_infos+(timeout_infos*1000)))  // it's time to send infos
    {
          Serial.println("Call command INFOS");
          cmd[0] = CMD_INFOS;
          cmd[1] = 0;
          cmd[2] = 0;
          ret = CmdRobot (cmd, resp, &resp_len);
       
          Serial.print("Call CmdRobot, ret: ");
          Serial.println(ret);
          
          Serial.println("Call WIFI send INFOS");
          ret = WiFiCmdRobot::WiFiSendInfos();
          if (ret == SUCCESS) {
              t_infos = millis();
          }
          else {   
              Serial.print("WiFiSendInfos error: "); Serial.println(ret);
              tcpClient.close();
              tcpServer.close();
              delay (5000);  // wait 5s before re-init
              ret = WiFiCmdRobot::WiFiCmdRobot_init_TCPIP();
          }
    }
    
    if (resp_len > 8) {
         temperature = resp[7];
         lux         = resp[8];
    }     
    
    digitalWrite(Led_Yellow, LOW);               // turn off led yellow
          
    return SUCCESS;    
}


/***    void printIP(IPv4& ip)
 *
 *    Parameters:
 *          ip, the IP to print
 *              
 *    Return Values:
 *          None
 *
 *    Description: 
 *    
 *      A simple routine to print the IP address out
 *      on the serial monitor.
 * ------------------------------------------------------------ */
void WiFiCmdRobot::printIP(IPv4& ip)
{
    Serial.print("IP: ");
    WiFiCmdRobot::printNumb(ip.rgbIP, 4, '.');
}

/***    void printMAC(MAC& mac)
 *
 *    Parameters:
 *          mac, the MAC to print
 *              
 *    Return Values:
 *          None
 *
 *    Description: 
 *    
 *      A simple routine to print the MAC address out
 *      on the serial monitor.
 * ------------------------------------------------------------ */
void WiFiCmdRobot::printMAC(MAC& mac)
{
    Serial.print("MAC: ");
    WiFiCmdRobot::printNumb(mac.rgbMAC, 6, ':');
}

/***    void printNumb(byte * rgb, int cb, char chDelim)
 *
 *    Parameters:
 *          rgb - a pointer to a MAC or IP, or any byte string to print
 *          cb  - the number of bytes in rgb
 *          chDelim - the delimiter to use between bytes printed
 *              
 *    Return Values:
 *          None
 *
 *    Description: 
 *    
 *      A simple routine to print to the serial monitor bytes in either HEX or DEC
 *      If the delimiter is a ':' the output will be in HEX
 *      All other delimiters will be in DEC.
 *      Only HEX numbers get leading 0
 *      DEC number have no leading zeros on the number.
 * ------------------------------------------------------------ */
void WiFiCmdRobot::printNumb(byte * rgb, int cb, char chDelim)
{
    for(int i=0; i<cb; i++)
    {
        if(chDelim == ':' && rgb[i] < 16)
        {
            Serial.print(0, DEC);
        }  
        if(chDelim == ':')
        {
            Serial.print(rgb[i], HEX);
        }
        else
        {
            Serial.print(rgb[i], DEC);
        }  
        if(i < cb-1)
        {
            Serial.print(chDelim);  
        }
    }
}


int WiFiCmdRobot::Cmd (String s)
{
   int cmdTag;
   int And;
   String szparam[10];
   int iparam[10];
   int paramTag;
   int Start;
   int Semicolumn;
   int ret=SUCCESS;
   
   //GET /Robot?CMD=MOVE_TILT_PAN&PARAM=122;118; HTTP/1.1[\r][\n]
   //GET /Robot?CMD=INFOS HTTP/1.1[\r][\n]

   cmdTag = s.indexOf("CMD=");
   if (cmdTag > 0)
   {   
       Serial.print("CMD=");
       And = s.indexOf('&', cmdTag + 1);
       if (And == -1) And = s.indexOf(' ', cmdTag + 1);  
       szcmd = s.substring(cmdTag + 4, And);
       Serial.println(szcmd);

       paramTag = s.indexOf("PARAM=");
       if (paramTag > 0)
       {   
          Start = paramTag + 6;
          Semicolumn = s.indexOf('%3B', Start + 1); // semi column is encoded in URL
          for (int p=0; Semicolumn != -1; p++)
          {
               szparam[p] = s.substring(Start, Semicolumn - 2);
               iparam[p] = szparam[p].toInt();
               Serial.print("param");
               Serial.print(p);
               Serial.print("=");
               Serial.print(szparam[p]);
               Serial.print("/");
               Serial.println(iparam[p]);
               Start = Semicolumn + 1;
               Semicolumn = s.indexOf('%3B', Start + 1);
               
          }                                   
       }
       // CMD
       
       if (szcmd == "START")
       {
               cmd[0] = CMD_START;
               cmd[1] = iparam[0];
               cmd[2] = 0;
               cmd_GO[0]= 0; //reset GO command
       }        
       if (szcmd == "STOP")
       {
               cmd[0] = CMD_STOP;
               cmd[1] = 0;
               cmd[2] = 0;
               cmd_GO[0]= 0; //reset GO command
       }                                 
       if (szcmd == "INFOS")
       {
               cmd[0] = CMD_INFOS;
               cmd[1] = 0;
               cmd[2] = 0;
       }                                    
       if (szcmd == "PICTURE")
       {
               cmd[0] = CMD_PICTURE;
               cmd[1] = 0;
               cmd[2] = 0;
       }                                    
       if (szcmd == "TURN_RIGHT")
       {
               cmd[0] = CMD_TURN_RIGHT;
               cmd[1] = iparam[0];
               cmd[2] = 0;
       }                                    
       if (szcmd == "TURN_LEFT")
       {
               cmd[0] = CMD_TURN_LEFT;
               cmd[1] = iparam[0];
               cmd[2] = 0;
       }                                           
       if (szcmd == "CHECK_AROUND")
       {
               cmd[0] = CMD_CHECK_AROUND;
               cmd[1] = iparam[0];
               cmd[2] = 0;
       }                                    
       if (szcmd == "MOVE_TILT_PAN")
       {
               cmd[0] = CMD_MOVE_TILT_PAN;
               cmd[1] = iparam[0];
               cmd[2] = iparam[1];
       }                                    
       if (szcmd == "GO")
       {
               cmd_GO[0] = CMD_GO;
               cmd_GO[1] = iparam[0];
               cmd_GO[2] = iparam[1];
               cmd[0] = cmd_GO[0];
               cmd[1] = t_GO;
               cmd[2] = cmd_GO[2];
       } 
       
       ret = CmdRobot (cmd, resp, &resp_len);
       
       Serial.print("Call CmdRobot, ret: ");
       Serial.print(ret);
       Serial.print(" / resp_len: ");
       Serial.println(resp_len); 
       
       return ret;                                                                                           
   }
   else
   {
       // no CMD
       tcpClient.println("HTTP/1.1 400 Bad Request");
       tcpClient.println("Content-Type: text/html");
       tcpClient.println("Server: ChipkitEDH/0.1"); 
       tcpClient.println(); 
       
       return -400;                              
   }
   
}


int WiFiCmdRobot::ReplyOK ()
{
    int ret=SUCCESS; 
    
    if (cmd[0] == CMD_PICTURE)
    { 
          Serial.println("cmd[0] == CMD_PICTURE");
          if (tcpClient.isConnected()) Serial.println("tcpClient.isConnected");
          else Serial.println("tcpClient.is not Connected");
          tcpClient.println("HTTP/1.1 200 OK");
          tcpClient.println("Content-Type: application/octet-stream");
          tcpClient.println("Server: ChipkitEDH/0.1");                                             
          tcpClient.println();

          ret= WiFiSendPicture (resp[0]);
          
          if (ret != SUCCESS){  Serial.print("WiFiSendPicture error"); Serial.print(ret);}
    }
    else                                           
    {                               
          tcpClient.println("HTTP/1.1 200 OK");
          for(int i=0; i<resp_len; i++)
          {
              tcpClient.print("Field ");
              tcpClient.print(String(i));
              tcpClient.print(":");
              tcpClient.print(String((int)resp[i]));
              tcpClient.println(";");
         }
         tcpClient.println("Content-Type: text/html");
         tcpClient.println("Server: ChipkitEDH/0.1");                                             
         tcpClient.println();
    }
    
    return ret;    
}

int WiFiCmdRobot::ReplyKO ()
{
     tcpClient.println("HTTP/1.1 500 Internal Server Error");
     tcpClient.println("Content-Type: text/html");
     tcpClient.println("Server: ChipkitEDH/0.1");                                   
     tcpClient.println();
     
     return SUCCESS;
                                  
}


int WiFiCmdRobot::WiFiSendPicture (int16_t n)
{
  int ret=SUCCESS;
  int16_t nbytes; 
  uint8_t buf[PAYLOAD_SIZE];
  char filename[12+1];
 
  Serial.print("n: ");
  Serial.println(n);
  
  // Open the file
  sprintf(filename, "PICT%02d.jpg", n);
  if (!FilePicture.open(&root, filename, O_READ)) return FILE_OPEN_ERROR;  

  Serial.print("Open file: ");
  Serial.println(filename);

  // read from the file until there's nothing else in it:
  while ((nbytes = FilePicture.read(buf, sizeof(buf))) > 0 && ret == SUCCESS) {
       for (unsigned int idx = 0; idx<nbytes;idx++)
       {
         tcpClient.print(buf[idx]);
       }
 
  }// while
  
  //Close file
  if (!FilePicture.close()) return FILE_CLOSE_ERROR;  
  
  return SUCCESS;
}

int WiFiCmdRobot::WiFiSendFastPicture (int16_t n)
{
  int ret=SUCCESS;
  int16_t nbytes; 
  uint8_t buf[PAYLOAD_SIZE];
  char filename[12+1];
 
  Serial.print("n: ");
  Serial.println(n);
  
  // Open the file
  sprintf(filename, "PICT%d.jpg", n);
  if (!FilePicture.open(&root, filename, O_READ)) return FILE_OPEN_ERROR;  

  Serial.print("Open file: ");
  Serial.println(filename);
  
  // Prepare HTTP POST
  uint32_t jpglen = FilePicture.fileSize();
  char start_request[256];
  sprintf(start_request, "\n--AaB03x\nContent-Disposition: form-data; name=\"picture\"; filename=%s\nContent-Type: image/jpeg\nContent-Transfer-Encoding: binary\n\n", filename);
  char end_request[]    ="\n--AaB03x--\n";
  uint32_t len = jpglen + strlen(start_request) + strlen(end_request);
  
  Serial.print("len: ");
  Serial.println(len);
  
  // Connect to the server
  Serial.println("Starting connection to server...");  
  if (tcpClient.connect(external_ipServer, external_portServer)) {           
          Serial.println("connecting to server...");
          unsigned long msBlockMax= 5000;
 		  if(tcpClient.isConnected(msBlockMax, &status)) {
 		        Serial.println("is connected to server");
  
                // Send HTPP POST		        
                tcpClient.println("POST /robot.php HTTP/1.1");   
                tcpClient.print("Host: "); tcpClient.println(external_ipServer);
                tcpClient.println("Content-Type: multipart/form-data; boundary=AaB03x");
                tcpClient.print("Content-Length: "); tcpClient.println(len);
                tcpClient.print(start_request);
  
                // read from the file until there's nothing else in it:
                while ((nbytes = FilePicture.read(buf, sizeof(buf))) > 0) {
                    tcpClient.writeStream(buf, nbytes,&status);
       
                    Serial.print("status: ");
                    Serial.println(DNETcK::getError(status));       
                    delay(100);
                }// while
  
                tcpClient.print(end_request);
                tcpClient.println();
  		  }    
 		  else {
 		        Serial.print("is NOT connected to server status: ");
 		        Serial.print(status, DEC);
 		        Serial.print(", error: ");
                Serial.println(DNETcK::getError(status));
                
                tcpClient.close();
                Serial.println("Closing TcpClient"); 
                
                FilePicture.close();
 		        return -1;
          } 
         
          // Close
          tcpClient.close();
          Serial.println("Closing TcpClient"); 
  }
  else {
         Serial.print("can't connect to server status: ");
 		 Serial.print(status, DEC);
 		 Serial.print(", error: ");
         Serial.println(DNETcK::getError(status));
            
         tcpClient.close();
         Serial.println("Closing TcpClient"); 

         FilePicture.close();
 		 return -2; 
  }
  	 
  //Close file
  if (!FilePicture.close()) return FILE_CLOSE_ERROR;  
  
  return SUCCESS;
}


int WiFiCmdRobot::WiFiSendInfos ()
{
  int ret=SUCCESS;
  char buf[1024]= "";
  char szresp[10]= "";

  Serial.println("Starting connection to server...");  
  if (tcpClient.connect(external_ipServer, external_portServer)) {           
          Serial.println("connecting to server...");
 		    
 		  if(tcpClient.isConnected(&status)) {
 		        Serial.println("is connected to server");

		        for(int j=0; j<resp_len; j++)
                {
                    strcat(buf,szField[j]);
                    strcat(buf,"=");
                    sprintf(szresp, "%10d", (int)resp[j]);
                    strcat(buf,szresp);
                    strcat(buf,"&");
                }
			    Serial.println(buf);
	        
		        tcpClient.println("POST /robot.php HTTP/1.1");   
		        tcpClient.print("Host: "); tcpClient.println(external_ipServer);
 		        tcpClient.println("User-Agent: ChipkitEDH/0.1");
	            tcpClient.println("Connection: close");
		        tcpClient.println("Content-Type: application/x-www-form-urlencoded");
   		        tcpClient.print("Content-length: "); tcpClient.println(strlen(buf));
                tcpClient.println();
    
                tcpClient.println(buf);
                tcpClient.println();    		        
 		  }    
 		  else {
 		        Serial.print("is NOT connected to server status: ");
 		        Serial.print(status, DEC);
 		        Serial.print(", error: ");
                Serial.println(DNETcK::getError(status));
                
                tcpClient.close();
                Serial.println("Closing TcpClient"); 

 		        return -1;
          } 
         
          // Close
          tcpClient.close();
          Serial.println("Closing TcpClient"); 
  }
  else {
         Serial.print("can't connect to server status: ");
 		 Serial.print(status, DEC);
 		 Serial.print(", error: ");
         Serial.println(DNETcK::getError(status));
            
         tcpClient.close();
         Serial.println("Closing TcpClient"); 


 		 return -2;
  } 
 
  return SUCCESS;              
} 

int WiFiCmdRobot::Check_Alert ()
{
  int motion_status = 0;
  int i = 0;
  
  // Check motion
  motion_status = Motion.Motion_status();  
  Serial.print("Motion: "); Serial.println(motion_status);
  if(motion_status == 1) return ALERT_MOTION;
 
  // Check Temperature Variation
  if (temperature != previous_temperature) {
      previous_temperature = temperature;  
      if (VAR_TEMPERATURE > abs(avg_temperature -temperature) && tab_temperature[NB_TEMPERATURE-1] != 0) {
          return ALERT_TEMPERATURE;
      }    
  
      avg_temperature = 0;
      for (i=NB_TEMPERATURE;i<2;i--) { 
          tab_temperature[i-1] = tab_temperature[i-2];
          avg_temperature += tab_temperature[i-1];
      }
      tab_temperature[0]=temperature;
      avg_temperature = (avg_temperature+temperature)/NB_TEMPERATURE;
  }
         
  // Check Lux Variation
  if (lux != previous_lux) {
      previous_lux = lux;  
      if (VAR_LUX > abs(avg_lux -lux) && tab_lux[NB_LUX-1] != 0) {
          return ALERT_LUX;
      }    
  
      avg_lux = 0;
      for (i=NB_LUX;i<2;i--) { 
          tab_lux[i-1] = tab_lux[i-2];
          avg_lux += tab_lux[i-1];
      }
      tab_lux[0]=lux;
      avg_lux = (avg_lux+lux)/NB_LUX;
  }
      
  return NO_ALERT;              
}        

   