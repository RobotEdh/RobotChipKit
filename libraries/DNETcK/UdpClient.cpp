/************************************************************************/
/*																		*/
/*	EthernetcK.cpp  The Digilent UdpClient Classes For the chipKIT       */
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
/*	This implements the UdpClient Class      				*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	11/20/2011(KeithV): Created											*/
/*																		*/
/************************************************************************/
#include "DNETcK.h"
#include "DNETcKAPI.h"


const IPv4 UdpClient::broadcastIP = {0xFF, 0xFF, 0xFF, 0xFF};

/***	Prevent Copies
**
**  Notes:
**      
**      These are private methods that prevent
**      copying of the instance. They are
**      dummy functions and are never used.
**
*/
UdpClient&  UdpClient::operator=(UdpClient& udpClient){return(udpClient);}
UdpClient::UdpClient(UdpClient& udpClient){};

/***	UdpClient Constructors
**
**  Notes:
**      
**      This is made private so the user must supply a datagram cache buffer
**
*/
UdpClient::UdpClient()
{
    initUdpClient();
}

/***	UdpClient Constructors
**
**  Notes:
**      
**      Just initializes a UdpClient instance and assigns the Udp datagram cache.
**
**      rgbReadBuffer       A pointer to a Udp datagram cache buffer; must be valid for the life of the UdpClient instance
**      cbReadBufferSize    The size of the Udp datagram cache buffer
**      msARPWait           How many msec to wait for before resending the ARP request
**      cARPRetriesDefault  How many time to attempt an ARP request before failing.
**                              Total time is: cARPRetriesDefault * msARPWait
**
*/
UdpClient::UdpClient(byte * rgbCache, size_t cbCache)
{
    construct(rgbCache, cbCache, msARPWaitDefault, cARPRetriesDefault);
}
UdpClient::UdpClient(byte * rgbCache, size_t cbCache, unsigned long msARPWait, unsigned long cARPRetries)
{
    construct(rgbCache, cbCache, msARPWait, cARPRetries);
}
void UdpClient::construct(byte * rgbCache, size_t cbCache, unsigned long msARPWait, unsigned long cARPRetries)
{
    initUdpClient();
    _msARPWait = msARPWait;
    _cARPRetries = cARPRetries;
    _cbCache = 0;
    _rgbCache = NULL;

    // if they are passing in a buffer for us to use, then lets use it.
    if(rgbCache != NULL && cbCache >= cbDatagramCacheMin)
    {
        _rgbCache = rgbCache;
        _cbCache = cbCache;
    }
}
void UdpClient::initUdpClient(void)
{
   _classState = DNETcK::EndPointNotSet;
   _hUDP = INVALID_UDP_SOCKET;
   _fEndPointsSetUp = false;
   _localPort = 0; 
   _remoteEP = (IPEndPoint) {{0,0,0,0}, 0};
   _remoteMAC = (MAC) {0,0,0,0,0,0};
   _szHostName = NULL;
   _msARPtStart = 0;
}

/***	UdpClient Destructor
**
**  Notes:
**      
**      Terminates the UdpClient and free all resources (socket).
**      The user supplied datagram cache is no longer needed.
**
*/
UdpClient::~UdpClient()
{
    close();
}

/***	void UdpClient::close(void)
**
**	Synopsis:   
**      Closes the socket and clears the UdpClient instance;
**      Returns the instance to a just constructed state releasing
**      all resources except the user supplied datagram cache buffer
**      which will be reused if SetEndPoint is called again.
**
**	Parameters:
**      None
**
**	Return Values:
**      None
**
**	Errors:
**      None
**
**  Notes:
**
**      Returns the UdpClient instance to 
**      a state just as if the instance had just been
**      constructed. The user supplied datagram cache
**      should still be kept valid.
**
*/
void UdpClient::close(void)
{
    if(_hUDP < INVALID_UDP_SOCKET)
    {
        // remove from the UDP Cache
        ExchangeCacheBuffer(_hUDP, NULL, 0);

        // release MAL socket
        UDPClose(_hUDP);
        _hUDP = INVALID_UDP_SOCKET;
    }

    // initialize this instance of the class
    initUdpClient();
}

