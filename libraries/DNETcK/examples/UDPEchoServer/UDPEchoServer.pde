/************************************************************************/
/*                                                                      */
/*    UDPEchoServer                                                     */
/*                                                                      */
/*    A chipKIT DNETcK UDP Server application to                        */
/*    demonstrate how to use the udpServer Class.                       */
/*    This can be used in conjuction  with UDPEchoClient                */        
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
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    12/19/2011(KeithV): Created                                       */
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
/*              If your base chipKIT board has a network adaptor        */
/*              manufactured on the board, such as the MX7cK,           */
/*              then you do not need to specify any hardware library    */
/*              for that network hardware on the board.                 */
/*              However, if you are using alternate network hardware,   */
/*              then you will need to specify the hardware library      */
/*              for the hardware you are using.                         */
/*                                                                      */
/*              Refer to the hardware library header file               */
/*              for supported boards and hardware configurations        */
/*                                                                      */
/************************************************************************/
#include <NetworkShield.h>
// #include <PmodNIC.h>
// #include <PmodNIC100.h>

/************************************************************************/
/*                    Required library, Do NOT comment out              */
/************************************************************************/
#include <DNETcK.h>

/************************************************************************/
/*                                                                      */
/*              SET THESE VALUES FOR YOUR NETWORK                       */
/*                                                                      */
/************************************************************************/

IPv4 ipServer = {192,168,1,190};
unsigned short portServer = 44400; 
   
//******************************************************************************************
//******************************************************************************************
//***************************** END OF CONFIGURATION ***************************************
//******************************************************************************************
//******************************************************************************************

typedef enum
{
    NONE = 0,
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

STATE state = LISTEN;

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

DNETcK::STATUS status;

// a read buffer
byte rgbRead[1024];
int cbRead = 0;
int count = 0;

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
 *      Initialize the Serial Monitor, and initializes the
 *      the IP stack for a static IP of ipServer
 *      No other network addresses are supplied so 
 *      DNS will fail as any name lookup and time servers
 *      will all fail. But since we are only listening, who cares.
 *      
 * ------------------------------------------------------------ */
void setup() {
  
    Serial.begin(9600);
    Serial.println("UDPEchoServer 1.0");
    Serial.println("Digilent, Copyright 2011");
    Serial.println("");

    // intialize the stack with a static IP
    DNETcK::begin(ipServer);
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
