/************************************************************************/
/*                                                                      */
/*      WiFiConfigOverride                                              */
/*                                                                      */
/*      A chipKIT DNETcK TCP Server application to                      */
/*      demonstrate how to use the TcpServer Class.                     */
/*      This can be used in conjuction  with TCPEchoClient              */            
/*                                                                      */
/*      While this is just like the WiFi TcpServer sketch, in this      */
/*      directory there is a WF_Config.x that changes the security      */
/*      to use a calculated key rather than the pass phrase.            */
/*      When creating the connection to the router, use                 */
/*      DWIFIcK::connect(0) with the profile/connection ID of 0         */
/*      to specify that the parameters come out of WF_Config.x file.    */
/*      This allows for easy configuation of the WiFi security          */
/*      parameters just by changing the WF_Config.x file.               */      
/*                                                                      */
/*      WF_Config.x can be modified with the TCPIP Configuration Wizard */
/*      You do not need to select or change the TCPIP configuation      */
/*      when the wizard is opened, just select the WiFi configuration   */
/*      and point to the WF_Config.h in this sketch directory.          */
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
/*      2/7/20121(KeithV): Updated for WiFi                             */
/*      2/7/20121(KeithV): Updated to use WF_Config.x for WiFi          */
/*                          security parameters                         */
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

    Serial.begin(9600);
    Serial.println("WiFiConfigOverride 1.0");
    Serial.println("Digilent, Copyright 2012");
    Serial.println("");

    if((conID = DWIFIcK::connect(0, &status)) != DWIFIcK::INVALID_CONNECTION_ID)
    {
        Serial.print("Connection Created, ConID = ");
        Serial.println(conID, DEC);
        state = INITIALIZE;
    }
    else
    {
        Serial.print("Unable to connection, status: ");
        Serial.println(status, DEC);
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

    // say to listen on the port
    case LISTEN:
        if(tcpServer.startListening(portServer))
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
        if(tcpClient.isConnected())
        {               
            Serial.println("Writing: ");  
            for(int i=0; i < cbRead; i++) 
            {
                Serial.print(rgbRead[i], BYTE);
            }
            Serial.println("");  

            tcpClient.writeStream(rgbRead, cbRead);
            state = READ;
            tStart = (unsigned) millis();
        }

        // the connection was closed on us, go back to listening
        else
        {
            Serial.println("Unable to write back.");  
            state = CLOSE;
        }
        break;
        
    // close our tcpClient and go back to listening
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
    default:
        break;
    }

    // every pass through loop(), keep the stack alive
    DNETcK::periodicTasks(); 
}