/***	bool UdpClient::setEndPoint(const char *szRemoteHostName, unsigned short remotePort)
**      bool UdpClient::setEndPoint(const char *szRemoteHostName, unsigned short remotePort, unsigned short localPort)
**      bool UdpClient::setEndPoint(const IPEndPoint& remoteEP)
**      bool UdpClient::setEndPoint(const IPv4& remoteIP, unsigned short remotePort)
**      bool UdpClient::setEndPoint(const IPEndPoint& remoteEP, unsigned short localPort)
**      bool UdpClient::setEndPoint(const IPv4& remoteIP, unsigned short remotePort, unsigned short localPort)
**      bool setEndPoint(const IPEndPoint& remoteEP, MAC& remoteMAC);
**      bool setEndPoint(const IPv4& remoteIP, unsigned short remotePort, MAC& remoteMAC);
**      bool setEndPoint(const IPEndPoint& remoteEP, MAC& remoteMAC, unsigned short localPort);
**      bool setEndPoint(const IPv4& remoteIP, unsigned short remotePort, MAC& remoteMAC, unsigned short localPort);
**
**	Synopsis:   
**      Sets the remote endpoint data for a socket. When a datagram is written, it will be
**      sent to the specified endpoint.
**      In itself, this does not so anything, IsEndPointResolved must be called to determine
**      if the endpoint information has been resolved (DNS and ARP have finished).
**
**	Parameters:
**      szRemoteHostName    A pointer to a zero terminated string holding the hostname of the remote endpoint. 
**                              This will call the underlying DNS service to resolve the hostname
**                              to an IPv4 address. If no DNS servers were specified or DHCP was not used to
**                              intialize DNETck, then the DNS lookup will fail. The hostname string must be valid
**                              until IsEndPointResolved succeeds, or a hard failure status is returned.
**                              IsStatusAnError can be used to identify a hard failure status
**
**      remotePort          The port on the remote machine you want to send too.
**
**      remoteMAC           The remote MAC to send the datagram to on the local network, 
**                          if omitted, ARP is called to resolve the IP
**
**      localPort           The local port you want to receive datagram on, this is optional and if not specified
**                              the underlying socket engine will pick one.
**  
**      remoteIP            The IPv4 address of the remote machine. This will
**                          call the underlying APR service to resolve the IP address to a MAC.
**
**      remoteEP            A IPEndPoint containing the remote IPv4 address and remote port. This will
**                          call the underlying APR service to resolve the IP address to a MAC.
**
**
**	Return Values:
**      true                This will usually return true. It does not mean that DNS or ARP
**                          succeeded, call IsEndPointResolved to see if the endpoint was fully resolved.
**
**      false               Only returns false if the UdpClient has already set up an endpoint
**
**	Errors:
**      None
**
**  Notes:
**
**      Call IsEndPointResolved to determine if the endpoint was fully resolved to an IP and MAC.
**
*/
bool UdpClient::setEndPoint(const char *szRemoteHostName, unsigned short remotePort)
{
    return(setEndPoint(szRemoteHostName, remotePort, 0));
}
bool UdpClient::setEndPoint(const char *szRemoteHostName, unsigned short remotePort, unsigned short localPort)
{
    if(_classState != DNETcK::EndPointNotSet)
    {
        return(false);
    }

    _szHostName = szRemoteHostName;
    _localPort = localPort;
    _remoteEP.port = remotePort;
    _classState = DNETcK::DNSResolving;

    // get the ball rolling
    isEndPointResolved(DNETcK::msImmediate, NULL);
    return(true);
}
bool UdpClient::setEndPoint(const IPEndPoint& remoteEP)
{
    return(setEndPoint(remoteEP.ip, remoteEP.port, 0));
}
bool UdpClient::setEndPoint(const IPv4& remoteIP, unsigned short remotePort)
{
    return(setEndPoint(remoteIP, remotePort, 0));
}
bool UdpClient::setEndPoint(const IPEndPoint& remoteEP, unsigned short localPort)
{
    return(setEndPoint(remoteEP.ip, remoteEP.port, localPort));
}
bool UdpClient::setEndPoint(const IPv4& remoteIP, unsigned short remotePort, unsigned short localPort)
{
    if(_classState != DNETcK::EndPointNotSet)
    {
        return(false);
    }

    _classState = DNETcK::ARPResolving;

    _localPort = localPort;
    _remoteEP.ip = remoteIP;
    _remoteEP.port = remotePort;
    DNETcK::requestARPIpMacResolution(_remoteEP.ip);
    _msARPtStart = millis();

    // get the ball rolling
    isEndPointResolved(DNETcK::msImmediate, NULL);
    return(true);
}
bool UdpClient::setEndPoint(const IPEndPoint& remoteEP, MAC& remoteMAC)
{
    return(setEndPoint(remoteEP.ip, remoteEP.port, remoteMAC, 0));
}
bool UdpClient::setEndPoint(const IPEndPoint& remoteEP, MAC& remoteMAC, unsigned short localPort)
{
    return(setEndPoint(remoteEP.ip, remoteEP.port, remoteMAC, localPort));
}
bool UdpClient::setEndPoint(const IPv4& remoteIP, unsigned short remotePort, MAC& remoteMAC)
{
    return(setEndPoint(remoteIP, remotePort, remoteMAC, 0));
}
bool UdpClient::setEndPoint(const IPv4& remoteIP, unsigned short remotePort, MAC& remoteMAC, unsigned short localPort)
{

    if(_classState != DNETcK::EndPointNotSet)
    {
        return(false);
    }

    _classState = DNETcK::AcquiringSocket;

    _localPort = localPort;
    _remoteEP.ip = remoteIP;
    _remoteEP.port = remotePort;
    _remoteMAC = remoteMAC;
 
    // if went well IsEndPointResolved should complete successfully
    return(isEndPointResolved(DNETcK::msImmediate, NULL));
 }

