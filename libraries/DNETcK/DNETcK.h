/************************************************************************/
/*																		*/
/*	DNETck.h        The Digilent Internet Classes For the chipKIT       */
/*                  product line. This includes the Arduino compatible  */
/*					chipKIT boards as well as the Cerebot cK boards  	*/
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
/*	This the Static DNETck Class Header file             				*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	10/19/2011(KeithV): Created											*/
/*																		*/
/************************************************************************/
#ifndef _DNETCK_H
#define _DNETCK_H

#ifdef _DWIFICK_H
#error DNETcK.h must be included before DWIFIcK.h
#endif

#ifndef __cplusplus
#define byte BYTE
#define bool BOOL
#endif

#ifdef __cplusplus

#include "WProgram.h"	
#include "Print.h"

#define INVALID_SOCKET (0xFEu)
#define UNKNOWN_SOCKET (0xFFu)
#define INVALID_UDP_SOCKET (0xFFu)

typedef union 
{
    byte rgbIP[4];
    uint32_t u32IP;
} IPv4;

typedef struct
{
    byte rgbMAC[6];
} MAC;

typedef struct
{
    IPv4            ip;
    unsigned short  port;
} IPEndPoint;

// everything is static so this class does NOT have to be instantiated
// you call things directly as DNETcK::begin();
class DNETcK // : public DWIFIcK
{
private:

    static const unsigned long msDefaultBlockTime = 15000;      // 15 seconds
    static unsigned long _msDefaultTimeout;

    // this will init by the compiler by default to zero because it is a static
    // I would initialize except only const can be initialized; and I have no constructor on a static class
    static bool _fBegun;  
    static bool _fIsInitialized;

    static MAC _mac;
    static IPv4 _ip;
    static IPv4 _ipGateway;
    static IPv4 _subMask;
    static IPv4 _ipDns1;
    static IPv4 _ipDns2;

public:

#endif

    // we explicitly assign values so you can easily lookup 
    // what the error code is.
    typedef enum 
    {
        None                            = 0,
        Success                         = 1,
        UDPCacheToSmall                 = 2,

        // Initialization status
        NetworkNotInitialized           = 10,
        NetworkInitialized              = 11,
        DHCPNotBound                    = 12,

        // Epoch status
        TimeSincePowerUp                = 20,
        TimeSinceEpoch                  = 21,

        // DNS status
        DNSIsBusy                       = 30,
        DNSResolving                    = 31,
        DNSLookupSuccess                = 32,
        DNSUninitialized                = 33,
        DNSResolutionFailed             = 34,
        DNSHostNameIsNULL               = 35,
        DNSRecursiveExit                = 36,

        // TCP connect state machine states
        NotConnected                    = 40,
        WaitingConnect                  = 41,
        WaitingReConnect                = 42,
        Connected                       = 43,

        // other connection status
        LostConnect                     = 50,
        ConnectionAlreadyDefined        = 51,
        SocketError                     = 52,
        WaitingMACLinkage               = 53,
        LostMACLinkage                  = 54,

        // write status
        WriteTimeout                    = 60,

        // read status
        NoDataToRead                    = 70,

        // Listening status
        NeedToCallStartListening        = 80,
        NeedToResumeListening           = 81,
        AlreadyStarted                  = 82,
        AlreadyListening                = 83,
        Listening                       = 84,
        ExceededMaxPendingAllowed       = 85,
        MoreCurrentlyPendingThanAllowed = 86,
        ClientPointerIsNULL             = 87,
        SocketAlreadyAssignedToClient   = 88,
        NoPendingClients                = 89,
        IndexOutOfBounds                = 90,

        // UDP endpoint resolve state machine
        EndPointNotSet                  = 100,

        // DNSResolving                         
        ARPResolving                    = 110,
        AcquiringSocket                 = 111,
        Finalizing                      = 112,
        EndPointResolved                = 113,

        // DNSResolutionFailed
        ARPResolutionFailed             = 120,
        // SocketError
 
        // WiFi Stuff below here
        WFStillScanning                 = 130,
        WFUnableToGetConnectionID       = 131,
        WFInvalideConnectionID          = 132,
        WFAssertHit                     = 133,

    } STATUS;

#ifdef __cplusplus

    static const unsigned long msInfinite = 0xFFFFFFFFFFFFFFFF;
    static const unsigned long msImmediate = 0;
    static const MAC zMAC;
    static const IPv4 zIPv4;

