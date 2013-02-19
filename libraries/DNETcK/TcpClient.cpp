/************************************************************************/
/*																		*/
/*	TcpClient.cpp   The Digilent TcpIP Client Class implementation for  */
/*                  the chipKIT product line. This includes the         */
/*					Arduino compatible chipKIT boards as well as  	    */
/*					the Digilent Cerebot cK boards.						*/
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
/*	This implements the TcpClient Class                 				*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	11/20/2011(KeithV): Created											*/
/*																		*/
/************************************************************************/
#include "DNETcK.h"
#include "DNETcKAPI.h"

/***	Prevent Copies
**
**  Notes:
**      
**      These are private methods that prevent
**      copying of the instance. They are
**      dummy functions and are never used.
**
*/
TcpClient&  TcpClient::operator=(TcpClient& tcpClient){return(tcpClient);}
TcpClient::TcpClient(TcpClient& tcpClient){};

/***	TcpClient Constructors
**
**  Notes:
**      
**      Just initializes an empty, not connected TcpClient instance
**
*/
TcpClient::TcpClient()
{
    construct();
}
void TcpClient::construct(void)
{
    _hTCP = INVALID_SOCKET;
    _classState = DNETcK::NotConnected;
    _fEndPointsSetUp = false;
}

/***	TcpClient Destructor
**
**  Notes:
**      
**      Terminates the instance, releasing all resources
**
*/
TcpClient::~TcpClient()
{
    close();
}

/***	void TcpClient::close(void)
**
**	Synopsis:   
**      Closes the socket and clears the TcpClient instance;
**      Returns the instance to a just constructed state releasing
**      all resources.
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
**      Returns the TcpClient instance to 
**      a state just as if the instance had just been
**      constructed. It also, Close the connection if
**      if it is still active.
**
*/
void TcpClient::close(void)
{
    if(_hTCP < INVALID_SOCKET)
    {
        // clean out the buffer
        TCPDiscard(_hTCP);

        // release the resources back to the MAL
        TCPClose(_hTCP);
    }

    // we are just closed
    _hTCP = INVALID_SOCKET;
    _classState = DNETcK::NotConnected;

    // say the enpoints are gone too
    _fEndPointsSetUp = false;
 }

