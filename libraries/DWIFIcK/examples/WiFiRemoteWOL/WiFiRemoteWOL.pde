/************************************************************************/
/*                                                                      */
/*    WiFiRemoteWOL                                                     */
/*                                                                      */
/*    A chipKIT WiFi Server implementation                              */
/*    to broadcast a Wake-On-LAN MAC datagram to wake sleeping          */
/*    computers.                                                        */
/*    This sketch is designed to work with the RemoteWOL                */
/*    Windows application supplied in the examples directory            */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2011, Digilent Inc.                                     */
/************************************************************************/
/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/************************************************************************/
/*                                                                      */
/*    This library is explicity targeting the chipKIT compatible        */
/*    product line                                                      */
/*                                                                      */
/*  RemoteWOL Description:                                              */
/*                                                                      */
/*    An issue with Wake-On-LAN is that most routers and gateways       */
/*    block broadcast messages and it is difficult or impossible        */
/*    to remotely wake up a sleeping computer outside of your subnet    */
/*    as the WOL broadcast message will not make it though the router.  */
/*                                                                      */
/*    If this application is running on the subnet where                */
/*    other computers are sleeping and you would like to remotely       */
/*    wake them up, for example, so you can remote desktop to them.     */
/*    This application can listen on a port for a TCPIP request to send */
/*    a WOL broadcast message and wake the sleeping computer.           */
/*                                                                      */
/*    RemoteWOL will wait and listen on the specified port              */
/*    and when a MAC address is sent to it, it will re-broadcast        */
/*    that MAC as a WOL Magic Packet on the local subnet to wake up     */
/*    the sleeping computer. The network card on the sleeping computer  */
/*    must be configured for Magic Packet Wake-up for this to work;     */
/*    see your computer documentation or search online for Wake-On-LAN  */
/*    To get to your local network, your router will probably need      */
/*    to port forward the servers port to the machine you are running   */
/*    the RemoteWol Server                                              */
/*                                                                      */
/*    This sketch is designed to work with the RemoteWOL                */
/*    Windows application supplied in the examples directory            */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    8/30/2011(KeithV): Created                                        */
/*    12/15/2011(KeithV): Added History and LED status                  */
/*    2/1/2012(KeithV): Added WiFi support                              */
/*    11/13/2012(KeithV): Modified to be generic for all HW libraries   */
/*                                                                      */
/************************************************************************/

//******************************************************************************************
//******************************************************************************************
//***************************** SET YOUR CONFIGURATION *************************************
//******************************************************************************************
//******************************************************************************************

/************************************************************************/
/*                                                                      */
/*              Include ONLY 1 hardware library that matches            */
/*              the network hardware you are using                      */
/*                                                                      */
/*              Refer to the hardware library header file               */
/*              for supported boards and hardware configurations        */
/*                                                                      */
/************************************************************************/
// #include <WiFiShieldOrPmodWiFi.h>                       // This is for the MRF24WBxx on a pmodWiFi or WiFiShield
#include <WiFiShieldOrPmodWiFi_G.h>                     // This is for the MRF24WGxx on a pmodWiFi or WiFiShield

/************************************************************************/
/*                    Required libraries, Do NOT comment out            */
/************************************************************************/
#include <DNETcK.h>
#include <DWIFIcK.h>

/************************************************************************/
/*                                                                      */
/*              SET THESE VALUES FOR YOUR NETWORK                       */
/*                                                                      */
/************************************************************************/

// Set the static IP and listening port
byte localStaticIP = 190;                                       // this will be the gateway IP with the last byte being 190
unsigned short listeningPort = DNETcK::iPersonalPorts44 + 200;  // 44000 + 200 = 44200

// Specify the SSID
const char * szSsid = "chipKIT";

// select 1 for the security you want, or none for no security
#define USE_WPA2_PASSPHRASE
//#define USE_WPA2_KEY
//#define USE_WEP40
//#define USE_WEP104
//#define USE_WF_CONFIG_H

// modify the security key to what you have.
#if defined(USE_WPA2_PASSPHRASE)

    const char * szPassPhrase = "Digilent";
    #define WiFiConnectMacro() DWIFIcK::connect(szSsid, szPassPhrase, &status)

#elif defined(USE_WPA2_KEY)

    DWIFIcK::WPA2KEY key = { 0x27, 0x2C, 0x89, 0xCC, 0xE9, 0x56, 0x31, 0x1E, 
                            0x3B, 0xAD, 0x79, 0xF7, 0x1D, 0xC4, 0xB9, 0x05, 
                            0x7A, 0x34, 0x4C, 0x3E, 0xB5, 0xFA, 0x38, 0xC2, 
                            0x0F, 0x0A, 0xB0, 0x90, 0xDC, 0x62, 0xAD, 0x58 };
    #define WiFiConnectMacro() DWIFIcK::connect(szSsid, key, &status)