    // Official UDP + TCP Port Ranges
    static const unsigned short iReservedPort = 0;
    static const unsigned short iWellKnownPorts = 1;
    static const unsigned short iRebootPort = 69;
    static const unsigned short iRegisteredPorts = 1024;
    static const unsigned short iEphemeralPorts = 49152;
    static const unsigned short iMaxPort = 0xFFFF;

    // Unofficial and --probably-- safe to use for personal use
    // these ports are within the range of the IANA controled ports. 
    // But for the most part these have not been registered  
    // as of 12/9/2011. Each range is 1000 ports. 
    // Use at your own risk as these can be registered at any time.
    // Be aware that there are also many unofficial ports withing the IANA range.
    static const unsigned short iPersonalPorts35 = 35000;
    static const unsigned short iPersonalPorts38 = 38000;
    static const unsigned short iPersonalPorts39 = 39000;
    static const unsigned short iPersonalPorts44 = 44000;
    static const unsigned short iPersonalPorts45 = 45000;
    static const unsigned short iPersonalPorts46 = 46000;

    // Here are some common well know ports.
    // This is not an exhaustive list by any means!
    static const unsigned short iFTPDataPort        = 20;
    static const unsigned short iFTPControlPort     = 21;
    static const unsigned short iTelnetPort         = 23;
    static const unsigned short iSMTPPort           = 25;
    static const unsigned short iDNSPPort           = 53;
    static const unsigned short iTFTPPort           = 69;
    static const unsigned short iGopherPort         = 70;
    static const unsigned short iHTTPPort           = 80;
    static const unsigned short iNTPPort            = 123;
  
    static bool begin(void);
    static bool begin(const IPv4& ip);
    static bool begin(const IPv4& ip, const IPv4& ipGateway);
    static bool begin(const IPv4& ip, const IPv4& ipGateway, const IPv4& subnetMask);
    static bool begin(const IPv4& ip, const IPv4& ipGateway, const IPv4& subnetMask, const IPv4& ipDns1);
    static bool begin(const IPv4& ip, const IPv4& ipGateway, const IPv4& subnetMask, const IPv4& ipDns1, const IPv4& ipDns2);
    
    static void end(void);
    
    static bool isInitialized(void); 
    static bool isInitialized(STATUS * pStatus); 
    static bool isInitialized(unsigned long msBlockMax);
    static bool isInitialized(unsigned long msBlockMax, STATUS * pStatus);
    
    static void periodicTasks(void);

    static unsigned long secondsSinceEpoch(void);  
    static unsigned long secondsSinceEpoch(STATUS * pStatus);  
    
    static bool setMyMac(const MAC& mac);

    static bool getMyMac(MAC *pMAC);
    static bool getMyIP(IPv4 *pIP);
    static bool getGateway(IPv4 *pIPGateway);
    static bool getSubnetMask(IPv4 *pSubnetMask);
    static bool getDns1(IPv4 *pIPDns1);
    static bool getDns2(IPv4 *pIPDns2);

    static void requestARPIpMacResolution(const IPv4& ip);
    static bool isARPIpMacResolved(const IPv4& ip, MAC * pMAC);
    static bool isARPIpMacResolved(const IPv4& ip, MAC * pMAC, unsigned long msBlockMax);

    static bool isDNSResolved(const char * szHostName, IPv4 * pIP);
    static bool isDNSResolved(const char * szHostName, IPv4 * pIP, unsigned long msBlockMax);
    static bool isDNSResolved(const char * szHostName, IPv4 * pIP, STATUS * pStatus);
    static bool isDNSResolved(const char * szHostName, IPv4 * pIP, unsigned long msBlockMax, STATUS * pStatus);
    static void terminateDNS(void);

    static unsigned long setDefaultBlockTime(unsigned long msDefaultBlockTime); 
    static bool isStatusAnError(STATUS status);

    friend class TcpClient;
    friend class UdpClient;
    friend class DWIFIcK;
};

class TcpClient : public Print {
private:

    // this is used in the connect state machine, it is not an error state.
    DNETcK::STATUS    _classState; 
 
    byte            _hTCP;
    bool            _fEndPointsSetUp;
    IPEndPoint      _localEP;
    IPEndPoint      _remoteEP;
    MAC             _remoteMAC;

    // to prevent copies
    TcpClient&  operator=(TcpClient& tcpClient);
    TcpClient(TcpClient& tcpClient);

    // private methods
    void construct(void);

