/************************************************************************/
/*																		*/
/*	RemoteWol	Sends, receives or broadcast a Magic Packet             */
/*                  on a local subnet to wake the sleeping              */
/*					computer with the specified MAC address	            */
/*																		*/
/************************************************************************/
/*	Author: 	Keith Vogel 											*/
/*	Copyright 2011  									                */
/************************************************************************/
/*
  This source is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This source is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  To get a copy of the GNU Lesser General Public
  License write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*	An issue with Wake-On-LAN is that most routers and gateways			*/
/*	block broadcast messages and it is difficult or impossible			*/
/*	to remotely wake up a sleeping computer outside of your subnet		*/
/*	as the WOL broadcast message will not make it though the router.	*/
/*																		*/
/*	If there is a server always running on the local subnet where		*/
/*	other computers are sleeping and you would like to remotely	        */
/*	wake them up, so for example, so you can remote desktop into them. 	*/
/*	This application can listen on a port for a TCIP request to send	*/
/*	a WOL broadcast message and wake the sleeping computer.				*/
/*																		*/
/*	It may seem odd to provide a solution to get to a sleeping computer */
/*  that requires another computer to alwasy be running. However,		*/
/*	if you have a WEB, FTP or other server running that has limited		*/
/*	functionality and is well protected from threats and you would like */
/*	your personal computer to usually be sleeping and off the network, 	*/
/*	this program allows for waking up the personal computer only 	    */
/*	when needed.														*/
/*																		*/
/*	This program acts as 4 compoents, a WOL server, a WOL client,	    */
/*	an immediate Magic Packet Broadcast application, or an application	*/
/*	to request a broadcast history from a server						*/
/*																		*/
/*																		*/
/*	As a server it will wait and listen on the specified port		    */
/*	and when a MAC address is sent to it, it will re-broadcast			*/
/*	that MAC as a WOL Magic Packet on the local subnet to wake up		*/
/*	the sleeping computer. The network card on the sleeping computer	*/
/*	must be configured for Magic Packet Wake-up for this to work; see   */
/*	your computer documentation or search online for Wake-On-LAN.		*/
/*	To get to your local network, your router will probably need        */
/*	to port forward the servers port to the machine you are running	    */
/*	the RemoteWol Server			                                    */
/*																		*/
/*	As a client, RemoteWol will send to the IP and port of              */
/*	a the listening RemoteWOL server, the MAC address that the server   */
/*	should broadcast the WOL packet. That is, the MAC address of the    */
/*	machine you want to wake up.										*/
/*																		*/
/*	As an appliation, RemoteWol will immediately broadcast on its		*/
/*	local network the WOL Magic Packet without connecting to a 			*/
/*	RemoteWol server.													*/
/*																		*/
/*	To request a broadcast history, this gets a list of 				*/												
/*	time, MAC, and IP's that requested WOL broadcasts from the server   */
/*																		*/
/*	Because it might be undesirable to run a machine on a local network	*/
/*	exclusively to run the RemoteWol server, as part of the 			*/
/*	chipKIT(tm) Arduino complient MAX32 and Network Sheild examples,	*/
/*	there is an example sketch that implements the server RemoteWol		*/
/*	application, and the client RemoteWol can trigger the MAX32			*/
/*	to issue the local WOL Magic Packet, thus eliminating the need		*/
/*	to run a computer continously on the local network. 				*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	5/20/2011(KeithV): Created											*/
/*	8/30/2011(KeithV): Modified to work with the MAX32 / Network Shield	*/
/*	9/17/2011(KeithV): Modified to take packet commands					*/
/*																		*/
/************************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Diagnostics;
using System.Net.NetworkInformation;
using System.Runtime.InteropServices;


// 192.168.1.130 9040 0024E80063FC
// 192.168.1.130 9040 -h

namespace RemoteWol
{
    class RemoteWol
    {

        // these are debug constants to help while debugging.
        const bool fRunServerAndClient = false;          // normal operations, should be false

        static byte[] rgTerm = { 0xFF, 0xFF, 0xFF, 0xFF };
        const int cbMsgHdr = 8;

        /***	void Main(string[] args)
         *
         *	Parameters:
         *	
         *      args - command line strings
         *              
         *
         *	Return Values:
         *      None
         *
         *	Errors:
         *
         *
         *	Description: 
         *	
         *      Main entrypoint to the program.
         *      
         *      if only a port number, it is assumed to be a server side (listening) code
         *      if only a MAC, then it is an immediate broadcast of the Magic Packet.
         *      Otherwise as a client an IP address, Port number, and MAC value is expected.
         *      See help screen info below.
         * ------------------------------------------------------------ */
        static void Main(string[] args)
        {
            IPINFO ipInfo = new IPINFO();
            bool fServer = false;
            bool fClient = false;
            bool fBroadCast = false;
            bool fGetHistory = false;
            DateTimeKind dtk = DateTimeKind.Utc;

            Thread thrdListen = new Thread(ListenForWOLRequest);

            Console.WriteLine("RemoteWol Version {0}", typeof(RemoteWol).Assembly.GetName().Version.ToString());
            Console.WriteLine("Keith Vogel, Copyright 2011");
            Console.WriteLine("");
 
            // if we only have only 1 parameter, then we have either a port number and we are server
            // or we have a MAC and we are to send a Magic Packet broadcast to the MAC
            if (args.Length == 1)
            {
                // if this is a Mac Address, a valid port number would never be 6 bytes long.
                if (args[0].Length == 12 || args[0].Length == 17)
                {
                    fBroadCast = (ipInfo.mac = GetMac(args[0])) != null;
                }
                else
                {
                    fServer = ((ipInfo.port = GetPort(args[0])) != 0);
                }
            }

            // else if we have IP, Port, and MAC, we are sending a WOL request
            else if (args.Length == 3)
            {
                // Get the IP
                if ((ipInfo.ip4 = GetIPv4Address(args[0])) != null)
                {
                    // now get the port
                    if ((ipInfo.port = GetPort(args[1])) !=  0)
                    {
                        // see if this is an option
                        if (args[2].Length == 2 && (args[2][0] == '/' || args[2][0] == '-'))
                        {
                            switch (args[2][1])
                            {
                                case 'h':
                                    fGetHistory = true;
                                    dtk = DateTimeKind.Local;
                                   break;

                                case 'H':
                                    fGetHistory = true;
                                    dtk = DateTimeKind.Utc;
                                    break;

                                default:
                                    break;
                            }
                        }

                        // see if is to send a broadcast
                        else if ((fClient = ((ipInfo.mac = GetMac(args[2])) != null)))
                        {
                            // if we want both server and client to run.
                            fServer = (fRunServerAndClient && fClient);
                        }
                    }
                }
            }

            // if we are to run the server
            if (fServer)
            {
                // this starts a thread and will return immediately
                thrdListen.Start(ipInfo);

                // this is so we don't start the client too fast if the client is to be started as well.
                Thread.Sleep(5000); // give it sometime to start the server

                if (fClient)
                {
                    SenderWOLRequest(ipInfo);
                }
            }

            // if we are to run the client
            else if (fClient)
            {
                SenderWOLRequest(ipInfo);
            }

            // if we are to broadcast the Magic Packet
            else if (fBroadCast)
            {
                // we only care about the MAC address
               BroadCast(ipInfo.mac);
            }
            else if (fGetHistory)
            {
                PrintBroadcastHistory(ipInfo, dtk);
            }

            // This is the Help Screen Info
            // if we are to do nothing, print help.
            else
            {
                Console.WriteLine("Server:\t\tRemoteWol Port");
                Console.WriteLine("Client:\t\tRemoteWol ServerURL Port MAC");
                Console.WriteLine("Remote Request:\tRemoteWol ServerURL Port -X");
                Console.WriteLine("WOL Broadcast:\tRemoteWol MAC");
                Console.WriteLine("");
                Console.WriteLine("Where:");
                Console.WriteLine("");
                Console.WriteLine("    ServerURL:\tis the IP/DNS Name of the server to transmit the WOL.");
                Console.WriteLine("    Port:\tis the decimal port number the server is listening on.");
                Console.WriteLine("    MAC:\tis the 12 hex character MAC Address of the machine to wake up,");
                Console.WriteLine("    \t\t    colons \":\" and dashes \"-\" are allowed between hex bytes.");
                Console.WriteLine("    X:");
                Console.WriteLine("    \t    h:\tRequest broadcast history displayed in local time.");
                Console.WriteLine("    \t    H:\tRequest broadcast history displayed in UTC time.");
                Console.WriteLine("");
                Console.WriteLine("Example:");
                Console.WriteLine("");
                Console.WriteLine("    RemoteWol mynetwork.com 2000 65:24:EF:04:23:FC");
            }

            return;
        }

        /***	PrintBroadcastHistor(IPINFO ipInfo)
         * 
         *	Parameters:
         *               
         *      ipInfo  - The global class containing all of the IP like info about the
         *                this machine we are running on; and the server we want to contact.
         * 
         *	Return Values:
         *      None
         *          
         *	Errors:
         *
         *
         *	Description: 
         *
         *      This is the start of the client side RemoteWol code.
         * ------------------------------------------------------------ */
        private static void PrintBroadcastHistory(IPINFO ipInfo, DateTimeKind dtk)
        {
            TcpClient tcpClient = new TcpClient();
            IPEndPoint ipEndPoint = new IPEndPoint(ipInfo.ip4, ipInfo.port);
            NetworkStream targetStrm = null;

            try
            {
                // connect to the server; there is some default timeout, it does break out if it can not connect.
                tcpClient.Connect(ipEndPoint);
                targetStrm = tcpClient.GetStream();

                MSG msg = new MSG(MSG.CMDMSG.HistoryRequest);

                // ask that the magic packet be sent
                if (WriteMsgToStream(targetStrm, msg))
                {
                    msg = ReadMsgFromStream(targetStrm);
                }

                // See what we got back, it should be a history reply
                if (msg != null && msg.cmdmsg == MSG.CMDMSG.HistoryReply && msg.rgbData.Length % BHE.Length == 0)
                {
                    byte[] rgbBHE = new byte[BHE.Length];
                    BHE bhe = null;

                    Console.WriteLine("Got the broadcast history.");
                    for (int i = 0; i < msg.rgbData.Length; i += BHE.Length)
                    {
                        // get a bhe
                        Array.Copy(msg.rgbData, i, rgbBHE, 0, BHE.Length);
                        bhe = new BHE(rgbBHE);

                        switch(dtk)
                        {
                            case DateTimeKind.Utc:
                                Console.WriteLine("UTC Time: {0}, MAC: {1} from IP: {2}.",
                                                    bhe.UtcTime.ToString("G"),
                                                    bhe.MAC.ToString(),
                                                    bhe.IP.ToString());
                              break;

                            case DateTimeKind.Local:
                                Console.WriteLine("Local Time: {0}, MAC: {1} from IP: {2}.",
                                                    bhe.UtcTime.ToLocalTime().ToString("G"),
                                                    bhe.MAC.ToString(),
                                                    bhe.IP.ToString());
                               break;
                        }
                    }
                }
                else
                {
                    Console.WriteLine("Unable to get Broadcast History.");
                }

                Console.WriteLine("");

                // success or failure, close up the tcp Client
                targetStrm.Close();
                tcpClient.Close();
            }

            // we didn't even connect, probably the IP address is bad.
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
                return;
            }
        }

        /***	SenderWOLRequest(IPINFO ipInfo)
         * 
         *	Parameters:
         *               
         *      ipInfo  - The global class containing all of the IP like info about the
         *                this machine we are running on; and the server we want to contact.
         * 
         *	Return Values:
         *      None
         *          
         *	Errors:
         *
         *
         *	Description: 
         *
         *      This is the start of the client side RemoteWol code.
         * ------------------------------------------------------------ */
        private static void SenderWOLRequest(IPINFO ipInfo)
        {
            TcpClient tcpClient = new TcpClient();
            IPEndPoint ipEndPoint = new IPEndPoint(ipInfo.ip4, ipInfo.port);
            NetworkStream targetStrm = null;

            try
            {

                // connect to the server; there is some default timeout, it does break out if it can not connect.
                tcpClient.Connect(ipEndPoint);
                targetStrm = tcpClient.GetStream();


                // send the MAC
                if (ipInfo.mac != null)
                {
                    MSG msg = new MSG();
                    BHE bhe = new BHE();

                    msg.cmdmsg = MSG.CMDMSG.BroadCastMAC;
                    msg.rgbData = ipInfo.mac.GetAddressBytes();

                    // ask that the magic packet be sent
                    if (WriteMsgToStream(targetStrm, msg))
                    {
                        msg = ReadMsgFromStream(targetStrm);
                    }

                    // See what we got back, it should only be the MAC we sent
                    if (msg != null && msg.cmdmsg == MSG.CMDMSG.Succeeded)
                    {
                        Console.WriteLine("Magic Packet Sent.");
                    }
                    else
                    {
                        Console.WriteLine("Unable to send Magic Packet");
                    }
                }

                Console.WriteLine("");

                // success or failure, close up the tcp Client
                targetStrm.Close();
                tcpClient.Close();
            }

            // we didn't even connect, probably the IP address is bad.
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
                return;
            }
        }

        /***	void ListenForWOLRequest(Object obj)
         * 
         *	Parameters:
         *               
         *      obj -       The obj is really an ipInfo and is casted to such.
         *                  The ipInfo class containing all of the IP like info about the
         *                  this machine we are running on; and the server we want to contact.
         * 
         *	Return Values:
         *      None
         *          
         *	Errors:
         *
         *
         *	Description: 
         *
         *      This is the start of the server side RemoteWol code.
         *      
         *      This was written as a seperate thread so that debugging of
         *      the client in the main thread against a server in a different
         *      thread could be done in one debug session. Client / Server
         *      combined application is only when the debug flag fRunServerAndClient is TRUE.
         * ------------------------------------------------------------ */
        private static void ListenForWOLRequest(Object obj)
        {
            IPINFO ipInfo = (IPINFO) obj;
            uint terminate = 0;

            TcpListener tcpListener = null;

            try
            {
  
                // this will throw and exception if ipAddress == null;
                tcpListener = new TcpListener(ipInfo.myIPs.ipMe, ipInfo.port);
                tcpListener.Start();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
                return;
            }

            // go in a loop waiting for someone to connect.
            do
            {
                TcpClient client = null;
                NetworkStream targetStrm = null;
                MSG msg; ;

                try
                {
                    // block until we get something
                    Console.WriteLine("Listening on port: {0}.", ipInfo.port.ToString());
                    client = tcpListener.AcceptTcpClient();
                    targetStrm = client.GetStream();

                    // see if we can read something reasonable
                    msg = ReadMsgFromStream(targetStrm);
                    if (msg != null)
                    {
                        Console.WriteLine("{0}: Message {1} detected.", DateTime.Now.ToString("g"), msg.cmdmsg.ToString());
                        switch (msg.cmdmsg)
                        {

                            case MSG.CMDMSG.BroadCastMAC:

                                // see if we have a valid MAC
                                if (msg.rgbData.Length >= 6)
                                {
                                    MSG msgRet = new MSG(MSG.CMDMSG.Failed);
                                    BHE bhe = new BHE();
  
                                    // we only care about the MAC address
                                    Array.Resize(ref msg.rgbData, 6);
                                    PhysicalAddress macAddress = new PhysicalAddress(msg.rgbData);
                                    IPEndPoint ipEP = (IPEndPoint) client.Client.RemoteEndPoint;

                                    Console.WriteLine("Request to broadcast MAC: {0} by IP: {1}.", macAddress.ToString(), ipEP.ToString());

                                    bhe.IP = ipEP.Address;
                                    bhe.MAC = macAddress;
                                    bhe.UtcTime = DateTime.Now.ToUniversalTime();
                                    ipInfo.bhs.AddBHE(bhe);

                                    // do the broadcast
                                    msg.cmdmsg = MSG.CMDMSG.Failed;
                                    if (BroadCast(macAddress))
                                    {
                                        msgRet.cmdmsg = MSG.CMDMSG.Succeeded;
                                    }
 
                                    // write back our status code
                                    WriteMsgToStream(targetStrm, msgRet);
                                }

                                break;

                            case MSG.CMDMSG.HistoryRequest:
                                WriteMsgToStream(targetStrm, ipInfo.bhs.GetBroadcastHistoryReplyMsg());
                                break;

                            case MSG.CMDMSG.HistoryReply:
                                WriteMsgToStream(targetStrm, new MSG(MSG.CMDMSG.Failed));
                                break;

                            case MSG.CMDMSG.Terminate:
                                WriteMsgToStream(targetStrm, new MSG(MSG.CMDMSG.Succeeded));
                                terminate = 0xFFFFFFFF;
                                break;

                            default:
                                break;
                        }
                    }

                    // we have done our work, close the stream
                    targetStrm.Close();
                    client.Close();
                }

                // something bad happened, but we want to just print the exception and go back and wait
                // for another connection.
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                    terminate = 0;
                }

            } while (terminate != 0xFFFFFFFF);

            // just stop listening, we are done.
            tcpListener.Stop();
        }

        /***	MSG ReadMsgFromStream(NetworkStream targetStrm)
         * 
         *	Parameters:
         *               
         *      targetStrm - The Stream to read the message from
         * 
         *	Return Values:
         *      msg         - The message packet, null if there was an error
         *          
         *	Errors:
         *
         *
         *	Description: 
         *	
         *      Read a full message packet, or timesout and returns null
         *      
         * ------------------------------------------------------------ */
        private static MSG ReadMsgFromStream(NetworkStream targetStrm)
        {
            const int cbBufferBlock = 1024;
            DTBT dtbt = new DTBT();
            int cbRead = 0;
            byte[] rgbRcv = new byte[cbBufferBlock];
            bool fGotHeader = false;
            int cbNeed = 8;     // got to get the header
            MSG msg = new MSG();

            try
            {
                // Set the timeout so we don't wait too long
                targetStrm.ReadTimeout = 10000;     // set to 10 seconds for the read

                // loop until we get the whole data packet
                while (cbRead < cbNeed)
                {
                    // will throw and exception if this times out
                    cbRead += targetStrm.Read(rgbRcv, cbRead, rgbRcv.Length - cbRead);

                    // if we need more space
                    if (cbRead >= rgbRcv.Length)
                    {
                        Array.Resize(ref rgbRcv, rgbRcv.Length + cbBufferBlock);
                    }

                    if (!fGotHeader && rgbRcv.Length >= 8)
                    {
                        // get the command 
                        dtbt.b0 = rgbRcv[0];
                        dtbt.b1 = rgbRcv[1];
                        dtbt.b2 = rgbRcv[2];
                        dtbt.b3 = rgbRcv[3];
                        msg.cmdmsg = (MSG.CMDMSG)dtbt.i;

                        // find out how many more bytes must come in.
                        dtbt.b0 = rgbRcv[4];
                        dtbt.b1 = rgbRcv[5];
                        dtbt.b2 = rgbRcv[6];
                        dtbt.b3 = rgbRcv[7];
                        cbNeed = dtbt.i + 8;

                        // yeah, junk came in.
                        if (cbNeed < 8)
                        {
                            return (null);
                        }

                        fGotHeader = true;
                    }
                }

                // copy over just the data.
                msg.rgbData = new byte[cbRead - 8];
                Array.Copy(rgbRcv, 8, msg.rgbData, 0, cbRead - 8);
            }

            // something bad happened, but we want to just print the exception and go back and wait
            // for another connection.
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
                return (null);
            }

            return (msg);
        }

        /***	bool WriteMsgToStream(NetworkStream targetStrm, MSG msg)
         * 
         *	Parameters:
         *               
         *      targetStrm - The Stream to write the message to
         *      msg         - msg to write to the stream.
         * 
         *	Return Values:
         *      true - a message was read
         *      false, no message was read, something went wrong
         *          
         *	Errors:
         *
         *
         *	Description: 
         *	
         *	    Write out a complete message packet
         *	
         * ------------------------------------------------------------ */
        private static bool WriteMsgToStream(NetworkStream targetStrm, MSG msg)
        {
            byte[] rgbHeader = new byte[8];
            DTBT dtbt = new DTBT();

            // put the command in
            dtbt.i = (int) msg.cmdmsg;
            rgbHeader[0] = dtbt.b0;
            rgbHeader[1] = dtbt.b1;
            rgbHeader[2] = dtbt.b2;
            rgbHeader[3] = dtbt.b3;

            // and how much data is written
            dtbt.i = msg.rgbData.Length;
            rgbHeader[4] = dtbt.b0;
            rgbHeader[5] = dtbt.b1;
            rgbHeader[6] = dtbt.b2;
            rgbHeader[7] = dtbt.b3;

            try
            {

                // Set the timeout so we don't wait too long
                targetStrm.WriteTimeout = 10000;     // set to 10 seconds for the write

                // write out the header
                targetStrm.Write(rgbHeader, 0, rgbHeader.Length);

                // write out the message if any
                if (msg.rgbData.Length > 0)
                {
                    targetStrm.Write(msg.rgbData, 0, msg.rgbData.Length);
                }
            }

            // something bad happened, but we want to just print the exception and go back and wait
            // for another connection.
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
                return (false);
            }

           return (true);
        }

        /***	bool BroadCast(PhysicalAddress macAddress)
         * 
         *	Parameters:
         *               
         *      rgMAC - The MAC address to broadcast as a Magic Packet
         * 
         *	Return Values:
         *      TRUE is the broadcast succeeded, FALSE otherwise.
         *      Since this is UDP, it probably succeeded.
         *          
         *	Errors:
         *
         *
         *	Description: 
         *	
         *      This creates a Magic Packet with the specified MAC address of 
         *      the machine to wake up and broadcasts it.
         *      
         *      A Magic Packet is 6 0xFF followed by 16 
         *      copies of the MAC address.
         * ------------------------------------------------------------ */
        private static bool BroadCast(PhysicalAddress macAddress)
        {
            int i = 0;
            byte[] rgDataGram = new byte[102]; // 6 0xFF and 16 * 6 byte MAC = 17 * 6 = 102 bytes

            UdpClient updClient = new UdpClient();
            IPEndPoint ipBroadCast = new IPEndPoint(IPAddress.Broadcast, 0xFF);

            int cbSent = 0;

            // build the datagram
            // first there must be 6 bytes of 0xFF;
            for (i = 0; i < 6; i++) rgDataGram[i] = 0xFF;

            // then 16 MAC 
            for (int j = 0; j < 16; j++)
            {
                macAddress.GetAddressBytes().CopyTo(rgDataGram, (j + 1) * 6);
            }

            updClient.EnableBroadcast = true;
            cbSent = updClient.Send(rgDataGram, rgDataGram.Length, ipBroadCast);

            if (cbSent == rgDataGram.Length)
            {
               Console.WriteLine("Magic Packet Sent.");
               return (true);
            }
            else
            {
                return (false);
            }
        }

        /***	IPAddress GetIPv4Address(string szIP)
         * 
         *	Parameters:
         *               
         *      szIP -  The hostname to get the IP address for
         * 
         *	Return Values:
         *      And IPv4 ip address if one is found
         *          
         *	Errors:
         *
         *	Description: 
         *	
         *      This resolves the hostname (szIP) to an binary IP address via DNS
         *      It looks specifically for an IPv4 IP address as we do not support IPv6.
         * ------------------------------------------------------------ */
        private static IPAddress GetIPv4Address(string szIP)
        {
            byte[] ipAddress = new byte[0];

            try
            {
                // get the server to talk to
                IPAddress[] rgIPAddr = Dns.GetHostAddresses(szIP);

                // I have to find a IP4 address; don't support anything else
                foreach (IPAddress ipAddr in rgIPAddr)
                {
                    if (ipAddr.AddressFamily == AddressFamily.InterNetwork)
                    {
                        // looking for an IPv4 address
                        if (ipAddr.GetAddressBytes().Length == 4)
                        {
                            return(ipAddr);
                        }
                        break;
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception: " + e.ToString());
            }

            Console.WriteLine("Invalid IPv4 Address: " + szIP);
            Console.WriteLine("");
            return (null);
        }

        /***	bool GetPort(string szPort, IPINFO ipd)
         * 
         *	Parameters:
         *               
         *      szPort -    The port number represented as a string
         *      ipd -   This is the global data structure to save our server Port into
         * 
         *	Return Values:
         *      true if we got a valid port number, false otherwise
         *          
         *	Errors:
         *
         *	Description: 
         *	
         *      Very simple, convert a string port number into a number port number
         *	
         * ------------------------------------------------------------ */
        private static int GetPort(string szPort)
        {
            int port = 0;

            try
            {
                port = Convert.ToInt32(szPort);
                return (port);
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception: " + e.ToString());
            }

            // if we had an error, print out that this is not a good Port
            Console.WriteLine("Invalid Port: " + szPort);
            Console.WriteLine("");

            return (0);
        }

        /***	PhysicalAddress GetMac(string szMAC)
         * 
         *	Parameters:
         *               
         *      szMAC -    The MAC number represented as a string
         * 
         *	Return Values:
         *      The physical address (MAC).
         *          
         *	Errors:
         *
         *	Description: 
         *	
         *      Very simple, convert a string MAC number into a number MAC number
         *	
         * ------------------------------------------------------------ */
        private static PhysicalAddress GetMac(string szMAC)
        {
            PhysicalAddress mac = null;

            // MAC Addresses are 6 bytes (12 characters) long, plus maybe 5 characters for : or -
            if (szMAC.Length == 12 || szMAC.Length == 17)
            {
                try
                {
                    mac = PhysicalAddress.Parse((String.Copy(szMAC)).Replace(':', '-').ToUpper());
                    return (mac);
                }
                catch (Exception e)
                {
                    Console.WriteLine("Exception: " + e.ToString());
                    Console.WriteLine("Invalid MAC Adress: " + szMAC);
                    Console.WriteLine("");
               }
            }

            return (null);
         }

        /*				IPINFO Class Implementation		
         * ------------------------------------------------------------ **
         * 
         *	Description: Global data structure to pass around to
         *	             all of the routines to use.
         *
         * ------------------------------------------------------------ */
        private class IPINFO
        {
            public MYIPS myIPs = new MYIPS();
            public IPAddress ip4 = null;
            public int port = 0;
            public PhysicalAddress mac = null;
            public BHS bhs = new BHS();
        }

        /*				MYIPS Class Implementation		
         * ------------------------------------------------------------ **
         * 
         *	Description: Represents the current machines
         *	             IP, Gateway, and submask
         *
         *
         * ------------------------------------------------------------ */
        private class MYIPS
        {
            public IPAddress ipMe = null;
            public IPAddress ipMask = null;
            public IPAddress ipGateway = null;

            /*				MYIPS Class Constructor	
             * ------------------------------------------------------------ **
             * 
             *	Description:
             *	
             *          This constructs the MYIPS class filling in 
             *          the IP address of the current machine
             *          along with the gateway and subnet mask.
             *
             *
             * ------------------------------------------------------------ */
            public MYIPS()
            {
                NetworkInterface[] adapters = NetworkInterface.GetAllNetworkInterfaces();

                // just look at all of the adaptors connected to this machine.
                foreach (NetworkInterface adaptor in adapters)
                {
                    // skip the loop back address
                    if (adaptor.NetworkInterfaceType == NetworkInterfaceType.Loopback) continue;

                    UnicastIPAddressInformationCollection UnicastIPInfoCol = adaptor.GetIPProperties().UnicastAddresses;
                    GatewayIPAddressInformationCollection listGateway = adaptor.GetIPProperties().GatewayAddresses;

                    foreach (UnicastIPAddressInformation ipInfo in UnicastIPInfoCol)
                    {
                        // here we guess what adaptor to pick
                        // there could be several, so go ahead and pick the first IPv4 IP
                        // Who know if this is the correct adaptor; but typically
                        // we only connect to 1 network at a time
                        // except maybe a wired connection and a wireless one; on the same network.
                        if (!IPAddress.IsLoopback(ipInfo.Address) && ipInfo.Address.AddressFamily == AddressFamily.InterNetwork)
                        {
                            ipMe = ipInfo.Address;
                            ipMask = ipInfo.IPv4Mask;
                            break;
                        }
                    }

                    // found it, get out.
                    if (ipMe != null)
                    {
                        // there should be a gateway for this adaptor
                        // probably only one gateway, but take the first one.
                        if (listGateway.Count > 0)
                        {
                            ipGateway = listGateway[0].Address;
                        }
                        break;
                    }
                }
            }
        }

        /*				MSG Class Implementation		
         * ------------------------------------------------------------ **
         * 
         *	Description: This is a message packet sent between
         *	             RemoteWol Client / Server
         *
         *
         * ------------------------------------------------------------ */
        private class MSG
        {
            public enum CMDMSG : int
            {
                None = 0,
                Succeeded = 1,
                Failed = 2,
                BroadCastMAC = 3,
                HistoryRequest = 4,
                HistoryReply = 5,
                Terminate = 6
            }
      
            public CMDMSG cmdmsg;
            public byte[] rgbData;

            // constructor
            public MSG()
            {
                cmdmsg = CMDMSG.None;
                rgbData = new byte[0];
            }

            // constructor
            public MSG(CMDMSG cmd)
            {
                cmdmsg = cmd;
                rgbData = new byte[0];
            }

            // constructor
            public MSG(CMDMSG cmd, byte[] rgb)
            {
                cmdmsg = cmd;

                if (rgb == null)
                {
                    rgbData = new byte[0];
                }
                else
                {
                    rgbData = rgb;  
                }
            }
        }

        /*				BHE Class Implementation		
         * ------------------------------------------------------------ **
         * 
         *	Description: This is Broadcast History Store
         *	
         * 
         *      In C/C++ this is how the BHE looks, get your constans correct!
         * 
         *      typedef struct {
         *          unsigned int epochTime;
         *          uint32_t u32IP;
         *          byte rgbMAC[6];
         *          byte flags1;
         *          byte flags2;
         *      } BHE;
         *
         *
         * ------------------------------------------------------------ */
        private class BHE
        {

            // these are you BHE constants, get them right!
            public const int Length = 16;
            private const int iEpochTime = 0;
            private const int iIP = 4;
            private const int iMAC = 8;
            private const int iFlag1 = 14;
            private const int iFlag2 = 15;
            private const int cbMACAddress = 6;    // length of a MAC address
            private const int cbIPAddress = 4;    // length of an IP address

            private const long cTicksPerSec = 10000000;
            private readonly long ticksEpoch = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc).Ticks;

            private byte[] rgbBHEBuffer = null;

            public BHE()
            {
                rgbBHEBuffer = new byte[Length];
            }

            public BHE(byte[] rgbBHE_C)
            {
                Debug.Assert(rgbBHE_C.Length == Length);
                rgbBHEBuffer = rgbBHE_C;
            }

            public BHE(MSG msg)
            {
                Debug.Assert(msg.cmdmsg == MSG.CMDMSG.BroadCastMAC && msg.rgbData.Length == Length);
                rgbBHEBuffer = msg.rgbData;
            }

            public PhysicalAddress MAC
            {
                get
                {
                    byte[] rgbMAC = new byte[cbMACAddress];
                    Array.Copy(rgbBHEBuffer, iMAC, rgbMAC, 0, cbMACAddress);
                    return (new PhysicalAddress(rgbMAC));
                }
                set
                {
                    Array.Copy(value.GetAddressBytes(), 0, rgbBHEBuffer, iMAC, cbMACAddress);
                }
            }

            public IPAddress IP
            {
                get
                {
                    byte[] rgbIP = new byte[cbIPAddress];
                    Array.Copy(rgbBHEBuffer, iIP, rgbIP, 0, cbIPAddress);
                    return (new IPAddress(rgbIP));
                }
                set
                {
                    Array.Copy(value.GetAddressBytes(), 0, rgbBHEBuffer, iIP, cbIPAddress);
                }
            }

            public DateTime UtcTime
            {
                get
                {
                    int i = iEpochTime;
                    DTBT epochTime = new DTBT();

                    epochTime.b0 = rgbBHEBuffer[i++];
                    epochTime.b1 = rgbBHEBuffer[i++];
                    epochTime.b2 = rgbBHEBuffer[i++];
                    epochTime.b3 = rgbBHEBuffer[i++];

                    return (new DateTime((EpochTime * cTicksPerSec) + ticksEpoch, DateTimeKind.Utc));
                }
                set
                {
                    int i = iEpochTime;
                    DTBT epochTime = new DTBT();

                    epochTime.ui = (uint)((value.Ticks - (long)ticksEpoch) / cTicksPerSec);
                    rgbBHEBuffer[i++] = epochTime.b0;
                    rgbBHEBuffer[i++] = epochTime.b1;
                    rgbBHEBuffer[i++] = epochTime.b2;
                    rgbBHEBuffer[i++] = epochTime.b3;
                }
            }

            public uint EpochTime
            {
                get
                {
                    int i = iEpochTime;
                    DTBT epochTime = new DTBT();

                    epochTime.b0 = rgbBHEBuffer[i++];
                    epochTime.b1 = rgbBHEBuffer[i++];
                    epochTime.b2 = rgbBHEBuffer[i++];
                    epochTime.b3 = rgbBHEBuffer[i++];

                    return (epochTime.ui);
                }
                set
                {
                    int i = iEpochTime;
                    DTBT epochTime = new DTBT();

                    epochTime.ui = value;

                    rgbBHEBuffer[i++] = epochTime.b0;
                    rgbBHEBuffer[i++] = epochTime.b1;
                    rgbBHEBuffer[i++] = epochTime.b2;
                    rgbBHEBuffer[i++] = epochTime.b3;
                }
            }

            public byte[] rgbBHE
            {
                get
                {
                    return (rgbBHEBuffer);
                }
            }
        }

        /*				BHS Class Implementation		
         * ------------------------------------------------------------ **
         * 
         *	Description: This is Broadcast History Store
         *	
         * 
         *      In C/C++ this is how the BHE looks, get your constans correct!
         * 
         *      typedef struct {
         *          unsigned int epochTime;
         *          uint32_t u32IP;
         *          byte rgbMAC[6];
         *          byte flags1;
         *          byte flags2;
         *      } BHE;
         *
         *
         * ------------------------------------------------------------ */
        private class BHS
        {
            
            private const int cHistoryEntries = 1024;
            List<BHE> listBHE = null;
            private int iNext = 0;
 
            public BHS()
            {
                listBHE = new List<BHE>(cHistoryEntries);
            }

            public BHS(int cBHE)
            {
                listBHE = new List<BHE>(cBHE);
            }

            public MSG GetBroadcastHistoryReplyMsg()
            {
                int i = 0;
                int iStart = 0;
                MSG msg = new MSG(MSG.CMDMSG.HistoryReply, new byte[listBHE.Count * BHE.Length]);

                // get the end of the buffer if any
                if(iNext < listBHE.Count)
                {
                    for(i = 0, iStart = iNext; iStart < listBHE.Count; iStart++, i++)
                    {
                        Array.Copy(listBHE[iStart].rgbBHE, 0, msg.rgbData, i * BHE.Length, BHE.Length);
                    }
                }

                // get the start of the buffer
                for(iStart = 0; iStart < iNext; iStart++, i++)
                {
                         Array.Copy(listBHE[iStart].rgbBHE, 0, msg.rgbData, i * BHE.Length, BHE.Length);
                }

                return (msg);
            }

            public void AddBHE(BHE bhe)
            {
                if (iNext < listBHE.Count)
                {
                    listBHE[iNext] = bhe;
                }
                else
                {
                    listBHE.Add(bhe);
                }

                iNext = (iNext + 1) % (int)listBHE.Capacity;
            }
        } 
                                
        /*				DTBT Structure Implementation		
         * ------------------------------------------------------------ **
         * 
         *	Description: This is just so we can switch between
         *	             4 bytes and an int
         *
         *
         * ------------------------------------------------------------ */
        [System.Runtime.InteropServices.StructLayout(LayoutKind.Explicit)]
        private struct DTBT
        {
            [System.Runtime.InteropServices.FieldOffset(0)]
            public int i;

            [System.Runtime.InteropServices.FieldOffset(0)]
            public uint ui;

            [System.Runtime.InteropServices.FieldOffset(0)]
            public byte b0;

            [System.Runtime.InteropServices.FieldOffset(1)]
            public byte b1;

            [System.Runtime.InteropServices.FieldOffset(2)]
            public byte b2;

            [System.Runtime.InteropServices.FieldOffset(3)]
            public byte b3;
        }
     }
}