/***	bool TcpClient::connect(const char *szRemoteHostName, unsigned short remotePort)
**      bool TcpClient::connect(const IPv4& remoteIP, unsigned short remotePort) 
**      bool TcpClient::connect(const IPEndPoint& remoteEP)
**      bool TcpClient::connect(const IPv4& remoteIP, unsigned short remotePort, DNETcK::STATUS * pStatus)
**      bool TcpClient::connect(const char *szRemoteHostName, unsigned short remotePort, DNETcK::STATUS * pStatus)
**      bool TcpClient::connect(const IPEndPoint& remoteEP, DNETcK::STATUS * pStatus)
**
**
**	Synopsis:   
**      Starts the connection process to connect a TcpClient to a remote host.
**      In itself, this does not "finish" the connection, to determine if the
**      connection has successfully been made, IsConnected should be called.
**
**	Parameters:
**      szRemoteHostName    A pointer to a zero terminated string holding the hostname of the remote endpoint
**                              to connect to. This will call the underlying DNS service to resolve the hostname
**                              to an IPv4 address. If no DNS servers were specified or DHCP was not used to
**                              intialize DNETck, then the DNS lookup will fail. The hostname string must be valid
**                              until IsConnected succeeds, or a hard failure status is returned.
**                              IsStatusAnError can be used to identify a hard failure status
**
**      remotePort          The port on the remote machine you want to connect too.
**  
**      remoteIP            The IPv4 address of the remote machine you want to connect to. This will
**                          call the underlying APR service to resolve the IP address to a MAC.
**
**      remoteEP            A IPEndPoint containing the remote IPv4 address and remote port to connect to. This will
**                          call the underlying APR service to resolve the IP address to a MAC.
**
**      pStatus             A pointer to receive the connect status. This is a status and may not be a failure. 
**                          Use IsStatusAnError to determine if this is a hard failure.
**
**
**	Return Values:
**      true                This will return true if a socket was allocated for the connection. It does not mean
**                          the connection was made, call IsConnected to see if the connection process is done.
**
**      false               A socket was not defined, false usually indicates a hard failure.
**
**	Errors:
**      None
**
**  Notes:
**
**      Call IsConnected to determine if the connection has been completed, or was lost.
**
*/
bool TcpClient::connect(const char *szRemoteHostName, unsigned short remotePort)
{
    return(connect(szRemoteHostName, remotePort, NULL));
}
bool TcpClient::connect(const IPv4& remoteIP, unsigned short remotePort) 
{
    return(connect(remoteIP, remotePort, NULL));
}
bool TcpClient::connect(const IPEndPoint& remoteEP)
{
    return(connect(remoteEP, NULL));
}
bool TcpClient::connect(const IPv4& remoteIP, unsigned short remotePort, DNETcK::STATUS * pStatus)
{
    IPEndPoint ipep = {remoteIP, remotePort};
    return(connect(ipep, pStatus));
}
bool TcpClient::connect(const char *szRemoteHostName, unsigned short remotePort, DNETcK::STATUS * pStatus)
{
    // make sure we haven't already started a connect process
    if(_classState != DNETcK::NotConnected)
    {
        if(pStatus != NULL) *pStatus = DNETcK::ConnectionAlreadyDefined;
        return(false);
    }

    // do not need to check to see if we are initialized because I can assign sockets before then

    // call the MAL to get a socket
    _hTCP = TcpClientConnectByName(szRemoteHostName, remotePort);

    if(_hTCP < INVALID_SOCKET)
    {
        _classState = DNETcK::WaitingConnect;
        if(pStatus != NULL) *pStatus = DNETcK::WaitingConnect;
        return(true);
    }
    else
    {
        if(pStatus != NULL) *pStatus = DNETcK::SocketError;
        return(false);
    }
}                         
bool TcpClient::connect(const IPEndPoint& remoteEP, DNETcK::STATUS * pStatus)
{
    // make sure we haven't already started a connect process
    if(_classState != DNETcK::NotConnected)
    {
        if(pStatus != NULL) *pStatus = DNETcK::ConnectionAlreadyDefined;
        return(false);
    }

    // call the MAL to get a handle.
    _hTCP =  TcpClientConnectByEndPoint(remoteEP.ip.rgbIP, remoteEP.port );

    if(_hTCP < INVALID_SOCKET)
    {
        _classState = DNETcK::WaitingConnect;
        if(pStatus != NULL) *pStatus = DNETcK::WaitingConnect;
        return(true);
    }
    else
    {
        if(pStatus != NULL) *pStatus = DNETcK::SocketError;
        return(false);
    }
}