    // This is implementing the virtual methods for Print
    // by making these private we are hiding write() from TcpClient
    // while not specifically needed, we do a "using" of Print::write
    // so that any other default implementations remain visible
    // in TcpClient however I have hidden all Print virtual methods
    // so TcpClient will not see any write() methods.
    // print() and println() are visible to TcpClient
    // also, if you pass TcpClient to a method taking Print
    // that method will see all of the write() methods off of Print
    using Print::write;
    void write(uint8_t bData);
    void write(const char *str);
    void write(const uint8_t *buffer, size_t size);

public:
    TcpClient();
    ~TcpClient();

    void close(void);

    bool connect(const char *szRemoteHostName, unsigned short remotePort);                         
    bool connect(const IPEndPoint& remoteEP);
    bool connect(const IPv4& remoteIP, unsigned short remotePort);  
    bool connect(const char *szRemoteHostName, unsigned short remotePort, DNETcK::STATUS * pStatus);                         
    bool connect(const IPEndPoint& remoteEP, DNETcK::STATUS * pStatus);
    bool connect(const IPv4& remoteIP, unsigned short remotePort, DNETcK::STATUS * pStatus);  

    bool isConnected(void);                    
    bool isConnected(DNETcK::STATUS * pStatus);                     
    bool isConnected(unsigned long msBlockMax);  
    bool isConnected(unsigned long msBlockMax, DNETcK::STATUS * pStatus);

    void discardReadBuffer(void);
    size_t available(void);

    int peekByte(void);
    int peekByte(size_t index);

    size_t peekStream(byte *rgbPeek, size_t cbPeekMax);
    size_t peekStream(byte *rgbPeek, size_t cbPeekMax, size_t index);
 
    int readByte(void);
    size_t readStream(byte *rgbRead, size_t cbReadMax);
 
    int writeByte(byte bData);                      
    int writeByte(byte bData, DNETcK::STATUS * pStatus);

    size_t writeStream(const byte *rgbWrite, size_t cbWrite);                            
    size_t writeStream(const byte *rgbWrite, size_t cbWrite, DNETcK::STATUS * pStatus);
    size_t writeStream(const byte *rgbWrite, size_t cbWrite, unsigned long msBlockMax);
    size_t writeStream(const byte *rgbWrite, size_t cbWrite, unsigned long msBlockMax, DNETcK::STATUS * pStatus);
 
    bool getRemoteEndPoint(IPEndPoint *pRemoteEP);
    bool getLocalEndPoint(IPEndPoint *pLocalEP);
    bool getRemoteMAC(MAC *pRemoteMAC);

    friend class TcpServer;
};

class TcpServer {
private:
    static const int       _cMaxPendingAllowed     = 10;
    static const int       _cMaxPendingDefault     = 3;

    bool            _fStarted;
    bool            _fListening;
    unsigned short  _localPort;
    int             _cPendingMax;
    int             _cPending;
 
    byte            _rghTCP[_cMaxPendingAllowed];
    
    // to prevent copies
    TcpServer&  operator=(TcpServer& tcpServer);
    TcpServer(TcpServer& tcpServer);

    void construct(int cMaxPendingClients);    
    void clear(void);

public:
    TcpServer();    
    TcpServer(int cMaxPendingClients);    
    ~TcpServer();

    bool startListening(unsigned short localPort);
    bool startListening(unsigned short localPort, DNETcK::STATUS * pStatus);

    bool isListening(void);
    bool isListening(DNETcK::STATUS * pStatus);

    void stopListening(void);
    void resumeListening(void);
    void close(void);

    int availableClients(void);

    // We can get errors, you passed me a NULL, or an opened tcpClient, or index out of range.
    bool acceptClient(TcpClient * pTcpClient);       
    bool acceptClient(TcpClient * pTcpClient, int index);         
    bool acceptClient(TcpClient * pTcpClient, DNETcK::STATUS * pStatus);       
    bool acceptClient(TcpClient * pTcpClient, int index, DNETcK::STATUS * pStatus);       
   
    bool getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP);
    bool getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP, int index);
    bool getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP, MAC * pRemoteMAC, int index);

    bool getListeningEndPoint(IPEndPoint *pListeningEP);
};


// it is intentional that this does NOT inherit from Print. 
class UdpClient {
private:

    static const unsigned long cARPRetriesDefault = 3;
    static const unsigned long msARPWaitDefault  = 1000;
 
    unsigned long _cARPRetries;
    unsigned long _msARPWait;
    unsigned long _msARPtStart;

    // this is used in the resolve state machine, it is not an error state.
    DNETcK::STATUS      _classState; 
 
    byte            _hUDP;
    bool            _fEndPointsSetUp;
    unsigned short  _localPort;
    IPEndPoint      _remoteEP;
    MAC             _remoteMAC;
    const char *    _szHostName;

