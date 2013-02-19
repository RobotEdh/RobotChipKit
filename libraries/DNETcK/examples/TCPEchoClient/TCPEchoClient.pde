/************************************************************************/
/*                                                                      */
/*    TCPEchoClient                                                     */
/*                                                                      */
/*    A chipKIT DNETcK TCP Client application to                        */
/*    demonstrate how to use the TcpClient Class.                       */
/*    This can be used in conjuction  with TCPEchoServer                */        
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

char * szIPServer = "192.168.1.180";
unsigned short portServer = 44300;    
   
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

TcpClient tcpClient;
byte rgbRead[1024];
int cbRead = 0;

// this is for Print.write to print
byte rgbWrite[] = {'*','W','r','o','t','e',' ','f','r','o','m',' ','p','r','i','n','t','.','w','r','i','t','e','*','\n'};
int cbWrite = sizeof(rgbWrite);

// this is for tcpClient.writeStream to print
byte rgbWriteStream[] = {'*','W','r','o','t','e',' ','f','r','o','m',' ','t','c','p','C','l','i','e','n','t','.','w','r','i','t','e','S','t','r','e','a','m','*','\n'};
int cbWriteStream = sizeof(rgbWriteStream);

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
 *      connection to the TCPEchoServer
 *      Use DHCP to get the IP, mask, and gateway
 *      by default we connect to port 11000
 *      
 * ------------------------------------------------------------ */
void setup() {
  
    Serial.begin(9600);
    Serial.println("TCPEchoClient 1.0");
    Serial.println("Digilent, Copyright 2011");
    Serial.println("");

    // use DHCP to get our IP and network addresses
    DNETcK::begin();

    // make a connection to our echo server
    tcpClient.connect(szIPServer, portServer);
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
 *      We are using the default timeout values for the DNETck and TcpClient class
 *      which usually is enough time for the Tcp functions to complete on their first call.
 *
 *      This code will write  some stings to the server and have the server echo it back
 *      
 * ------------------------------------------------------------ */
void loop() {
    int cbRead = 0;

    switch(state)
    {

       // write out the strings  
       case WRITE:
            if(tcpClient.isConnected())
                {     
                Serial.println("Got Connection");
  
                tcpClient.writeStream(rgbWriteStream, cbWriteStream);
 
                // write() should all fail to compile
                // while write is inherited from Print, we Hide this in the TcpClient class
                // as writeStream should be used; and we don't want confusing and competing calls
                // we just want to inherit the print() and println() methods from Print in TcpClient
                //tcpClient.write("Printed from tcpClient.write");
                //tcpClient.write(rgbWrite, cbWrite);
                //tcpClient.write((uint8_t) 'b');

                // check that print() and println() work
                tcpClient.print("*Printed from tcpClient.print*\n");
                tcpClient.println("*Printed from tcpClient.println*");

                // however, tcpClient "is-a" Print, so if I pass it as a Print
                // interally it should work as-a Print.
                printWrite(tcpClient);

                Serial.println("Bytes Read Back:");
                state = READ;
                tStart = (unsigned) millis();
                }
            break;

        // look for the echo back
        case READ:

            // see if we got anything to read
            if((cbRead = tcpClient.available()) > 0)
            {
                cbRead = cbRead < sizeof(rgbRead) ? cbRead : sizeof(rgbRead);
                cbRead = tcpClient.readStream(rgbRead, cbRead);
 
                for(int i=0; i < cbRead; i++) 
                {
                    Serial.print(rgbRead[i], BYTE);
                }
            }

            // give us some time to get everything echo'ed back
            else if( (((unsigned) millis()) - tStart) > tWait )
            {
                Serial.println("");
                state = CLOSE;
            }
            break;

        // done, so close up the tcpClient
        case CLOSE:
            tcpClient.close();
            Serial.println("Closing TcpClient, Done with sketch.");
            state = DONE;
            break;

        case DONE:
        default:
            break;
    }

    // keep the stack alive each pass through the loop()
    DNETcK::periodicTasks(); 
}

/***    void printWrite(Print& print)
 *
 *    Parameters:
 *          print - This is a Print object to check to see if it works
 *              
 *    Return Values:
 *          None
 *
 *    Description: 
 *    
 *      
 *      If we pass a TcpClient into a fuction taking Print, all Print methods should
 *      work as TcpClient "is-a" Print.
 *      
 * ------------------------------------------------------------ */
void printWrite(Print& print)
{

    // check the print() and println() methods
    tcpClient.print("*Printed from print.print*\n");
    tcpClient.println("*Printed from print.println*");

    // While these are hidden from TcpClient
    // they should not be hidden from Print
    // these should all work.
    print.write((uint8_t) 'b');
    print.write("\n*Wrote from print.write*\n");
    print.write(rgbWrite, cbWrite);
}