/***	bool TcpClient::isConnected(void)
**      bool TcpClient::isConnected(DNETcK::STATUS * pStatus) 
**      bool TcpClient::isConnected(unsigned long msBlockMax)
**      bool TcpClient::isConnected(unsigned long msBlockMax, DNETcK::STATUS * pStatus)
**
**	Synopsis:   
**      This is the workhorse for the connection process. This will determine if you
**      currenlty have an active connection to the remote endpoint. This call should
**      be made immediately before assuming the connection is valid as the connection
**      can be dropped at anytime by the remote source or a network failure.
**
**	Parameters:
**      msBlockMax  If the connection process has not completed, this indicates the maximum time
**                      IsConnected should block waiting for the process to complete before returning
**                      control back to the caller. The connection process continues even if the timeout
**                      has occured, and IsConnected can be repeatedly called until either the connection
**                      is made or a hard error occurs.
**
**      pStatus     A pointer to receive the status of the connection process. This may not be a hard error
**                      and IsStatusAnError should be called to determine if the connection process has failed or is lost.
**
**	Return Values:
**      true        If the connection is active and ready
**      false       If the connection has not been made yet or was dropped or closed.
**
**	Errors:
**      Use IsStatusAnError to determine if the returned status is a hard error and the connection is 
**      permanently lost.
**
**  Notes:
**
**      This call will return true in 1 case when in fact the connection may be lost
**      If there are unread bytes in the socket, this call will return true until
**      you, read the bytes, empty the buffer, or close the TcpClient instance.
**
*/
bool TcpClient::isConnected(void)
{
    return(isConnected(DNETcK::_msDefaultTimeout, NULL));
}
bool TcpClient::isConnected(DNETcK::STATUS * pStatus)                    
{
    return(isConnected(DNETcK::_msDefaultTimeout, pStatus));
}
bool TcpClient::isConnected(unsigned long msBlockMax)
{
    return(isConnected(msBlockMax, NULL));
}
bool TcpClient::isConnected(unsigned long msBlockMax, DNETcK::STATUS * pStatus)
{
    unsigned long tStart = 0;
    bool fConnected = false;
    DNETcK::STATUS statusT = DNETcK::WaitingReConnect;

    // we want this to be PeriodicTasks and not StackTasks because we
    // want to run all of the applications when IsConnected is called
    // because this is a common call and we want to keep all things running
    EthernetPeriodicTasks();

    // see if we ever supplied info for a connection
    if(_hTCP >= INVALID_SOCKET)
    {
        if(pStatus != NULL) *pStatus = DNETcK::SocketError;
        return(false);
    }

    switch(_classState)
    {
        case DNETcK::Connected:
        case DNETcK::WaitingReConnect:
        case DNETcK::LostConnect:
            if(!DNETcK_IsMacConnected(&statusT))
            {
                _classState = statusT;
                if(pStatus != NULL) *pStatus = statusT;
                return(false);
            }
            else if(TCPIsConnected(_hTCP))
            {
                _classState = DNETcK::Connected;
                if(pStatus != NULL) *pStatus = DNETcK::Connected;
                return(true);
            }
            else
            {
               _classState = DNETcK::LostConnect;
               if(pStatus != NULL) *pStatus = DNETcK::LostConnect;
               return(false);
            }
            break;

        case DNETcK::NotConnected:
            _classState = DNETcK::WaitingConnect;

            // fall thru to get connected

        case DNETcK::WaitingConnect:

            // loop until connected, or the timeout
            tStart = millis();
            do {
                    // in order to know what connection state we have
                    // we must run the stack tasks
                    EthernetPeriodicTasks();

                    if(TCPIsConnected(_hTCP))
                    {                         
                        // now that we are connected, we can setup the endpoints
                        if(!_fEndPointsSetUp)
                        {
                            // we just Connected, there should be nothing in the flush buffers
                            // and we should have our remote endpoints all set up.
                            GetTcpSocketEndPoints(_hTCP, &_remoteEP.ip, &_remoteMAC, &_remoteEP.port, &_localEP.port);
                            DNETcK::getMyIP(&_localEP.ip);
                            _fEndPointsSetUp = true;
                        }

                       // we are connected
                        _classState = DNETcK::Connected;
                       if(pStatus != NULL) *pStatus = DNETcK::Connected;
                       return(true);
                    }

            } while(!hasTimeElapsed(tStart, msBlockMax, millis()));

            // clearly not connected, return our state
            if(pStatus != NULL) *pStatus = _classState;
            return(false);
            break;

        default:
            if(pStatus != NULL) *pStatus = _classState;
            return(false);
           break;
    }
}

/***	void TcpClient::discardReadBuffer(void)
**
**	Synopsis:   
**      Empties (flushes) the input (read) buffer.
**      All bytes are discarded.
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
**      This call is safe to make without checking the connection status.
**
*/
void TcpClient::discardReadBuffer(void)
{
    // run periodic tasks, and finish a connection
    isConnected(DNETcK::msImmediate);

    if(_hTCP < INVALID_SOCKET)
    {
        // clean out the buffer
        TCPDiscard(_hTCP);
    }
}

/***	int TcpClient::available(void)
**
**	Synopsis:   
**      Returns the number of bytes available for reading.
**
**	Parameters:
**      None
**
**	Return Values:
**      The actual number of bytes available in the socket ready for reading, 0 if none or the connection is lost.
**
**	Errors:
**      None
**
**  Notes:
**
**      This call is safe to make without checking the connection status.
**
*/
size_t TcpClient::available(void)
{
    // run periodic tasks, and finish a connection
    isConnected(DNETcK::msImmediate);

    if(_hTCP < INVALID_SOCKET)
    {
        return((int) TCPIsGetReady(_hTCP));
    }

    return(0);  
}

