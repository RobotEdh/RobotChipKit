/************************************************************************/
/*                                                                      */
/*      TCPEchoServer                                                   */
/*                                                                      */
/*      A chipKIT DNETcK TCP WiFi Server application to                 */
/*      demonstrate how to use the TcpServer Class.                     */
/*      This can be used in conjuction with TCPEchoClient               */            
/*                                                                      */
/*      This particluar sketch overrides and moves the MRF24WB0MA       */
/*      from the default JF connector on the MX7cK to the JE            */
/*      connector. However, since this connector will cause SPI         */
/*      to conflict with the Serial Monitor, the Serial Monitor         */
/*      can not be used. So instead, on the MX7cK, LED2 is used         */
/*      to indicate the state of the server. If the LED is steady       */
/*      OFF, the MX7cK is still connecting to the WiFi. If it is ON,    */
/*      an input to the server is being processed. And if it blinks     */
/*      the server is ready and listening.                              */
/*                                                                      */
/*    Works On (Only and specifically):                                 */
/*      Cerebot MX7ck with PmodWiFi on connector JE                     */
/*                                                                      */
/************************************************************************/
/*      Author:       Keith Vogel                                       */
/*      Copyright 2012, Digilent Inc.                                   */
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
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*      1/31/2012(KeithV): Created                                      */
/*      11/13/2012(KeithV): Modified to be generic for all HW libraries */
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

IPv4 ipServer = {192,168,1,190};
unsigned short portServer = DNETcK::iPersonalPorts44 + 300;     // port 44300 

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
   
//******************************************************************************************
//******************************************************************************************
//***************************** END OF CONFIGURATION ***************************************
//******************************************************************************************
//******************************************************************************************

typedef enum
{
    NONE = 0,
    INITIALIZE,
    LISTEN,
    ISLISTENING,
    AVAILABLECLIENT,
    ACCEPTCLIENT,
    READ,
    WRITE,
    CLOSE,
    EXIT,
    DONE
} STATE;

STATE state = INITIALIZE;

unsigned tStart = 0;
unsigned tWait = 5000;

TcpServer tcpServer;
TcpClient tcpClient;

byte rgbRead[1024];
int cbRead = 0;
int count = 0;

DNETcK::STATUS status;

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

    /***      void setLED(SLED::STATE state)
     *
     *      Parameters:
     *          state The SYSSTATE to change to
     *              
     *      Return Values:
     *          None
     *
     *      Description: 
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

/***      void setup()
 *
 *      Parameters:
 *          None
 *              
 *      Return Values:
 *          None
 *
 *      Description: 
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
void setup() {
    int conID = DWIFIcK::INVALID_CONNECTION_ID;

    // Set the LED off, for not initialized
    pinMode(PIN_LED_SAFE, OUTPUT);
    SLED::setLED(SLED::NOTREADY);     

    if((conID = WiFiConnectMacro()) != DWIFIcK::INVALID_CONNECTION_ID)
    {
        state = INITIALIZE;
    }
    else
    {
        state = EXIT;
    }

   // intialize the stack with a static IP
    DNETcK::begin(ipServer);
}

/***      void loop()
 *
 *      Parameters:
 *          None
 *              
 *      Return Values:
 *          None
 *
 *      Description: 
 *      
 *      Arduino loop function.
 *      
 *      We are using the default timeout values for the DNETck and TcpServer class
 *      which usually is enough time for the Tcp functions to complete on their first call.
 *
 *      This code listens for a connection, then echos any strings that come in
 *      After about 5 seconds of inactivity, it will drop the connection.
 *
 *      This is a simple sketch to illistrate a simple TCP Server. 
 *      
 * ------------------------------------------------------------ */
void loop() {

    switch(state)
    {

    case INITIALIZE:
        if(DNETcK::isInitialized(&status))
        {
            state = LISTEN;
        }
        else if(DNETcK::isStatusAnError(status))
        {
            state = EXIT;
        }
        break;

    // say to listen on the port
    case LISTEN:
        if(tcpServer.startListening(portServer))
        {
            state = ISLISTENING;
        }
        else
        {
            state = EXIT;
        }
        break;

    // not specifically needed, we could go right to AVAILABLECLIENT
    // but this is a nice way to print to the serial monitor that we are 
    // actively listening.
    // Remember, this can have non-fatal falures, so check the status
    case ISLISTENING:
        if(tcpServer.isListening(&status))
        {
            SLED::setLED(SLED::READY);
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
            SLED::setLED(SLED::WORKING);
            state = READ;
            tStart = (unsigned) millis();
        }

        // this probably won't happen unless the connection is dropped
        // if it is, just release our socket and go back to listening
        else
        {
            state = CLOSE;
        }
        break;

    // wait fot the read, but if too much time elapses (5 seconds)
    // we will just close the tcpClient and go back to listening
    case READ:

        // see if we got anything to read
        if((cbRead = tcpClient.available()) > 0)
        {
            cbRead = cbRead < sizeof(rgbRead) ? cbRead : sizeof(rgbRead);
            cbRead = tcpClient.readStream(rgbRead, cbRead);
            state = WRITE;
        }

        // If too much time elapsed between reads, close the connection
        else if( (((unsigned) millis()) - tStart) > tWait )
        {
            state = CLOSE;
        }
        break;

    // echo back the string
    case WRITE:
        if(tcpClient.isConnected())
        {               
            tcpClient.writeStream(rgbRead, cbRead);
            state = READ;
            tStart = (unsigned) millis();
        }

        // the connection was closed on us, go back to listening
        else
        {
            state = CLOSE;
        }
        break;
        
    // close our tcpClient and go back to listening
    case CLOSE:
        tcpClient.close();
        state = ISLISTENING;
        break;

    // something bad happen, just exit out of the program
    case EXIT:
        tcpClient.close();
        tcpServer.close();
         state = DONE;
        break;

    // do nothing in the loop
    case DONE:
    default:
        break;
    }

    // every pass through loop(), keep the stack alive
    DNETcK::periodicTasks(); 
    SLED::setLED(SLED::PROCESS);
}
