#include <WiFiCmdRobot.h>
#include <robot.h>


// Specify the IP+Port
extern IPv4 ipServer = {192,168,0,15};
extern unsigned short portServer = 44300;

// Specify the SSID
extern const char * szSsid = "WIFICOTEAU";

extern const int iWEPKey = 0;
//02 96 61 D7 B1 D4 81 50 D7 76 97 60 C8
DWIFIcK::WEP104KEY keySet = {   0x02, 0x96, 0x61, 0xD7, 0xB1, 0xD4, 0x81, 0x50, 0xD7, 0x76, 0x97, 0x60, 0xC8,   // Key 0
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // Key 1
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // Key 2
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // Key 3




//Sd2Card card;       // SD Card       
//SdVolume volume;    // SD Volume
extern SdFile root;        // SD Root
SdFile FilePicture; // SD File

TcpServer tcpServer;
TcpClient tcpClient;

byte rgbRead[1024];
int cbRead = 0;
int count = 0;
    
uint16_t cmd[CMD_SIZE];
uint16_t resp[RESP_SIZE];
int resp_len = 0;
   
DNETcK::STATUS status;

/***       void setup()
 *
 *       Parameters:
 *          None
 *              
 *       Return Values:
 *          None
 *
 *       Description: 
 *       
 *      Arduino setup function.
 *      
 *      Initialize the Serial Monitor, and initializes the
 *      the IP stack for a static IP of ipServer
 *      No other network addresses are supplied so 
 *      DNS will fail as any name lookup and time servers
 *      will all fail. But since we are only listening, who cares.
 *      
 * ------------------------------------------------------------ */
void WiFiCmdRobot::WiFiCmdRobot_begin() {
  
    int conID = DWIFIcK::INVALID_CONNECTION_ID;
    int cNetworks = 0;
    int iNetwork = 0; 
    int ret=SUCCESS;
    
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
    }
    
    // get infos from SD-Card  
    ret=infoSDCard();
    if (ret != SUCCESS)
    {  
        Serial.print("Error Infos SD-Card, error: ");
        Serial.println(ret);
    }
    
    // set my default wait time to nothing
    DNETcK::setDefaultBlockTime(DNETcK::msImmediate); 

    // start a scan
    DWIFIcK::beginScan();
    while (1)
    {
            if(DWIFIcK::isScanDone(&cNetworks, &status))
            {
                Serial.println("Scan Done");
                break;
            }
            else if(DNETcK::isStatusAnError(status))
            {
                Serial.print("Scan Failed");
                return -1;
            }
            
    }        
    
    while (1)
    {            
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

                    Serial.print("Beacon Period: ");
                    Serial.println(scanInfo.beconPeriod, DEC);    

                    Serial.print("dtimPeriod: ");
                    Serial.println(scanInfo.dtimPeriod, DEC);    

                    Serial.print("atimWindow: ");
                    Serial.println(scanInfo.atimWindow, DEC); 
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

    if((conID = WiFiConnectMacro()) != DWIFIcK::INVALID_CONNECTION_ID)
    {
          Serial.print("Connection Created, ConID = ");
          Serial.println(conID, DEC);
    }
    else
    {
          Serial.print("Unable to connection, status: ");
          Serial.println(status, DEC);
          return -2;
    }
 
    while (1)
    { 
          // initialize the stack with a static IP
          DNETcK::begin(ipServer);
          if(DNETcK::isInitialized(&status))
          {
                Serial.println("IP Stack Initialized");
                break;
          }
          else if(DNETcK::isStatusAnError(status))
          {
                Serial.print("Error in initializing, status: ");
                Serial.println(status, DEC);
                return -3;
          }
    }
        
    Serial.println("");
    IPv4 ip;

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

       
    tcpServer.startListening(portServer);

    Serial.println("End WIFI Init");                        
}

void WiFiCmdRobot::WiFiCmdRobot_main() {
    String stringRead;
    int ret = 0;
 
typedef enum
{
    NONE = 0,
    ISLISTENING,
    AVAILABLECLIENT,
    ACCEPTCLIENT,
    READ,
    CLOSE,
    EXIT,
    DONE
} STATE;
STATE state = ISLISTENING; 
    
    case ISLISTENING: 
    // not specifically needed, we could go right to AVAILABLECLIENT
    // but this is a nice way to print to the serial monitor that we are 
    // actively listening.
    // Remember, this can have non-fatal falures, so check the status
            if(tcpServer.isListening(&status))
            {
                Serial.print("Listening on port: ");
                Serial.print(portServer, DEC);
                Serial.println("");
                state = AVAILABLECLIENT;
            }
            else if(DNETcK::isStatusAnError(status))
            {
                state = EXIT;
            }
            break;

    // wait for a connection
    case AVAILABLECLIENT:
            if((count = tcpServer.availableClients()) > 0)
            {
                Serial.print("Got ");
                Serial.print(count, DEC);
                Serial.println(" clients pending");
                state = ACCEPTCLIENT;
            }
            break;

    // accept the connection
    case ACCEPTCLIENT:
            // probably unneeded, but just to make sure we have
            // tcpClient in the  "just constructed" state
            tcpClient.close(); 

            // accept the client 
            if(tcpServer.acceptClient(&tcpClient))
            {
                Serial.println("Got a Connection");
                state = READ;
            }

            // this probably won't happen unless the connection is dropped
            // if it is, just release our socket and go back to listening
            else
            {
                state = CLOSE;
            }
            break;
   
    case READ:   
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
                                    ret = ReplyKO ();
                                }
                                break;                             
 
                          }
                          else if (stringRead.startsWith("\r"))
                          {
                                // empty line => end
                                Serial.println("empty line => end");
                                state = CLOSE;
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
            }            
            state = CLOSE;
            break;
             
     case CLOSE:
            tcpClient.close();
            Serial.println("Closing TcpClient");
            Serial.println("");
            state = ISLISTENING;
            break;

    // something bad happen, just exit out of the program
    case EXIT:
            tcpClient.close();
            tcpServer.close();
            Serial.println("Something went wrong, sketch is done.");  
            state = DONE;
            break;

    // do nothing in the loop
    case DONE:
            break;
    default:
            break;
    }

    // every pass through loop(), keep the stack alive
    DNETcK::periodicTasks(); 
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
   String szcmd;
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
       }        
       if (szcmd == "STOP")
       {
               cmd[0] = CMD_STOP;
               cmd[1] = 0;
               cmd[2] = 0;
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
               cmd[0] = CMD_GO;
               cmd[1] = iparam[0];
               cmd[2] = iparam[1];
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