/***	int TcpClient::peekByte(void)
**      int TcpClient::peekByte(size_t index)
**
**	Synopsis:   
**      Gets the indicated byte from the socket buffer without removing the byte from the socket.
**
**	Parameters:
**      index   Zero based index of the byte withing the socket to return, should be less than what Available returns
**
**      pStatus A pointer to receive the status of the call, usually the connection status, or an index out of bounds error.
**
**	Return Values:
**      The actual byte, casted to an integer. -1 is returned if no byte was returned or an error occured.
**
**	Errors:
**      Index out of bounds, or a connection error.
**
**  Notes:
**
**      This call is safe to make without checking the connection status.
**
*/
int TcpClient::peekByte(void)
{
    return(peekByte(0));
}
int TcpClient::peekByte(size_t index)
{
     size_t cbReady = 0;

    if( (cbReady = available()) > 0 )
    {
        if(index >= cbReady)
        {
            return(-1);
        }

        return((int) TCPPeek(_hTCP, index));
    }

     return(-1);
}

/***	int TcpClient::peekStream(byte *rgbPeek, size_t cbPeekMax, size_t index)
**      int TcpClient::peekStream(byte *rgbPeek, size_t cbPeekMax)
**
**	Synopsis:   
**      Gets an array of bytes from the socket buffer without removing the bytes from the socket.
**
**	Parameters:
**      rgbPeek     A pointer to a buffer to receive the bytes.
**
**      cbPeekMax   The maximum size of rgbPeek
**
**      index       Zero based index of where in the socket buffer to start copying bytes from.
**
**      pStatus A pointer to receive the status of the call, usually the connection status, or an index out of bounds error.
**
**	Return Values:
**      The actual number of bytes coped. 0 is returned if no bytes were copied or an error occured.
**
**	Errors:
**      Index out of bounds, or a connection error.
**
**  Notes:
**
**      This call is safe to make without checking the connection status.
**
*/
size_t TcpClient::peekStream(byte *rgbPeek, size_t cbPeekMax)
{
    return(peekStream(rgbPeek, cbPeekMax, 0));
}
size_t TcpClient::peekStream(byte *rgbPeek, size_t cbPeekMax, size_t index)
{
    size_t cbReady = 0;

    if( (cbReady = available()) > 0 )
    {
        if(index >= cbReady)
        {
            return(0);
        }

        cbReady -= index;
        cbReady = cbReady < cbPeekMax ? cbReady : cbPeekMax;
        return(TCPPeekArray(_hTCP, rgbPeek, cbReady, index));
    }

    return(0);
}

/***	int TcpClient::readByte(void)
**
**	Synopsis:   
**      Reads the next available byte out of the socket buffer (removing the byte from the socket)
**
**	Parameters:
**
**      pStatus A pointer to receive the status of the call, usually the connection status.
**
**	Return Values:
**      The actual byte, casted to an integer. -1 is returned if no byte was returned or an error occured.
**
**	Errors:
**      Nothing to read, or a connection error.
**
**  Notes:
**
**      This call is safe to make without checking the connection status.
**
*/
int TcpClient::readByte(void)
{
    byte bData = 0;

    // this will run the stack tasks
    if(available() > 0  && TCPGet(_hTCP, &bData))
    {
        return((int) bData);
    }

    return(-1);
}

/***	int TcpClient::readStream(byte *rgbRead, size_t cbReadMax)
**
**	Synopsis:   
**      Reads an array of bytes from the socket buffer (and removes the bytes from the socket)
**
**	Parameters:
**      rgbRead     A pointer to a buffer to receive the bytes.
**
**      cbReadMax   The maximum size of rgbPeek
**
**      pStatus A pointer to receive the status of the call, usually the connection status.
**
**	Return Values:
**      The actual number of bytes read. 0 is returned if no bytes were read or an error occured.
**
**	Errors:
**      No bytes to read, or a connection error.
**
**  Notes:
**
**      This call is safe to make without checking the connection status.
**
*/
size_t TcpClient::readStream(byte *rgbRead, size_t cbReadMax)
{
    size_t cbReady = 0;

    if( (cbReady = available()) > 0 )
    {
        cbReady = cbReady < cbReadMax ? cbReady : cbReadMax;
         return(TCPGetArray(_hTCP, rgbRead, cbReady));
    }

    return(0);
}

