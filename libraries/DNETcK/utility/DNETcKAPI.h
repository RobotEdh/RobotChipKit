/************************************************************************/
/*																		*/
/*	DNETcKAPI.h	--  DNETcK interface APIs to implement the              */
/*                  the interface between the DNETcK C++ and the        */
/*					Microchip MAL                                   	*/
/*																		*/
/************************************************************************/
/*	Author: 	Keith Vogel 											*/
/*	Copyright 2011, Digilent Inc.										*/
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
/*  Module Description: 												*/
/*																		*/
/*	This library is explicity targeting the chipKIT Max32 				*/
/*	PIC32MX795F512L MCU using the internal MAC along with the 			*/
/*  chipKIT Network Shield using the physical SMSC8720 analog driver    */
/*																		*/
/*	This module exposes API's to be used in conjuction with 			*/
/*	DNETcK.cpp to implement an interface between the C++ and        	*/
/*	the Microchip MAL													*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	8/08/2011(KeithV): Created											*/
/*	8/29/2011(KeithV): Added UDP support								*/
/*	12/8/2011(KeithV): Updated for the DNETcK library					*/
/*																		*/
/************************************************************************/
#ifndef _DNETCKAPI_H
#define _DNETCKAPI_H

#ifdef __cplusplus
extern "C"
{

    // typically these are MAL core C APIs where I fool with paramters to make them available in C++
    // I don't need prototypes in C as the MAL header files provide them.

    void StackTask(void);

    bool TCPIsConnected(byte hTCP);
    void TCPClose(byte hTCP);

    unsigned short TCPIsPutReady(byte hTCP);
    void TCPFlush(byte hTCP);
    unsigned short TCPPutArray(byte hTCP, const byte * rgbData, unsigned short cbData);

    unsigned short TCPIsGetReady(byte hTCP);
    void TCPDiscard(byte hTCP);
    unsigned short TCPGetArray(byte hTCP, byte * rgbRead, unsigned short rgbMax);
    bool TCPGet(byte hTCP, byte * bData);

    byte TCPPeek(byte hTCP, unsigned short index);
    unsigned short TCPPeekArray(byte hTCP, byte *rgbPeek, unsigned short cbPeekRequest, unsigned short index);
   
    unsigned long SNTPGetUTCSeconds(void);

    void GetTcpSocketEndPoints(byte hTCP, IPv4 * pRemoteIP, MAC * pRemoteMAC, unsigned short * pRemotePort, unsigned short * pLocalPort);
    void GetUdpSocketEndPoints(byte hUDP, IPv4 * pRemoteIP, MAC * pRemoteMAC, unsigned short * pRemotePort, unsigned short * pLocalPort);

    unsigned short UDPIsGetReady(byte hUDP);
    void UDPDiscard(void);
    void UDPClose(byte hUDP);

    unsigned short UDPIsPutReady(byte hUDP);
    unsigned short UDPPutArray(const byte * rgbWrite, unsigned short cbWrite);
    void UDPFlush(void);

    byte UDPOpenEx(void * forServerIsNull, byte openType, unsigned short localPort, unsigned short remotePort); 

    bool EthernetisInitialized(unsigned long msBlockMax, DNETcK::STATUS * pStatus);
    bool EthernetIsDNSResolved(const char * szHostName, byte * pIP, unsigned long msBlockMax, DNETcK::STATUS * pStatus);
    bool DNETcK_IsMacConnected(DNETcK::STATUS * pStatus);

#else

    bool EthernetIsDNSResolved(const char * szHostName, byte * pIP, unsigned long msBlockMax, STATUS * pStatus);
    bool DNETcK_IsMacConnected(STATUS * pStatus);


#endif

    // typically these are APIs that are implemented to implement the Ethernet Classes
    // but are NOT intrinsically part of the MAL.
    void DNETcK_Init(void);
 
    // BE VERY CAREFUL, while byte arrays are passed as IP address, they must be machine aligned or a processor alignment 
    // fault will occur. Address of IPv4 are passed as IPv4 is properly aligned.

    // initalization routines. The MAL is very bad at initializing it static memory
    // in there int routines, I have added this to init the static/global variables
    // to the "just booted" state. This allows end() and begin() to work.
    void InitDNSStaticMemory(void);
    void InitRebootStaticMemory(void);
    void InitSNTPStaticMemory(void);
    void InitNBNSStaticMemory(void);

    // EThernet APIs
    void EthernetBegin(const byte *rgbIP, const byte *rgbGateWay, const byte *rgbSubNet, const byte *rgbDNS1, const byte *rgbDNS2);
    void EthernetEnd(void);
    void EthernetPeriodicTasks(void);

    bool EthernetSetMAC(const byte *rgbMac);
    void EthernetGetMACandIPs(byte *rgbMac, byte *rgbIP, byte *rgbGateWay, byte *rgbSubNet, byte *rgbDNS1, byte *rgbDNS2);

    void EthernetRequestARPIpMacResolution(const byte * pIP);
    bool EthernetIsARPIpMacResolved(const byte * pIP, byte * pMAC, unsigned long msBlockMax);

    void EthernetDNSTerminate(void);

    // TcpClient
    byte TcpClientConnectByName(const char * szHostName, unsigned short port);
    byte TcpClientConnectByEndPoint(const byte * pIP, unsigned short port);

    // TcpServer
    byte TcpServerStartListening(unsigned short port);

    //UdpClient
    byte UdpClientSetEndPoint(const byte * pIP, const byte * pMAC, unsigned short remotePort, unsigned short localPort);
    byte * ExchangeCacheBuffer(byte hUDP, byte *rgbBuffer, unsigned short cbBuffer);
    unsigned short UdpClientAvailable(byte hUDP);
    void UdpClientEmptyNextDataGram(byte hUDP);
    unsigned short UdpClientRemoveBytesFromDataGram(byte hUDP, unsigned short cbRemove);
    unsigned short UdpClientPeek(byte hUDP, byte *rgbPeek, unsigned short cbPeekMax, unsigned short iIndex);

    // this is a helper macro to insure that timers handle rollover conditions
    // this calcuates the difference of 32 bit counters with 
    // unsigned math so the difference is always < 0xFFFFFFFF. unsigned long is used
    // instead of uint32_t so this macro can be used in both C and C++ code and
    // in files where uint32_t is not defined. C will calculate an unsigned long as 32 bits in
    // both 16 and 32 bit compilers. It does not matter what the units of tStart, tNow, tWait are
    // as long as they are consistant, and that they are at least 32 bits in value. Larger values
    // are fine with the restruction that the larges difference in time that this macro can handle is 0xFFFFFFFE ticks
    // a tWait value of 0xFFFFFFFF will wait forever as the > condition will never be satisfied.
    #define hasTimeElapsed(tStart, tWait, tNow) ((((unsigned long) (tNow)) - ((unsigned long) (tStart))) > ((unsigned long) (tWait)))

#ifdef __cplusplus
}
#endif

#endif // _DNETCKAPI_H