/***	bool UdpClient::isEndPointResolved(void)
**      bool UdpClient::isEndPointResolved(DNETcK::STATUS * pStatus) 
**      bool UdpClient::isEndPointResolved(unsigned long msBlockMax)
**      bool UdpClient::isEndPointResolved(unsigned long msBlockMax, DNETcK::STATUS * pStatus)
**
**	Synopsis:   
**      This is the workhorse for the connection process. This will determine if you
**      currenlty have an active connection to the remote endpoint. This call should
**      be made immediately before assuming the connection is valid as the connection
**      can be dropped at anytime by the remote source or a network failure.
**
**	Parameters:
**      msBlockMax  If the endpoint resolution process has not completed, this indicates the maximum time
**                      IsEndPointResolved should block waiting for the process to complete before returning
**                      control back to the caller. The resolution process continues even if the timeout
**                      has occured, and IsEndPointResolved can be repeatedly called until either resolution
**                      is made or a hard error occurs.
**
**      pStatus     A pointer to receive the status of the resolution process. This may not be a hard error
**                      and IsStatusAnError should be called to determine if the resolution process has failed.
**
**	Return Values:
**      true        If the endpoint resolution completed
**      false       If the endpoint resollution has not completed or there is an error
**
**	Errors:
**      Use IsStatusAnError to determine if the returned status is a hard error and resoluton failed
**
**  Notes:
**
**  a timeout of _msDefaultTimeout is used if not timeout value is given.
*/
bool UdpClient::isEndPointResolved(void)
{
    return(isEndPointResolved(DNETcK::_msDefaultTimeout, NULL));
}
bool UdpClient::isEndPointResolved(DNETcK::STATUS * pStatus)                  
{
    return(isEndPointResolved(DNETcK::_msDefaultTimeout, pStatus));
}
bool UdpClient::isEndPointResolved(unsigned long msBlockMax)  
{
    return(isEndPointResolved(msBlockMax, NULL));
}
bool UdpClient::isEndPointResolved(unsigned long msBlockMax, DNETcK::STATUS * pStatus)
{
    unsigned long tStart = 0;
    DNETcK::STATUS statusT = DNETcK::None;

    // we want this to be PeriodicTasks and not StackTask because we
    // want to run all of the applications when IsDNETcK::EndPointResolved is called
    // because this is a common call and we want to keep all things running
   EthernetPeriodicTasks();

    // nothing to do if we have already resolved it.
    if(_fEndPointsSetUp)
    {
        if(pStatus != NULL) *pStatus = DNETcK::EndPointResolved;
        return(true);
    }

    // make sure the network is initialized
    if(!DNETcK::isInitialized(msBlockMax, pStatus))
    {
        return(false);
    }
    
    // make sure the UDP cache is big enough
    if(_cbCache < UdpClient::cbDatagramCacheMin)
    {
        if(pStatus != NULL) *pStatus = DNETcK::UDPCacheToSmall;
        return(false);
    }

    // initialize our status to our current state
    if(pStatus != NULL) *pStatus = _classState;

    // return our current state, except if we are in the process of resolving
    if(DNETcK::isStatusAnError(_classState))
    {
        return(false);
    }

    // continue with the resolve process
    tStart = millis();
    do
    {
        // run the stack
        EthernetPeriodicTasks();

        switch(_classState)
        {
            case DNETcK::DNSResolving:
                if(DNETcK::isDNSResolved(_szHostName, &_remoteEP.ip, DNETcK::msImmediate, &statusT))
                {
                    DNETcK::requestARPIpMacResolution(_remoteEP.ip);
                    _classState = DNETcK::ARPResolving;
                    _msARPtStart = millis();
                }

                // stay at the resolving state until an error occurs
                // if an error, then we fail.
                else if(DNETcK::isStatusAnError(statusT))
                {
                    _classState = DNETcK::DNSResolutionFailed;
                }
                break;

            case DNETcK::ARPResolving:

                // see if we resolved the MAC address
                if(DNETcK::isARPIpMacResolved(_remoteEP.ip, &_remoteMAC, DNETcK::msImmediate))
                {
                    _classState = DNETcK::AcquiringSocket;
                    _msARPtStart = 0;
                }

                // if the ARP timeout has occured; rmember, ARP really doesn't give us failues
                else if(hasTimeElapsed(_msARPtStart, _msARPWait, millis())) 
                {
                    // resend the ARP Request
                    if(_cARPRetries > 0)
                    {
                        DNETcK::requestARPIpMacResolution(_remoteEP.ip);
                        _msARPtStart = millis();
                        _cARPRetries--;
                    }
                    // otherwise we are done
                    else
                    {
                        _classState = DNETcK::ARPResolutionFailed;
                        _msARPtStart = 0;
                    }
                }
                break;

            case DNETcK::AcquiringSocket:

                // set up the socket
                _hUDP = UdpClientSetEndPoint(_remoteEP.ip.rgbIP, _remoteMAC.rgbMAC, _remoteEP.port, _localPort);

                if(_hUDP >= INVALID_UDP_SOCKET)
                {
                    _classState = DNETcK::SocketError;
                    if(pStatus != NULL) *pStatus = _classState;
                    break;
                }

                // fall right into finalize; we have finalize for the UdpServer to use

            case DNETcK::Finalizing:

                // assume the best that we will finish, unless something bad happens.
                _classState = DNETcK::EndPointResolved;

                // we just set up the socket, so all of the IP and port info should be correct
                // let's just update our code so we know we have what the MAL thinks.
                GetUdpSocketEndPoints(_hUDP, &_remoteEP.ip, &_remoteMAC, &_remoteEP.port, &_localPort);

                // see if a cache buffer came in.
                if(_rgbCache == NULL)
                {
                    close();
                    _classState = DNETcK::UDPCacheToSmall;
                }

                // add it to the underlying caching engine
                else
                {
                    ExchangeCacheBuffer(_hUDP, _rgbCache, _cbCache);
                }
                break;

            // if we got an error in the process, we are done. 
            // this is how we get out of the do-while on an error
            default:
                if(pStatus != NULL) *pStatus = _classState;
                return(false);
        }

    } while(_classState != DNETcK::EndPointResolved && !hasTimeElapsed(tStart, msBlockMax, millis()));

    // not sure how we got out of the loop
    // but assign our state and only return true if we succeeded.
    if(pStatus != NULL) *pStatus = _classState;
    if(_classState == DNETcK::EndPointResolved)
    {
        _fEndPointsSetUp = true;
        return(true);
    }

    return(false);
}