    int             _cbCache;
    byte *          _rgbCache;

    // to prevent copies
    UdpClient&  operator=(UdpClient& udpClient);
    UdpClient(UdpClient& udpClient);

    void initUdpClient(void);
    void construct(byte * rgbReadBuffer, size_t cbReadBufferSize, unsigned long msARPWait, unsigned long cARPRetries);

    // Must supply a cache buffer
    UdpClient();
 
public:
    static const IPv4           broadcastIP;
    static const unsigned long  cbDatagramCacheMin = 32;

    UdpClient(byte * rgbCache, size_t cbCache);
    UdpClient(byte * rgbCache, size_t cbCache, unsigned long msARPWait, unsigned long cARPRetries);

    ~UdpClient();

    bool setEndPoint(const char *szRemoteHostName, unsigned short remotePort);
    bool setEndPoint(const IPv4& remoteIP, unsigned short remotePort);
    bool setEndPoint(const IPEndPoint& remoteEP);
    bool setEndPoint(const char *szRemoteHostName, unsigned short remotePort, unsigned short localPort);
    bool setEndPoint(const IPv4& remoteIP, unsigned short remotePort, unsigned short localPort);
    bool setEndPoint(const IPEndPoint& remoteEP, unsigned short localPort);
    bool setEndPoint(const IPEndPoint& remoteEP, MAC& remoteMAC);
    bool setEndPoint(const IPv4& remoteIP, unsigned short remotePort, MAC& remoteMAC);
    bool setEndPoint(const IPEndPoint& remoteEP, MAC& remoteMAC, unsigned short localPort);
    bool setEndPoint(const IPv4& remoteIP, unsigned short remotePort, MAC& remoteMAC, unsigned short localPort);

    bool isEndPointResolved(void);                    
    bool isEndPointResolved(DNETcK::STATUS * pStatus);                     
    bool isEndPointResolved(unsigned long msBlockMax);  
    bool isEndPointResolved(unsigned long msBlockMax, DNETcK::STATUS * pStatus);

    void close(void);

    void discardDatagram(void);

    size_t available(void);

    int peekByte(void);
    int peekByte(size_t index);

    size_t peekDatagram(byte *rgbPeek, size_t cbPeekMax);
    size_t peekDatagram(byte *rgbPeek, size_t cbPeekMax, size_t index);

    size_t readDatagram(byte *rgbRead, size_t cbReadMax);
    long int writeDatagram(const byte *rgbWrite, size_t cbWrite);
 
    bool getRemoteEndPoint(IPEndPoint *pRemoteEP);
    bool getLocalEndPoint(IPEndPoint *pLocalEP);
    bool getRemoteMAC(MAC *pRemoteMAC);

    friend class UdpServer;
};

class UdpServer {
private:

    static const int       _cMaxPendingAllowed     = 10;
    static const int       _cMaxPendingDefault     = 3;

    bool            _fStarted;
    bool            _fListening;
    unsigned short  _localPort;
    int             _cPendingMax;
    int             _cPending;

    byte *          _rgbCache;
    size_t          _cbCache;

    byte            _rghUDP[_cMaxPendingAllowed];   
    byte            _iBuff[_cMaxPendingAllowed];    

    void construct(int cMaxPendingClients, byte * rgbReadBuffer, size_t cbReadBufferSize);
    void clear(void);

    // to prevent copies
    UdpServer&  operator=(UdpServer& udpServer);
    UdpServer(UdpServer& udpServer);

    // Must supply a cache buffer
    UdpServer();

public:
    UdpServer(byte * rgbReadBuffer, size_t cbReadBufferSize, int cMaxPendingClients);
    ~UdpServer();

    bool startListening(unsigned short localPort);
    bool startListening(unsigned short localPort, DNETcK::STATUS * pStatus);

    bool isListening(void);
    bool isListening(DNETcK::STATUS * pStatus);

    void stopListening(void);
    void resumeListening(void);
 
    void close(void);

    int availableClients(void);

    // only error is we can't allocate memory, false means out of memory
    bool acceptClient(UdpClient * pUdpClient);       
    bool acceptClient(UdpClient * pUdpClient, int index); 
    bool acceptClient(UdpClient * pUdpClient, DNETcK::STATUS * pStatus);       
    bool acceptClient(UdpClient * pUdpClient, int index, DNETcK::STATUS * pStatus); 

    bool getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP);
    bool getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP, int index);
    bool getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP, MAC * pRemoteMAC, int index);

    bool getListeningEndPoint(IPEndPoint *pLocalEP);
};
#endif


#endif  // _DNETCK_H