/***	void TcpClient::writeStream(uint8_t bData)
**
**	Synopsis:   
**      Write a byte out on the TcpIP connection
**
**	Parameters:
**      bData     the byte to write out.
**
**	Return Values:
**      None
**
**	Errors:
**      None
**
**  Notes:
**
**      This call is safe to make without checking the connection status.
**
**      This is a private call and is provided so that TcpClient can inherit from the
**          the Print class, enabling print, println functionality for Arduino users.
**
**      This is a very costly call to make as the byte is flushed out on to the wire.
**      No caching of intermediate written bytes are done because it is impossible to know if
**      the caller only wants to write 1 byte, or has a sequence to write. If more than 1 byte
**      is to be put on the wire in one packet, Write should be used.
**
*/
void TcpClient::write(uint8_t bData){writeByte(bData);}
void TcpClient::write(const char *str){if(str != NULL) {writeStream((const byte *) str, strlen(str));}}
void TcpClient::write(const uint8_t *buffer, size_t size){writeStream(buffer, size);}

/***	int TcpClient::writeByte(byte bData)
**      int TcpClient::writeByte(byte bData, DNETcK::STATUS * pStatus)
**
**	Synopsis:   
**      Write a byte out on the TcpIP connection
**
**	Parameters:
**      bData     the byte to write out.
**
**      pStatus A pointer to receive the status of the call, usually the connection status.
**
**	Return Values:
**      1 if the byte was written. 0 is returned if no bytes were written or an error occured.
**
**	Errors:
**      connection status.
**
**  Notes:
**
**      This call is safe to make without checking the connection status.
**
**      This is a very costly call to make as the byte is flushed out on to the wire.
**      No caching of intermediate written bytes are done because it is impossible to know if
**      the caller only wants to write 1 byte, or has a sequence to write. If more than 1 byte
**      is to be put on the wire in one packet, Write should be used.
**
*/
int TcpClient::writeByte(byte bData)
{
    return((int) writeStream(&bData, 1, DNETcK::_msDefaultTimeout, NULL));
}                      
int TcpClient::writeByte(byte bData, DNETcK::STATUS * pStatus)                    
{
    return((int) writeStream(&bData, 1, DNETcK::_msDefaultTimeout, pStatus));
}