/***	void UdpClient::discardDatagram(void)
**
**	Synopsis:   
**      Throws out the next datagram (if any) in the
**      datagram cache.
**
**	Parameters:
**      None
**
**	Return Values:
**      None
**
**	Errors:
**      None
**
**  Notes:
**
**
*/
void UdpClient::discardDatagram(void)
{
    // proabaly do not want to call periodictasks or run the stack
    // kind of want to keep things static if someone is flushing
    // however, if Avaiable is called, periodicTasks will be called.
    UdpClientEmptyNextDataGram(_hUDP);
}

/***	int UdpClient::available(void)
**
**	Synopsis:   
**      Returns the number of bytes in the next available datagram
**
**	Parameters:
**      None
**
**	Return Values:
**      The actual number of bytes available in the datagram ready for reading, 0 if no datagrams are in the cache
**
**	Errors:
**      None
**
**  Notes:
**
**      If you read a partial datagram on a previous ReadDatagram, this call will return the
**      number of unread bytes in the remaining datagram. If other datagrams are in the cache, they
**      will not be visible until the previous datagram is read and removed from the cache.
**
*/
size_t UdpClient::available(void)
{
    // isDNETcK::EndPointResolved will call periodic tasks
    if(isEndPointResolved(DNETcK::msImmediate))
    {
        return((unsigned int) UdpClientAvailable(_hUDP));
    }
    else
    {
        return(0);
    }
}

