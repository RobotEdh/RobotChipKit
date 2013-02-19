/************************************************************************/
/*                                                                      */
/*      UDPEchoServer                                                   */
/*                                                                      */
/*      A chipKIT DNETcK UDP Server application to                      */
/*      demonstrate how to use the udpServer Class.                     */
/*      This can be used in conjuction  with UDPEchoClient              */            
/*                                                                      */
/************************************************************************/
/*      Author:       Keith Vogel                                       */
/*      Copyright 2011, Digilent Inc.                                   */
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
/*      12/19/2011(KeithV): Created                                     */
/*      2/7/2017(KeithV): Updated for WiFi                              */
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
unsigned short portServer = DNETcK::iPersonalPorts44 + 400;     // 44400 

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

// remember to give the UDP client a datagram cache
byte rgbUDPClientCache[1024];
UdpClient udpClient(rgbUDPClientCache, sizeof(rgbUDPClientCache));

// remember to give the server a datagram cache that is 
// equal or larger than the client datagram cache
const int cPending = 3; // number of clients the server will hold until accepted
byte rgbUDPServerCache[cPending * sizeof(rgbUDPClientCache)];
UdpServer udpServer(rgbUDPServerCache, sizeof(rgbUDPServerCache), cPending);

// a read buffer
byte rgbRead[1024];
int cbRead = 0;
int count = 0;

DNETcK::STATUS status;

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
    DNETcK::STATUS status;
    int conID = DWIFIcK::INVALID_CONNECTION_ID;
 
    Serial.begin(9600);
    Serial.println("WiFiUDPEchoServer 1.0");
    Serial.println("Digilent, Copyright 2012");
    Serial.println("");

    if((conID = WiFiConnectMacro()) != DWIFIcK::INVALID_CONNECTION_ID)
    {
        Serial.print("Connection Created, ConID = ");
        Serial.println(conID, DEC);
        state = INITIALIZE;
    }
    else
    {
        Serial.print("Unable to connection, status: ");
        Serial.println(status, DEC);
        state = CLOSE;
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
 *      We are using the default timeout values for the DNETck and UdpServer class
 *      which usually is enough time for the UDP functions to complete on their first call.
 *
 *      This code listens for a connection, then echos any strings that come in
 *      After about 5 seconds of inactivity, it will drop the connection.
 *
 *      This is a simple sketch to illistrate a simple UDP Server. 
 *      
 * ------------------------------------------------------------ */
void loop() {

    switch(state)
    {

    // say to listen on the port
    case INITIALIZE:
        if(DNETcK::isInitialized(&status))
        {
            Serial.println("IP Stack Initialized");
            state = LISTEN;
        }
        else if(DNETcK::isStatusAnError(status))
        {
            Serial.print("Error in initializing, status: ");
            Serial.println(status, DEC);
            state = EXIT;
        }
        break;

    case LISTEN:
        if(udpServer.startListening(portServer))
        {
            Serial.println("Started Listening");
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
        if(udpServer.isListening(&status))
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
        if((count = udpServer.availableClients()) > 0)
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
        // udpClient in the  "just constructed" state
        udpClient.close(); 

        // accept the client 
        if(udpServer.acceptClient(&udpClient))
        {
            Serial.println("Got a Connection");
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
    // we will just close the udpClient and go back to listening
    case READ:

        // see if we got anything to read
        if((cbRead = udpClient.available()) > 0)
        {
            cbRead = cbRead < sizeof(rgbRead) ? cbRead : sizeof(rgbRead);
            cbRead = udpClient.readDatagram(rgbRead, cbRead);

            Serial.print("Got ");
            Serial.print(cbRead, DEC);
            Serial.println(" bytes");
           
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

        Serial.println("Writing datagram: ");  
        for(int i=0; i < cbRead; i++) 
        {
            Serial.print(rgbRead[i], BYTE);
        }
        Serial.println("");  

        udpClient.writeDatagram(rgbRead, cbRead);
        state = READ;
        tStart = (unsigned) millis();
        break;
        
    // close our udpClient and go back to listening
    case CLOSE:
        udpClient.close();
        Serial.println("Closing UdpClient");
        Serial.println("");
        state = ISLISTENING;
        break;

    // something bad happen, just exit out of the program
    case EXIT:
        udpClient.close();
        udpServer.close();
        Serial.println("Something went wrong, sketch is done.");  
        state = DONE;
        break;

    // do nothing in the loop
    case DONE:
    default:
        break;
    }

    // every pass through loop(), keep the stack alive
    DNETcK::periodicTasks(); 
}