#elif defined(USE_WEP40)

    const int iWEPKey = 0;
    DWIFIcK::WEP40KEY keySet = {    0xBE, 0xC9, 0x58, 0x06, 0x97,     // Key 0
                                    0x00, 0x00, 0x00, 0x00, 0x00,     // Key 1
                                    0x00, 0x00, 0x00, 0x00, 0x00,     // Key 2
                                    0x00, 0x00, 0x00, 0x00, 0x00 };   // Key 3
    #define WiFiConnectMacro() DWIFIcK::connect(szSsid, keySet, iWEPKey, &status)

#elif defined(USE_WEP104)

    const int iWEPKey = 0;
    DWIFIcK::WEP104KEY keySet = {   0x3E, 0xCD, 0x30, 0xB2, 0x55, 0x2D, 0x3C, 0x50, 0x52, 0x71, 0xE8, 0x83, 0x91,   // Key 0
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // Key 1
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // Key 2
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // Key 3
    #define WiFiConnectMacro() DWIFIcK::connect(szSsid, keySet, iWEPKey, &status)

#elif defined(USE_WF_CONFIG_H)

    #define WiFiConnectMacro() DWIFIcK::connect(0, &status)

#else   // no security - OPEN

    #define WiFiConnectMacro() DWIFIcK::connect(szSsid, &status)

#endif
   
// this should be uncommented if you want to restart 
// the WiFi connection when DNETcK is restarted.
// #define RECONNECTWIFI

//******************************************************************************************
//******************************************************************************************
//***************************** END OF CONFIGURATION ***************************************
//******************************************************************************************
//******************************************************************************************

#define TooMuchTime()     (millis() > cMSecEnd)
#define RestartTimer()    cMSecEnd = millis() + cSecTimeout * 1000
#define SetTimeout(cSec) cSecTimeout = cSec
#define cHistoryEntries 512

static int cSecTimeout = 30;
static int cMSecEnd = 0;
static int cSecRest = 60;

// depending on the board, I may or may not be able to use LED1 as it conflicts with 
// the WiFi SPI pins. This occurs on the Uno32 and MX3cK, which are both MX3 devices
// other boards LED1 is fine.
#if defined (__32MX320F128H__)
    #define PIN_LED_SAFE PIN_LED2
#else
    #define PIN_LED_SAFE PIN_LED1
#endif

// A namespace to avoid some compiler scoping issues
namespace SLED {

    typedef enum
    {
        PROCESS = 0,    // just do what you were doing
        NOTREADY,       // LED is OFF
        READY,          // LED blinks
        WORKING         // LED is ON
    } STATE;

    /***    void setLED(SLED::STATE state)
     *
     *    Parameters:
     *          state The SYSSTATE to change to
     *              
     *    Return Values:
     *          None
     *
     *    Description: 
     *    
     *      This controls the system LED.
     *      If the system is not ready, that is no IP stack is open
     *      then we are NOTREADY and the LED will be OFF.
     *      If however we are listening, we are READY and the LED will blink
     *      If however we are processing a request, we are WORKING and the LED will be ON
     *
     *      It is the parameter SLED::STATE that the comiler will not 
     *      accept unless the namespace is also supplied, not sure why
     *      parameters are out of enum typedef scope when eveything else is.
     * ------------------------------------------------------------ */
    void setLED(SLED::STATE state)
    {
        static STATE stateCur = NOTREADY;
        static unsigned long msblinkPeriod = 500;     // blinks every 1/2 second
        static unsigned long tStart = 0;
        static int  blinkState = LOW;

        // only change if we are told to do something new
        if(state != PROCESS)
        {
            stateCur = state;
        }

        switch(stateCur)
        {
            case NOTREADY:
                blinkState = LOW;
                break;

            case READY:
                if(millis() - tStart > msblinkPeriod)
                {
                    blinkState ^= HIGH;
                    tStart = millis();               
                }
                break;

            case WORKING:
                blinkState = HIGH;
                break;

            case PROCESS:
            default:
                break;
        }

        digitalWrite(PIN_LED_SAFE, blinkState);
    }
}

typedef enum 
{
    // enumerate these number to make sure we match the host
    // sending these commands
    None = 0,
    Succeeded = 1,
    Failed = 2,
    BoardcastMAC = 3,
    HistoryRequest = 4,
    HistoryReply = 5,
    Terminate = 6,

    // this must be above all of the typical CMDMSG values
    // as we will use these for our state machine in the loop
    STARTSTATE = 1000,
    STARTWIFISCAN,
    WAITFORSCAN,
    PRINTAPINFO,
    WIFICONNECT,
    WIFICONNECTWITHKEY,
    WIFICONNECTED,
    WIFIKEYGEN,
    DYNAMICIPBEGIN,
    STATICIPBEGIN,
    END,
    INITIALIZE,
    WAITFORTIME,
    GETNETADDRESSES,
    PRINTADDRESSES,
    MAKESTATICIP,
    SETENDPOINTS,
    CHECKBCENDPOINT,
    PRINTWIFICONFIG,
    STARTLISTENING,
    LISTENING,
    NOTLISTENING,
    CHECKING,
    DISPLAYTIME,
    READHDR,
    READDATA,
    REPLY,
    STOPCLIENT,
    RESTART,
    RESTFORAWHILE,
    DONOTHING,
    ERROR    
} CMDMSG;

typedef struct {
    CMDMSG  cmdmsg;
    int     cbData;
} MSGHDR;

// this will DWORD align by the compiler because the int must be referenced on a boundary
// so go ahead and define some flags for future use in 2 byte fields. 
typedef struct {
    unsigned int epochTime;
    IPv4 remoteIP;
    byte rgbMAC[6];
    byte flags1;
    byte flags2;
} BHE;

typedef struct {
    int iNext;
    int cValid;
    BHE rgBHE[cHistoryEntries];
} BHS;

BHS bhs;

// These is our message data structure and buffers
MSGHDR msghdrIn;
byte rgbMsgData[128];  // max data we can read.

MSGHDR msghdrBack;

// this is where we will keep our WIFI Key once we calculate it.
DWIFIcK::SECINFO secInfo;
int conID = DWIFIcK::INVALID_CONNECTION_ID;
 
// Start with DHCP to get our addresses
// then restart with a static IP
// this is the initial state machine starting point.
CMDMSG state = STARTWIFISCAN;           // Scan WiFi First
// CMDMSG state = WIFICONNECT;          // No WiFi Scan
CMDMSG stateNext = MAKESTATICIP;
CMDMSG stateTimeOut = None;

DNETcK::STATUS status = DNETcK::None;
unsigned int epochTime = 0;

// scan variables
int  cNetworks = 0;
int iNetwork = 0;

// The IP address will be dependent on your local network:
IPv4 ipMyStatic = {0,0,0,0};    // a place to calculate our static IP 

// I will go through a 2 step process, the first step will be to connect
// via DHCP and get my network address, then I will connect
// with a static IP and apply the network addresses as supplied by DHCP
IPv4 ipMy;
IPv4 ipGateway;
IPv4 subnetMask;
IPv4 ipDns1;
IPv4 ipDns2;

int cbRead = 0;

unsigned short cMaxSocketsToListen = 5;

// make a datagram client instance
byte rgbCache[UdpClient::cbDatagramCacheMin];
UdpClient udpBroadcast(rgbCache, sizeof(rgbCache));

// and our server instance
TcpServer tcpServer(cMaxSocketsToListen);
TcpClient tcpClient;

/***    void setup()
 *
 *    Parameters:
 *          None
 *              
 *    Return Values:
 *          None
 *
 *    Description: 
 *    
 *      Arduino setup function.
 *      
 *      Initialize the Serial Monitor, TCPIP and UDP Ethernet Stack
 *      
 *      Use DHCP to get the IP, mask, and gateway
 *      Listening on port 44200
 *      
 * ------------------------------------------------------------ */
void setup() 
{
    bhs.iNext = 0;
    bhs.cValid = 0;

    // Set the LED off, for not initialized
    pinMode(PIN_LED_SAFE, OUTPUT);
    SLED::setLED(SLED::NOTREADY);     

    // because I want to control my timing, 
    // I am going to set my default wait time to immediate
    // but this is going to require me to check all the "Is"
    // methods before advancing to the next step
    DNETcK::setDefaultBlockTime(DNETcK::msImmediate);  
  
    Serial.begin(9600);  
    Serial.println("WiFiRemoteWOL 3.0");
    Serial.println("Digilent, Copyright 2012");
    Serial.println("");
}

/***    void loop()
 *
 *    Parameters:
 *          None
 *              
 *    Return Values:
 *          None
 *
 *    Description: 
 *    
 *      Arduino loop function.
 *      
 *      This illistrates how to write a state machine like loop
 *      so that the PeriodicTask is called everytime through the loop
 *      so the stack stay alive and responsive.
 *
 *      In the loop we listen for a request, verify it to a limited degree
 *      and then broadcast the Magic Packet to wake the request machine.
 *      
 * ------------------------------------------------------------ */
void loop()
{
   int cbAvailable;

  // see if we exceeded our timeout value.
  // then just be done and close the socket 
  // by default, a closed client is never connected
  // so it is safe to call isConnected() even if it is closed 
  if(stateTimeOut != None && TooMuchTime())
  {
    Serial.println("Timeout occured");
    state = stateTimeOut;
    stateTimeOut = None;
  }

  switch(state)
  {    

        case STARTWIFISCAN:
            Serial.println("Start WiFi Scan");
            DWIFIcK::beginScan();
            state = WAITFORSCAN;
            break;

        case WAITFORSCAN:
            if(DWIFIcK::isScanDone(&cNetworks, &status))
            {
                Serial.println("Scan Done");
                state = PRINTAPINFO;
            }
            else if(DNETcK::isStatusAnError(status))
            {
                Serial.println("Scan Failed");
                Serial.println("");
                state = WIFICONNECT;
            }
            break;

        case PRINTAPINFO:
            if(iNetwork < cNetworks)
            {
                DWIFIcK::SCANINFO scanInfo;
                int j = 0;

                if(DWIFIcK::getScanInfo(iNetwork, &scanInfo))
                {
                    Serial.println("");
                    Serial.print("Scan info for index: ");
                    Serial.println(iNetwork, DEC);

                    Serial.print("SSID: ");
                    Serial.println(scanInfo.szSsid);

                    Serial.print("Secuity type: ");
                    Serial.println(scanInfo.securityType, DEC);

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
                    Serial.println("");

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
                state = WIFICONNECT;
                Serial.println("");
            }
            break;

        case WIFICONNECT:
            Serial.println("About To Connect");
            
            if((conID = WiFiConnectMacro()) != DWIFIcK::INVALID_CONNECTION_ID)
            {
                Serial.print("Connection Created, ConID = ");
                Serial.println(conID, DEC);
                state = WIFICONNECTED;
            }
            else
            {
                Serial.println("Unable to get a connection with passphrase");
                state = ERROR;
            }
            break;

        case WIFICONNECTWITHKEY:
                    
            if((conID = DWIFIcK::connect(secInfo.securityType, szSsid, secInfo.key.rgbKey, secInfo.key.cbKey, secInfo.key.index, &status)) != DWIFIcK::INVALID_CONNECTION_ID)
            {
                Serial.print("Key Connection Created, ConID = ");
                Serial.println(conID, DEC);
                state = WIFICONNECTED;
            }
            else
            {
                Serial.println("Unable to get a connection with key");
                state = ERROR;
            }
            break;

        case WIFICONNECTED:
            if(DWIFIcK::isConnected(conID, &status))
            {
                Serial.println("Is Connected");
                Serial.print("connection status: ");
                Serial.println(status, DEC);

                if(stateNext == MAKESTATICIP)
                {
                     state = DYNAMICIPBEGIN;
                }
                else
                {
                    state = STATICIPBEGIN;
                }
            }
            else if(DNETcK::isStatusAnError(status))
            {
                Serial.println("WiFi not connected");
                state = RESTART;
            }
            break;

        case WIFIKEYGEN:
            Serial.println("Getting Key");
            DWIFIcK::getSecurityInfo(conID, &secInfo);
            state = DYNAMICIPBEGIN;
            Serial.print("Key Type: ");
            Serial.println(secInfo.securityType, DEC);
            Serial.print("Key Lenght: ");
            Serial.println(secInfo.key.cbKey, DEC);
            Serial.print("Key value: ");
            printNumb(secInfo.key.rgbKey, secInfo.key.cbKey, ':');
            Serial.println("");
            state = END;
            break;
 
    case DYNAMICIPBEGIN:

        Serial.println("Dynamic begin");

        // ultimately I want to to have a static IP address 
        // but first I want to get my network addresses from DHCP
        // so to start, lets use DHCP
        DNETcK::begin();
        state = INITIALIZE;
        break;

    case STATICIPBEGIN: 
        
        Serial.println("Static begin");

        // start again with static IP addresses
        DNETcK::begin(ipMyStatic, ipGateway, subnetMask, ipDns1, ipDns2);    
        state = INITIALIZE;
        break;

    case INITIALIZE:

        // wait for initialization of the internet engine to complete
        if(DNETcK::isInitialized(&status))
            {
                Serial.println("Network Initialized");
                state = GETNETADDRESSES;
                SLED::setLED(SLED::WORKING);
            }
        else if(DNETcK::isStatusAnError(status))
            {
                Serial.println("Not Initialized");
                state = ERROR;
            }
        break;

    case GETNETADDRESSES:
        
        // at this point we know we are initialized and
        // I can get my network address as assigned by DHCP
        // I want to save them so I can restart with them
        // there is no reason for this to fail

        // This is also called during the static IP begin
        // just to get in sync with what the underlying system thinks we have.
        DNETcK::getMyIP(&ipMy);
        DNETcK::getGateway(&ipGateway);
        DNETcK::getSubnetMask(&subnetMask);
        DNETcK::getDns1(&ipDns1);
        DNETcK::getDns2(&ipDns2);

        state = stateNext;  // depends if we are on pass 1 or 2
        break;
    
    case MAKESTATICIP:

        // build the requested IP for this subnet
        ipMyStatic = ipGateway;
        ipMyStatic.rgbIP[3] = localStaticIP;

        // make sure what we built is in fact on our subnet
        if( (ipMyStatic.u32IP & subnetMask.u32IP ) == (ipGateway.u32IP & subnetMask.u32IP ))
        {
            // if so, restart the IP stack and
            // use our static IP  
            state = WIFIKEYGEN;
        }

        // if not just use our dynamaically assigned IP
        else
        {
            // otherwise just continue with our DHCP assiged IP
            stateTimeOut = PRINTADDRESSES;
            state = WAITFORTIME;
            RestartTimer();
        }
        break;

    case END:

        // this is probably not neccessary but good
        // practice to make sure our instances are closed
        // before shutting down the Internet stack
        tcpClient.close();
        tcpServer.close();
        udpBroadcast.close();
        
        // terminate our internet engine
        DNETcK::end();

        // if we were asked to shut down the WiFi channel as well, then disconnect
        // we should be careful to do this after DNETcK:end().
#ifdef RECONNECTWIFI
        // disconnect the WiFi, this will free the connection ID as well.
        DWIFIcK::disconnect(conID);
        state = WIFICONNECTWITHKEY;
#else
        state = STATICIPBEGIN;
#endif 

        SLED::setLED(SLED::NOTREADY);

        stateNext = WAITFORTIME;
        stateTimeOut = PRINTADDRESSES;
        RestartTimer();
        break;

    case WAITFORTIME:
        epochTime = DNETcK::secondsSinceEpoch(&status); 
        if(status == DNETcK::TimeSinceEpoch)
        {
            stateTimeOut = None;
            printDayAndTime(epochTime);
            state = PRINTADDRESSES;
        }

        break;

    case PRINTADDRESSES:

        stateTimeOut = None;
        Serial.println("");

        {
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

            Serial.println("");
        }

        state = SETENDPOINTS;
        break;

    case SETENDPOINTS:

        // now set up the broadcast endpoint
        udpBroadcast.setEndPoint(UdpClient::broadcastIP, 9);
        state = CHECKBCENDPOINT;
        break;

    case CHECKBCENDPOINT:

        // wait for the braodcast endpoint to resolve, this should really just work!
        if(udpBroadcast.isEndPointResolved(&status))
            {
                state = PRINTWIFICONFIG;
            }
        else if(DNETcK::isStatusAnError(status))
            {
                Serial.println("Broadcast endpoint not resolved");
                state = ERROR;
            }
        break;

    case PRINTWIFICONFIG:
        {
            DWIFIcK::CONFIGINFO configInfo;

            if(DWIFIcK::getConfigInfo(&configInfo))
            {
                Serial.println("WiFi config information");

                Serial.print("Scan Type: ");
                Serial.println(configInfo.scanType, DEC);

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

                Serial.println("");

            }
            else
            {
                Serial.println("Unable to get WiFi config data");
            }
        }

        state = STARTLISTENING;
        break;

    case STARTLISTENING:
        
        // we know we are initialized, and our broadcast UdpClient is ready
        // we should just be able to start listening on our TcpIP port
        tcpServer.startListening(listeningPort);
        state = LISTENING;
        break;

    case LISTENING:
        if(tcpServer.isListening(&status))
        {          
            Serial.print("Listening on Port: ");
            Serial.println(listeningPort, DEC);
            Serial.println("");
            state = CHECKING;
            SLED::setLED(SLED::READY);
        }
        else if(DNETcK::isStatusAnError(status))
        {
            state = NOTLISTENING;
        }
        break;
 
    case CHECKING:   
        if(tcpServer.availableClients() > 0)
        {       
            if(tcpServer.acceptClient(&tcpClient))
            {
                SLED::setLED(SLED::WORKING);
                Serial.println("Got a client");

                // set the timer so if something bad happens with the client
                // we won't hang, also we don't need to check errors on the client
                // becasue we will just timeout if there is an error
                state = DISPLAYTIME;
                stateTimeOut = STOPCLIENT;
                RestartTimer();   
            }
            else
            {
                Serial.println("Failed to get client");
                state = STOPCLIENT;
            }
        }
        else if(!tcpServer.isListening(&status))
        {
            state = NOTLISTENING;
        }
        break;
      
    case NOTLISTENING:

        SLED::setLED(SLED::NOTREADY);
        Serial.println("Not Listening");
        switch(status)
        {
        case DNETcK::WaitingConnect:
            Serial.println("Waiting connection");
            state = LISTENING;
            break;
        case DNETcK::WaitingReConnect:
            Serial.println("Waiting reconnection");
            state = LISTENING;
            break;
        case DNETcK::NotConnected:
            Serial.println("No longer connected");
            state = RESTART;
            break;
        default:
            Serial.print("Other not-listening status: ");
            Serial.println(status, DEC);
            state = RESTART;
            break;
        }
        Serial.println("");
        break;

    case DISPLAYTIME:
        epochTime = DNETcK::secondsSinceEpoch();  
        printDayAndTime(epochTime);
        RestartTimer();
        state = READHDR;   
        break;
         
    case READHDR:
        
        // just loop until we can read the header in one shot
        if((tcpClient.available() >= sizeof(MSGHDR)) && (tcpClient.readStream((byte *) &msghdrIn, sizeof(MSGHDR)) == sizeof(MSGHDR)))
        {
                // made headway, don't time out
                RestartTimer();

                // check to see if this is a message we know how to process
                switch(msghdrIn.cmdmsg)
                {
                    case BoardcastMAC:
                        cbRead = 0;
                        state = READDATA;
                        break;

                    // a good request, but one we don't handle
                    case HistoryRequest:
                    case Terminate:
                        state = msghdrIn.cmdmsg;
                        break;

                    case HistoryReply:
                        state = Failed;
                        break;
                    
                    // some BS is coming in, just close the connection.
                    default:
                        Serial.println("Invalid Request Attempt");
                        state = STOPCLIENT;
                        break;                             
                }

            // if this is too much data for us to read 
            // Just return a failure
            if(state == READDATA && msghdrIn.cbData > sizeof(rgbMsgData) )
            {
                state = Failed;
            }   
        }  
        break;
      
    case READDATA:
       
        // only read data if data is available
        if((cbAvailable = tcpClient.available()) >= 0)
        {
            int cb = msghdrIn.cbData - cbRead;
            cb = cb < cbAvailable ? cb : cbAvailable;

            cbRead += tcpClient.readStream(&rgbMsgData[cbRead], cb);
            RestartTimer();
 
            // goto the next state if we got all of the message
            if(cbRead == msghdrIn.cbData)
            {
                state = msghdrIn.cmdmsg;
            }      
        }
        break;

    case BoardcastMAC:
        {
            IPEndPoint ipEP;

            Serial.println("Broadcast Request Detected");
            saveBroadcastHistory();
            Serial.print("Request to Wake MAC: ");
            printNumb(rgbMsgData, msghdrIn.cbData, ':');
            Serial.print(" from ");
            tcpClient.getRemoteEndPoint(&ipEP);
            printIP(ipEP.ip);
            Serial.print(":");
            Serial.print(ipEP.port, DEC);
            Serial.println(".");
            broadcast();
            RestartTimer();
            state = Succeeded;   
        }
        break;

    case Failed:
        Serial.println("Request Failed");
        msghdrBack.cmdmsg = Failed;
        msghdrBack.cbData = 0;
        RestartTimer();
        state = REPLY;
        break;

    case Succeeded:
        Serial.println("Request Succeeded");
        msghdrBack.cmdmsg = Succeeded;
        msghdrBack.cbData = 0;
        RestartTimer();
        state = REPLY;
        break;

    case REPLY:      
        tcpClient.writeStream((byte *) &msghdrBack, sizeof(msghdrBack));       
        RestartTimer();
        state = STOPCLIENT;             
        break;
    
    case HistoryRequest:
        Serial.println("History Request Detected");
        sendBroadcastHistory();
        RestartTimer();
        state = STOPCLIENT;             
        break;

    case Terminate:
        Serial.println("Terminate Request Detected");
        tcpClient.close();
        state = DONOTHING;
        SLED::setLED(SLED::NOTREADY);
        break;
      
    case STOPCLIENT:   
        tcpClient.close();
        stateTimeOut = None;
        state = LISTENING;
        Serial.println(" ");
        break;

    case RESTART:
 
        SLED::setLED(SLED::NOTREADY);

        Serial.println("Disconnecting WiFi");

        // this is probably not neccessary but good
        // practice to make sure our instances are closed
        // before shutting down the Internet stack
        tcpClient.close();
        tcpServer.close();
        udpBroadcast.close();
        
        // terminate our internet engine
        DNETcK::end();

        // disconnect the WiFi, this will free the connection ID as well.
        DWIFIcK::disconnect(conID);
 
        // make sure we don't hit our timeout code
        stateTimeOut != None;

        state = RESTFORAWHILE;
        break;

    case RESTFORAWHILE:
        {
            static bool fFirstEntry = true;
            static unsigned int tRestingStart = 0;

            if(fFirstEntry)
            {
                fFirstEntry = false;
                Serial.print("Resting for ");
                Serial.print(cSecRest, DEC);
                Serial.println(" seconds");
                tRestingStart = millis();
            }

            // see if we are done resting
            if((millis() - tRestingStart) >= (cSecRest * 1000))
            {
                fFirstEntry = true;
                state = WIFICONNECT;
                Serial.println("Done resting");
            }
        }
        break;

    case ERROR:
        Serial.print("Hard Error status #");
        Serial.print(status, DEC);
        Serial.println(" occurred.");
        tcpClient.close();
        stateTimeOut = None;
        state = DONOTHING;
        SLED::setLED(SLED::NOTREADY);
        break;
               
    case DONOTHING:
    default:
        break;
    }
  
    // Keep the Ethernet stack alive
    DNETcK::periodicTasks();
    SLED::setLED(SLED::PROCESS);
}

/***    void broadcast(void)
 *
 *    Parameters:
 *          None
 *              
 *    Return Values:
 *          None
 *
 *    Description: 
 *    
 *      This builds the Magic Packet with the requested MAC
 *      address and then broadcasts the Magic Packet over 
 *      the local subnet.
 * ------------------------------------------------------------ */
void broadcast(void)
{
  byte rgbDataGram[102];
  int i, j, k;
  
  // first there must be 6 bytes of 0xFF;
  for (i = 0; i < 6; i++) rgbDataGram[i] = 0xFF;

  // then 16 MAC 
  for (int j = 0; j < 16; j++)
  {
      for (int k = 0; k < 6; k++, i++) rgbDataGram[i] = rgbMsgData[k];
  }
  
  // now Broadcast the MAC
  i = udpBroadcast.writeDatagram(rgbDataGram, sizeof(rgbDataGram)); 
  
  if(i == sizeof(rgbDataGram))
  {
    Serial.println("WOL Broadcast Succeeded");
  }
  else
  {
    Serial.println("WOL Broadcast Failed");
  }
}

/***    void printDayAndTime()
 *
 *    Parameters:
 *          None
 *              
 *    Return Values:
 *          None
 *
 *    Description: 
 *    
 *      This illistrates how to use the Ethernet.secondsSinceEpoch()
 *      method to get the current time and display it.
 *
 *      In order for this call to work you must have a valid 
 *      DNS server so the time servers can be located 
 * ------------------------------------------------------------ */
void printDayAndTime(unsigned int epochTimeT)
{
    // Epoch is 1/1/1970; I guess that is when computers became real?
    // There are 365 days/year, every 4 years is leap year, every 100 years skip leap year. Every 400 years, do not skip the leap year. 2000 did not skip the leap year
    static const unsigned int secPerMin = 60;
    static const unsigned int secPerHour = 60 * secPerMin;
    static const unsigned int secPerDay  = 24 * secPerHour;
    static const unsigned int secPerYear = 365 * secPerDay;
    static const unsigned int secPerLeapYearGroup = 4 * secPerYear + secPerDay;
    static const unsigned int secPerCentury = 25 * secPerLeapYearGroup - secPerDay;
    static const unsigned int secPer400Years = 4 * secPerCentury + secPerDay;;
    static const int daysPerMonth[] = {31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29}; // Feb has 29, we must allow for leap year.
    static const char * szMonths[] = {"Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "Jan", "Feb"}; 
    
    // go ahead and adjust to a leap year, and to a century boundary
    // at Mar 2000 we have 30 years (From 1970) + 8 leap days (72,76,80,84,88,92,96, and Feb 2000 do not skip leap year) and Jan (31) + Feb(28)
    unsigned int secSinceMar2000 = epochTimeT - 30 * secPerYear - (8 + 31 + 28) * secPerDay;
 
    unsigned int nbr400YearGroupsFromMar2000 = secSinceMar2000 / secPer400Years;
    unsigned int secInThis400YearGroups = secSinceMar2000 % secPer400Years;
    
    // now we are aligned so the weirdness for the not skiping of a leap year is the very last day of the 400 year group.
    // because of this extra day in the 400 year group, it is possible to have 4 centries and a day.
    unsigned int nbrCenturiesInThis400YearGroup = secInThis400YearGroups / secPerCentury;
    unsigned int secInThisCentury = secInThis400YearGroups % secPerCentury;

    // if we come up with 4 centries, then we must be on leap day that we don't skip at the end of the 400 year group
    // so add the century back on as this  Century is the extra day in this century.
    if(nbrCenturiesInThis400YearGroup == 4)
    {
        nbrCenturiesInThis400YearGroup = 3;   // This can be a max of 3 years
        secInThisCentury += secPerCentury;    // go ahead and add the century back on to our time in this century
    }

    // This is going to work out just fine
    // either this is a normal century and the last leap year group is going to be a day short,
    // or this is at the end of the 400 year group and the last 4 year leap year group will work out to have 29 days as in a normal
    // 4 year leap year group.  
    unsigned int nbrLeapYearGroupsInThisCentury = secInThisCentury / secPerLeapYearGroup;
    unsigned int secInThisLeapYearGroup = secInThisCentury % secPerLeapYearGroup;
 
    // if this is at the end of the leap year group, there could be an extra day
    // which could cause us to come up with 4 years in this leap year group.
    unsigned int nbrYearsInThisLeapYearGroup = secInThisLeapYearGroup / secPerYear;
    unsigned int secInThisYear = secInThisLeapYearGroup % secPerYear;

    // are we on a leap day?
    if(nbrYearsInThisLeapYearGroup == 4)
    {
        nbrYearsInThisLeapYearGroup = 3;    // that is the max it can be.
        secInThisYear += secPerYear;        // add back the year we just took off the leap year group
    }
  
    int nbrOfDaysInThisYear = (int) (secInThisYear / secPerDay); // who cares if there is an extra day for leap year
    int secInThisDay = (int) (secInThisYear % secPerDay);
 
    int nbrOfHoursInThisDay = secInThisDay / secPerHour;
    int secInThisHours = secInThisDay % secPerHour;
 
    int nbrMinInThisHour = secInThisHours / secPerMin;
    int secInThisMin = secInThisHours % secPerMin;
    
    int monthCur = 0;
    int dayCur = nbrOfDaysInThisYear;
    int yearCur = 2000 + 400 * nbr400YearGroupsFromMar2000 + 100 * nbrCenturiesInThis400YearGroup + 4 * nbrLeapYearGroupsInThisCentury + nbrYearsInThisLeapYearGroup;
  
    // this will walk us past the current month as the dayCur can go negative.
    // we made the leap day the very last day in array, so if this is leap year, we will be able to
    // handle the 29th day.
    for(monthCur = 0, dayCur = nbrOfDaysInThisYear; dayCur >= 0; monthCur++)
    {
      dayCur -= daysPerMonth[monthCur];
    }
     
    // since we know we went past, we can back up a month
    monthCur--;
    dayCur += daysPerMonth[monthCur]; // put the last months days back to go positive on days
     
    // We did zero based days in a month, but we read 1 based days in a month.
    dayCur++;

    // we have one remaining issue
    // if this is Jan or Feb, we are really into the next year. Remember we started our year in Mar, not Jan
    // so if this is Jan or Feb, then add a year to the year
    if(monthCur >= 10)
    {
        yearCur++;
    }
     
    Serial.print("Current Day and UTC time: ");
    Serial.print(szMonths[monthCur]);
    Serial.print(" ");
    Serial.print(dayCur, DEC);
    Serial.print(", ");
    Serial.print(yearCur, DEC);
    Serial.print("  ");
    Serial.print(nbrOfHoursInThisDay, DEC);
    Serial.print(":");
    if(nbrMinInThisHour < 10)
    {
        Serial.print("0");
    }
    Serial.print(nbrMinInThisHour, DEC);
    Serial.print(":");
    if(secInThisMin < 10)
    {
        Serial.print("0");
    }
    Serial.println(secInThisMin, DEC);  
 }

/***    void saveBroadcastHistory()
 *
 *    Parameters:
 *          None
 *              
 *    Return Values:
 *          None
 *
 *    Description: 
 *    
 *      This will save the info about this broadcast into our memory
 *      to be returned when a history request is made on our server
 *
 *      It will only remain in memory for as long as the server stay running
 *      and will be lost if the server is restarted.
 * ------------------------------------------------------------ */
void saveBroadcastHistory(void)
{
    IPEndPoint remoteEP;

    // save this entry
    bhs.rgBHE[bhs.iNext].epochTime = epochTime;
    memcpy(bhs.rgBHE[bhs.iNext].rgbMAC, rgbMsgData, sizeof(bhs.rgBHE[bhs.iNext].rgbMAC));
    tcpClient.getRemoteEndPoint(&remoteEP);
    bhs.rgBHE[bhs.iNext].remoteIP = remoteEP.ip;
    bhs.rgBHE[bhs.iNext].flags1 = 0;
    bhs.rgBHE[bhs.iNext].flags2 = 0;

    // go to the next entry
    bhs.iNext++;

    // if at the end of the buffer, wrap to the start
    if(bhs.iNext >= cHistoryEntries)
    {
        bhs.iNext = 0;
    }

    // we added an entry
    bhs.cValid++;

    // if we have filled the buffer, than we are full.
    if(bhs.cValid > cHistoryEntries)
    {
        bhs.cValid = cHistoryEntries;
    }
}

/***    void sendBroadcastHistory()
 *
 *    Parameters:
 *          None
 *              
 *    Return Values:
 *          None
 *
 *    Description: 
 *    
 *      This responds to a history request from the client
 *      It returns the history of when and what MACs a
 *      magic packet was sent.
 *      
 * ------------------------------------------------------------ */
void sendBroadcastHistory(void)
{
    int iStart = bhs.iNext - bhs.cValid;
    int cSend1 = bhs.cValid;
    int cSend2 = 0;

    // if we have wrapped
    if(iStart < 0)
    {
        iStart += cHistoryEntries;
        cSend2 = bhs.iNext;
        cSend1 = cHistoryEntries - cSend2;
    }

    // Set the message header up
    msghdrBack.cmdmsg = HistoryReply;
    msghdrBack.cbData = bhs.cValid * sizeof(BHE);
    tcpClient.writeStream((byte *) &msghdrBack, sizeof(msghdrBack)); 

    if(cSend1 > 0)
    {
        // Wait for 10 seconds to get it all written
        tcpClient.writeStream((byte *) &bhs.rgBHE[iStart], cSend1 * sizeof(BHE), 10000UL); 
    }

    if(cSend2 > 0)
    {
        // wait for 10 seconds to write out all of the data
        tcpClient.writeStream((byte *) &bhs.rgBHE[0], cSend2 * sizeof(BHE), 10000UL); 
    }
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
void printIP(IPv4& ip)
{
    Serial.print("IP: ");
    printNumb(ip.rgbIP, 4, '.');
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
void printMAC(MAC& mac)
{
    Serial.print("MAC: ");
    printNumb(mac.rgbMAC, 6, ':');
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
void printNumb(byte * rgb, int cb, char chDelim)
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