/***	int UdpClient::PeekByte(void)
**      int UdpClient::peekByte(size_t index)
**
**	Synopsis:   
**      Gets the indicated byte from the datagram without removing the byte from the datagram.
**
**	Parameters:
**      index   Zero based index of the byte within the datagram to return, should be less than what Available returns
**
**
**	Return Values:
**      The actual byte, casted to an integer. -1 is returned if no byte was returned or an error occured.
**
**	Errors:
**
**  Notes:
**
*/
int UdpClient::peekByte(void)
{
    return(peekByte(0));
}
int UdpClient::peekByte(size_t index)
{
    byte rgb[1];

    if(peekDatagram(rgb, sizeof(rgb), index) == 1)
    {
        return((int) ((unsigned int) rgb[0]));
    }
    else
    {
        return(-1);
    }
}

/***	int UdpClient::peekDatagram(byte *rgbPeek, size_t cbPeekMax)
**      int UdpClient::peekDatagram(byte *rgbPeek, size_t cbPeekMax, size_t index)
**
**	Synopsis:   
**      Gets an array of bytes from the datagram without removing the bytes from the datagram.
**
**	Parameters:
**      rgbPeek     A pointer to a buffer to receive the bytes.
**
**      cbPeekMax   The maximum size of rgbPeek
**
**      index       Zero based index of where in the datagram to start copying bytes from.
**
**	Return Values:
**      The actual number of bytes coped. 0 is returned if no bytes were copied or an error occured.
**
**	Errors:
**      Index out of bounds, or no bytes available, this is specified by a return of 0.
**
**  Notes:
**
**
*/
size_t UdpClient::peekDatagram(byte *rgbPeek, size_t cbPeekMax)
{
    peekDatagram(rgbPeek, cbPeekMax, 0);
}
size_t UdpClient::peekDatagram(byte *rgbPeek, size_t cbPeekMax, size_t index)
{
    return((unsigned int) UdpClientPeek(_hUDP, rgbPeek, cbPeekMax, index));
}

/***	int UdpClient::readDatagram(byte *rgbRead, size_t cbReadMax)
**
**	Synopsis:   
**      Reads an array of bytes from the datagram (and removes the bytes from the datagram)
**
**	Parameters:
**      rgbRead     A pointer to a buffer to receive the bytes.
**
**      cbReadMax   The maximum size of rgbPeek
**
**	Return Values:
**      The actual number of bytes read. 0 is returned if no bytes were read.
**
**	Errors:
**      No bytes to read.
**
**  Notes:
**
*/
size_t UdpClient::readDatagram(byte *rgbRead, size_t cbReadMax)
{
    // read the bytes from the cache
    // you must specify an iIndex of 0! Otherwise the I can't remove the bytes
    unsigned short cbPeek = UdpClientPeek(_hUDP, rgbRead, cbReadMax, 0);

    // now remove the bytes from the cache
    if(cbPeek > 0)
    {
        UdpClientRemoveBytesFromDataGram(_hUDP, cbPeek);
    }
  
    return((unsigned int) cbPeek);
}

