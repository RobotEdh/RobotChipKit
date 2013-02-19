/************************************************************************/
/*                                                                      */
/*    UDPEchoClient                                                     */
/*                                                                      */
/*    A chipKIT DNETcK UDP Client application to                        */
/*    demonstrate how to use the UdpClient Class.                       */
/*    This can be used in conjuction  with UDPEchoServer                */        
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
/*    12/21/2011(KeithV): Created                                       */
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

char * szIPServer = "192.168.1.180";
unsigned short portServer = 44400; 
   
//******************************************************************************************
//******************************************************************************************
//***************************** END OF CONFIGURATION ***************************************
//******************************************************************************************
//******************************************************************************************

typedef enum
{
    NONE = 0,
    WRITE,
    READ,
    CLOSE,
    DONE,
} STATE;

STATE state = WRITE;

unsigned tStart = 0;
unsigned tWait = 5000;

// must have a datagram cache
byte rgbDatagramCache[2048];
UdpClient udpClient(rgbDatagramCache, sizeof(rgbDatagramCache));

// our sketch datagram buffer
byte rgbRead[1024];
int cbRead = 0;

// this is for udpClient.writeDatagram to write
byte rgbWriteDatagram[] = {'*','W','r','o','t','e',' ','f','r','o','m',' ','u','d','p','C','l','i','e','n','t','.','w','r','i','t','e','D','a','t','a','g','r','a','m','*','\n'};
int cbWriteDatagram = sizeof(rgbWriteDatagram);

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
 *      connection to the UDPEchoServer
 *      Use DHCP to get the IP, mask, and gateway
 *      by default we connect to port 12000
 *      
 * ------------------------------------------------------------ */
void setup() {
  
    Serial.begin(9600);
    Serial.println("UDPEchoClient 1.0");
    Serial.println("Digilent, Copyright 2011");
    Serial.println("");

    // use DHCP to get our IP and network addresses
    DNETcK::begin();

    // make a connection to our echo server
    udpClient.setEndPoint(szIPServer, portServer);
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
 *      We are using the default timeout values for the DNETck and UdpClient class
 *      which usually is enough time for the Udp functions to complete on their first call.
 *
 *      This code will write a sting to the server and have the server echo it back
 *      Remember, UDP is unreliable, so the server may not echo back if the datagram is lost
 *      
 * ------------------------------------------------------------ */
void loop() {
    int cbRead = 0;

    switch(state)
    {

       // write out the strings  
       case WRITE:
            if(udpClient.isEndPointResolved())
                {     
                Serial.println("Writing out Datagram");
  
                udpClient.writeDatagram(rgbWriteDatagram, cbWriteDatagram);


                Serial.println("Waiting to see if a datagram comes back:");
                state = READ;
                tStart = (unsigned) millis();
                }
            break;

        // look for the echo back
         case READ:

            // see if we got anything to read
            if((cbRead = udpClient.available()) > 0)
            {

                cbRead = cbRead < sizeof(rgbRead) ? cbRead : sizeof(rgbRead);
                cbRead = udpClient.readDatagram(rgbRead, cbRead);
 
                for(int i=0; i < cbRead; i++) 
                {
                    Serial.print(rgbRead[i], BYTE);
                }

                // give us some more time to wait for stuff to come back
                tStart = (unsigned) millis();
            }

            // give us some time to get everything echo'ed back
            // or if the datagram is never echoed back
            else if( (((unsigned) millis()) - tStart) > tWait )
            {
                Serial.println("Done waiting, assuming nothing more is coming");
                Serial.println("");
                state = CLOSE;
            }
            break;

        // done, so close up the tcpClient
        case CLOSE:
            udpClient.close();
            Serial.println("Closing udpClient, Done with sketch.");
            state = DONE;
            break;

        case DONE:
        default:
            break;
    }

    // keep the stack alive each pass through the loop()
    DNETcK::periodicTasks(); 
}

