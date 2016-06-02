#include <WiFiRobot.h>

// SD variables
extern SdFile root;          // SD Root
extern SdFile FilePicture;   // SD File


// Specify the SSID
const char * szSsid = "WIFICOTEAU2";

// WPA2 key
//You can find a tool to pre-calculate your PSK key at http://www.wireshark.org/tools/wpa-psk.html.
//kitesurf9397+ WIFICOTEAU2 => e0c1337f19334617316c4abdeed61c47e096e2bfa87cdb3db93864950c4a2295
DWIFIcK::WPA2KEY key = {0xe0 ,0xc1 ,0x33 ,0x7f ,0x19 ,0x33 ,0x46 ,0x17 ,0x31 ,0x6c ,0x4a ,0xbd ,0xee ,0xd6 ,0x1c ,0x47 ,0xe0 ,0x96 ,0xe2 ,0xbf ,0xa8 ,0x7c ,0xdb ,0x3d ,0xb9 ,0x38 ,0x64 ,0x95 ,0x0c ,0x4a ,0x22 ,0x95};

// Specify external server IP+Port
const char *         external_ipServer   = "192.168.0.18";
const unsigned short external_portServer = 80;

//My IP
IPv4                 my_ip   = {192,168,0,15};

TcpClient tcpClient;   
DNETcK::STATUS status;

  
byte rgbRead[1024];
int cbRead = 0;
int count = 0;

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
void WiFiRobotClass::printIP(IPv4& ip)
{
    Serial.print("IP: ");
    WiFiRobotClass::printNumb(ip.rgbIP, 4, '.');
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
void WiFiRobotClass::printMAC(MAC& mac)
{
    Serial.print("MAC: ");
    WiFiRobotClass::printNumb(mac.rgbMAC, 6, ':');
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
void WiFiRobotClass::printNumb(byte * rgb, int cb, char chDelim)
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

int WiFiRobotClass::WiFiRobot_scan() {
    int cNetworks = 0;
    int iNetwork = 0; 
    int ret=SUCCESS;
    
    Serial.println("Begin WiFiRobot_scan");
      
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
    
    Serial.println("End WiFiRobot_scan");
   
    return SUCCESS;   // Continue if error  
}


int WiFiRobotClass::WiFiRobot_init_TCPIP() {
    IPv4 ip;  

    int conID = DWIFIcK::INVALID_CONNECTION_ID;
    int ret=SUCCESS;
    
    Serial.println("Begin WiFiRobot_init_TCPIP");
 
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

    DNETcK::begin(my_ip);

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
    
    Serial.println("End WiFiRobot_init_TCPIP");

    return SUCCESS;
   
}

int WiFiRobotClass::WiFiRobot_begin() {
    int ret=SUCCESS;
    
    Serial.println("Begin WiFiRobot_begin");
             
    // Start a WIFI scan
    ret = WiFiRobotClass::WiFiRobot_scan();
        
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
       
    Serial.println("End WiFiRobot_begin");
 
    return SUCCESS;                        
}

int WiFiRobotClass::WiFiRobot_SendAlert(char *infos, int n) {
    int ret=SUCCESS;
   
    Serial.println("Call WiFiSendInfos");
    ret = WiFiRobotClass::WiFiSendInfos(infos);
  
    if (ret != SUCCESS) {
        Serial.print("WiFiSendInfos error: "); Serial.println(ret);
        tcpClient.close();
        return ret;
    }
  
    // Send last 3 pictures
    for(int j=n; j>=n-2; j--)
    {
          Serial.println("Call WiFiSendFastPicture");
          ret = WiFiRobotClass::WiFiSendFastPicture(j);
  
          if (ret != SUCCESS) {
              Serial.print("WiFiSendFastPicture error: "); Serial.println(ret);
              tcpClient.close();
              return ret;
          }
    }
       
    return SUCCESS;    
}

int WiFiRobotClass::WiFiSendInfos (char *infos)
{
  int ret=SUCCESS;
  
  // Init TCP/IP stack
  ret = WiFiRobotClass::WiFiRobot_init_TCPIP();
  if (ret != SUCCESS)
  {  
      Serial.print("Error Init TCP/IP stack, error: ");
      Serial.println(ret);
      return ret;
  }
 
  Serial.println("Starting connection to server...");  
  if (tcpClient.connect(external_ipServer, external_portServer)) {           
          Serial.println("connecting to server...");
 		    
 		  if(tcpClient.isConnected(&status)) {
 		        Serial.println("is connected to server");

			    Serial.println(infos);
	        
		        tcpClient.println("POST /robot.php HTTP/1.1");   
		        tcpClient.print("Host: "); tcpClient.println(external_ipServer);
 		        tcpClient.println("User-Agent: ChipkitEDH/0.1");
	            tcpClient.println("Connection: close");
		        tcpClient.println("Content-Type: application/x-www-form-urlencoded");
   		        tcpClient.print("Content-length: "); tcpClient.println(strlen(infos));
                tcpClient.println();
    
                tcpClient.println(infos);
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


int WiFiRobotClass::WiFiSendFastPicture (int16_t n)
{
  int ret=SUCCESS;
  int16_t nbytes; 
  uint8_t buf[PAYLOAD_SIZE];
  char filename[12+1];
 
  Serial.print("n: "); Serial.println(n);
  
  // Open the file
  sprintf(filename, "PICT%d.jpg", n);
  Serial.print("Open file: "); Serial.println(filename);
  if (!FilePicture.open(&root, filename, O_READ)) return FILE_OPEN_ERROR;  

  // Prepare HTTP POST
  uint32_t jpglen = FilePicture.fileSize();
  char start_request[256];
  sprintf(start_request, "\n--AaB03x\nContent-Disposition: form-data; name=\"picture\"; filename=%s\nContent-Type: image/jpeg\nContent-Transfer-Encoding: binary\n\n", filename);
  char end_request[]    ="\n--AaB03x--\n";
  uint32_t len = jpglen + strlen(start_request) + strlen(end_request);
  
  Serial.print("len: "); Serial.println(len);
    
  // Init TCP/IP stack
  ret = WiFiRobotClass::WiFiRobot_init_TCPIP();
  if (ret != SUCCESS)
  {  
      Serial.print("Error Init TCP/IP stack, error: ");
      Serial.println(ret);
      return ret;
  }
 
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
   