/***	int TcpClient::writeStream(const byte *rgbWrite, size_t cbWrite)
**      int TcpClient::writeStream(const byte *rgbWrite, size_t cbWrite, DNETcK::STATUS * pStatus)
**      int TcpClient::writeStream(const byte *rgbWrite, size_t cbWrite, unsigned long msBlockMax)
**      int TcpClient::writeStream(const byte *rgbWrite, size_t cbWrite, unsigned long msBlockMax, DNETcK::STATUS * pStatus)
**
**
**	Synopsis:   
**      Write an array of bytes out on the TcpIP connection
**
**	Parameters:
**      rgbWrite     A pointer to an array of bytes to write out.
**
**      cbWrite     The number of bytes to write out.
**
**      msBlockMax  The maximum amount of time that should be spent attempting to write out the buffer.
**                      Typically this parameter is not needed and the default value can be used.
**                      However, if writing a buffer larger than the internal TcpIP socket buffer,
**                      multiple writes will be used to write the whole buffer out and this could take
**                      some time depending on the speed of the network and the size of the array.
**                      If the timeout occurs before all of the bytes are written, the number of bytes
**                      actually written will be returned to the caller and Write will have to be
**                      called again to write out the un-written bytes.
**
**      pStatus     A pointer to receive the status of the call, usually the connection status.
**
**	Return Values:
**      The number of bytes actually written. 0 is returned if no bytes were written or an error occured.
**
**	Errors:
**      connection status or a write timeout.
**
**  Notes:
**
**      This call is safe to make without checking the connection status.
**
**      Each write will flush the array as a whole to the network.
**      Multiple write may occur if the array is larger than the socket buffer.
**
*/
size_t TcpClient::writeStream(const byte *rgbWrite, size_t cbWrite)                           
{
    return(writeStream(rgbWrite, cbWrite, DNETcK::_msDefaultTimeout, NULL));
}                      
size_t TcpClient::writeStream(const byte *rgbWrite, size_t cbWrite, DNETcK::STATUS * pStatus)
{
    return(writeStream(rgbWrite, cbWrite, DNETcK::_msDefaultTimeout, pStatus));
}                      
size_t TcpClient::writeStream(const byte *rgbWrite, size_t cbWrite, unsigned long msBlockMax)
{
    return(writeStream(rgbWrite, cbWrite, msBlockMax, NULL));
}                      
size_t TcpClient::writeStream(const byte *rgbWrite, size_t cbWrite, unsigned long msBlockMax, DNETcK::STATUS * pStatus)
{
    unsigned long tStart = 0;
    size_t cbWritten = 0;
    size_t cbReady = 0;

    tStart = millis();
    do
    {
        // make sure we are Connected
        // this will also run the stack
        if(!isConnected(DNETcK::msImmediate, pStatus))
        {
            return(cbWritten);
        }

        if((cbReady = TCPIsPutReady(_hTCP)) > 0)
        {
            cbReady = cbReady < cbWrite ? cbReady : cbWrite;
            cbReady = TCPPutArray(_hTCP, &rgbWrite[cbWritten], cbReady);
            cbWritten += cbReady;
            cbWrite -= cbReady;

            // flush out what we are trying to write
            TCPFlush(_hTCP);
        }
 
    } while(cbWrite > 0 && !hasTimeElapsed(tStart, msBlockMax, millis()));

    // put in the status
    if(cbWritten < cbWrite && pStatus != NULL)
    {
        *pStatus = DNETcK::WriteTimeout;
    }

    // make sure the last flush runs
    EthernetPeriodicTasks();

   return(cbWritten);
}

/***	bool TcpClient::getRemoteMAC(MAC *pRemoteMAC)
**
**	Synopsis:   
**      Gets the MAC address of the remote endpoint
**
**	Parameters:
**      pRemoteMAC  A pointer to a MAC to receive the remote MAC address
**
**	Return Values:
**      true    The remote MAC was returned.
**      false   The remote MAC is not known, usually because the connection was not set up yet.
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
bool TcpClient::getRemoteMAC(MAC *pRemoteMAC)
{
    if(pRemoteMAC != NULL) *pRemoteMAC = _remoteMAC;
    return(_fEndPointsSetUp);
}

/***	bool TcpClient::getRemoteEndPoint(IPEndPoint *pRemoteEP)
**
**	Synopsis:   
**      Gets the endpoint for the remote connection
**
**	Parameters:
**      pRemoteEP  A pointer to an IPEndPoint to receive the remote endpoint information.
**
**	Return Values:
**      true    The remote endpoint was returned.
**      false   The remote endpoint is not known, usually because the connection was not set up yet.
**
**	Errors:
**      None
**
**  Notes:
**
**      If false is returned, then *pRemoteEP will be garbage.
**
**      If true it will be the remote endpoint information even for machines not on the local area network.
**
*/
bool TcpClient::getRemoteEndPoint(IPEndPoint *pRemoteEP)
{
    if(pRemoteEP != NULL) *pRemoteEP = _remoteEP;
    return(_fEndPointsSetUp);
}

/***	bool TcpClient::getLocalEndPoint(IPEndPoint *pLocalEP)
**
**	Synopsis:   
**      Gets the endpoint for this connection. It will be the local machines IP address
**      and the port by this machine to talk to the remote machine.
**
**	Parameters:
**      pLocalEP  A pointer to an IPEndPoint to receive the local endpoint information.
**
**	Return Values:
**      true    The local endpoint was returned.
**      false   The local endpoint is not known, usually because the connection was not set up yet.
**
**	Errors:
**      None
**
**  Notes:
**
**      If false is returned, then *pLocalEP will be garbage.
**
*/
bool TcpClient::getLocalEndPoint(IPEndPoint *pLocalEP)
{
    if(pLocalEP != NULL) *pLocalEP = _localEP;
    return(_fEndPointsSetUp);
}