/***	int UdpClient::writeDatagram(const byte *rgbWrite, size_t cbWrite)
**
**
**	Synopsis:   
**      Writes the buffer out as a datagram and sends it to the instances remote endpoint
**
**	Parameters:
**      rgbWrite     A pointer to an array of bytes that composes the datagram
**
**      cbWrite     The number of bytes in the datagram.
**
**	Return Values:
**      The number of bytes actually written. 0 is returned if no bytes were written or an error occured.
**
**	Errors:
**
**  Notes:
**
**      Because Udp is a datagram protocol, this always transmits the buffer immediately
**      as a complete datagram.
**
*/
long int UdpClient::writeDatagram(const byte *rgbWrite, size_t cbWrite)
{
    int cbMax = 0;
    // isDNETcK::EndPointResolved will call periodic tasks
    if(isEndPointResolved(DNETcK::msImmediate))
    {
        cbMax = (int) ((unsigned int) UDPIsPutReady(_hUDP));

        if(cbMax >= cbWrite)
        {
            // write the datagram out and flush it
            cbMax = UDPPutArray(rgbWrite, cbWrite);
            UDPFlush();
            return(cbMax);
        }

        // our output buffer is not big enough
        else
        {
            return(-cbMax);
        }      
    }

    // endpoint not resolved
    else
    {
        return(0);
    }

    EthernetPeriodicTasks();
}

/***	bool UdpClient::getRemoteEndPoint(IPEndPoint *pRemoteEP)
**
**	Synopsis:   
**      Gets the remote endpoint as resolved
**
**	Parameters:
**      pRemoteEP  A pointer to an IPEndPoint to receive the remote endpoint information.
**
**	Return Values:
**      true    The remote endpoint was returned.
**      false   The remote endpoint is not known, usually because resolution failed
**
**	Errors:
**      None
**
**  Notes:
**
**      If false is returned, then *pRemoteEP will be garbage.
**
*/
bool UdpClient::getRemoteEndPoint(IPEndPoint *pRemoteEP)
{
    *pRemoteEP = _remoteEP;
    return(_fEndPointsSetUp);
}

/***	bool UdpClient::getLocalEndPoint(IPEndPoint *pLocalEP)
**
**	Synopsis:   
**      Gets the endpoint for this socket. It will be the local machines IP address
**      and the port that this machine will use to talk to the remote machine.
**
**	Parameters:
**      pLocalEP  A pointer to an IPEndPoint to receive the local endpoint information.
**
**	Return Values:
**      true    The local endpoint was returned.
**      false   The local endpoint is not known, usually because the endpoint was not set up yet.
**
**	Errors:
**      None
**
**  Notes:
**
**      If false is returned, then *pLocalEP will be garbage.
**
*/
bool UdpClient::getLocalEndPoint(IPEndPoint *pLocalEP)
{
    if(!_fEndPointsSetUp)
    {
        return(false);
    }

    // if Ethernet has not been initialized, this will return false
    pLocalEP->port = _localPort;
    return(DNETcK::getMyIP(&pLocalEP->ip));
}

/***	bool UdpClient::getRemoteMAC(MAC *pRemoteMAC)
**
**	Synopsis:   
**      Gets the MAC address of the remote endpoint
**
**	Parameters:
**      pRemoteMAC  A pointer to a MAC to receive the remote MAC address
**
**	Return Values:
**      true    The remote MAC was returned.
**      false   The remote MAC is not known, usually because resolution failed
**
**	Errors:
**      None
**
**  Notes:
**
**      If false is returned, then *pRemoteMAC will be garbage.
**
**      This is the MAC address as returned by ARP, it will be the actual MAC address
**      for any machine on the local area netowrk, but will typically be the MAC address
**      of your router if the remote connection is not local to your network.
**
*/
bool UdpClient::getRemoteMAC(MAC *pRemoteMAC)
{
    *pRemoteMAC = _remoteMAC;
    return(_fEndPointsSetUp);
}


