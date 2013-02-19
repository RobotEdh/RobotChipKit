/************************************************************************/
/*																		*/
/*	UdpServer.cpp  The Digilent UdpServer Classes For the chipKIT       */
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
/*	This the Static Ethernet Class implementation file     				*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	11/20/2011(KeithV): Created											*/
/*																		*/
/************************************************************************/
#include "DNETcK.h"
#include "DNETcKAPI.h"

#define GetBufferSize() (_cbCache / _cPendingMax)
#define GetBufferLocation(a) (_rgbCache + (_iBuff[a] * GetBufferSize()))

/***	Prevent Copies
**
**  Notes:
**      
**      These are private methods that prevent
**      copying of the instance. They are
**      dummy functions and are never used.
**
*/
UdpServer&  UdpServer::operator=(UdpServer& udpServer){return(udpServer);}
UdpServer::UdpServer(UdpServer& udpServer){};

/***	TcpServer Constructors
**
**  Notes:
**      
**      Initialize a new UdpServer Instance and specify
**      maximum number of sockets that it will use.
**      Only 1 socket is listening at a time on the port
**      however there may be many sockets pending to accept.
**      Once all sockets are consumed by unaccepted clients
**      UdpServer will stop listening on the port until
**      a client is accepted and a socket opens up.
**
**      Also, because UDP is a un-reliable protocol, most
**      stacks will toss the last UPD datagram if a new one comes
**      in before the old one is read. This implementation caches
**      datagrams as datagrams maintaining their packet integrity.
**      However they I need a cache space to store the unread datagrams.
**      The user must supply a cache space that the server can use
**      to cache unread datagrams until the client is accepted.
**      The rgbReadBuffer is the cache buffer, with a total size of 
**      cbReadBufferSize. The cache will be partitioned into 
**      cbReadBufferSize / cMaxPendingClients, dividing the total
**      cache space accross the maximum number of potential pending clients.
**
**      rgbReadBuffer   A pointer to a buffer to be used a the datagram cache
**                      for the server only. When a client is accepted, the datagrams
**                      are copied from the server datagram cache to the clients datagram cache.
**      
**      cbReadBufferSize    The total size of the datagram cace to be used by the server.    
**
**      cMaxPendingClients  The maximum number of clients that server wil allow. The datagram
**                          cache is shared equally accross all clients making each un-accepted
**                          client's datagram cache (cbReadBufferSize / cMaxPendingClients) bytes long.
**                          Integer math is used and is truncated down.
*/

// this constructor is made private so it can not be specified, a cache buffer is required.
UdpServer::UdpServer(void)
{
    construct(_cMaxPendingDefault, NULL, 0);
}
UdpServer::UdpServer(byte * rgbReadBuffer, size_t cbReadBufferSize, int cMaxPendingClients)
{
    construct(cMaxPendingClients, rgbReadBuffer, cbReadBufferSize);
}
void UdpServer::construct(int cMaxPendingClients, byte * rgbReadBuffer, size_t cbReadBufferSize)
{
    clear();

    _rgbCache      = NULL;
    _cbCache       = 0;
    _cPendingMax    = 0;

    if(rgbReadBuffer != NULL && cMaxPendingClients > 0 && (cbReadBufferSize / cMaxPendingClients) >= UdpClient::cbDatagramCacheMin)
    {
        _rgbCache      = rgbReadBuffer;
        _cbCache       = cbReadBufferSize;
        _cPendingMax    = cMaxPendingClients;
    }
}
/***	void UdpServer::clear(void)
**
**	Synopsis:   
**      initializes the UdpServer to it intial state.
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
void UdpServer::clear(void)
{
    _fStarted               = false;
    _fListening             = false;
    _localPort              = 0;
    _cPending               = 0;

    // Don't do, part of the constructor
    //_cPendingMax; 
    //_rgbCache;
    //_cbCache;

    // init the array
    for(int i = 0; i <_cMaxPendingAllowed; i++)
    {
        _rghUDP[i] = INVALID_UDP_SOCKET;
        _iBuff[i] = (byte) i;
    }
}

/***	UdpServer Destructor
**
**  Notes:
**      
**    Shuts down all sockets and releases all resources  
**      
**
*/
UdpServer::~UdpServer()
{
    close();
}

/***	void UdpServer::close(void)
**
**	Synopsis:   
**      Stops Listening and closes all unaccepted sockets
**      and clears everything back to it's originally constructed state.
**      The datagram cache buffers remain in use as this is specified on the constuctor
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
**      Returns the UdpServer instance to 
**      a state just as if the instance had just been
**      constructed. It also, Close all connections
**      and releases all resources (sockets).
**
*/
void UdpServer::close(void)
{
    stopListening();

    for(int i = 0; i < _cMaxPendingAllowed; i++)
    {
        if(_rghUDP[i] < INVALID_UDP_SOCKET)
        {
            // clean out the buffer
            UDPIsGetReady(_rghUDP[i]);
            UDPDiscard();
            UDPClose(_rghUDP[i]);
            ExchangeCacheBuffer(_rghUDP[i], NULL, 0);
            _rghUDP[i] = INVALID_UDP_SOCKET;
        }
    }

    clear();
}

/***	bool UdpServer::startListening(unsigned short localPort)
**      bool UdpServer::startListening(unsigned short localPort, DNETcK::STATUS * pStatus)
**
**	Synopsis:   
**      Starts listening for datagrams coming in on the specified port
**
**	Parameters:
**      localPort   The port to start listening on
**
**      pStatus     A pointer to the status of the server. 
**
**	Return Values:
**      true        If Listening started
**      false       This is an error and stated to listen failed.
**
**	Errors:
**      None
**
**  Notes:
**      Just because this returns true does not mean it is actually listening right now.
**      If there are no available sockets, it will not be listening yet this is not a 
**      a failure. Once sockets become available, it will automatically start listening.
**      To know if it is actually listening right now call IsListening
**
*/
bool UdpServer::startListening(unsigned short localPort)
{
    return(startListening(localPort, NULL));
}
bool UdpServer::startListening(unsigned short localPort, DNETcK::STATUS * pStatus)
{
    // make sure we haven't already started a connect process
    if(_fStarted)
    {
        if(pStatus != NULL) *pStatus = DNETcK::AlreadyStarted;
        return(false);
    }

    // now lets see if we have buffer space
    if(_rgbCache == NULL)
    {
        if(pStatus != NULL) *pStatus = DNETcK::UDPCacheToSmall;
        return(false);
    }

    // we started listening
    _localPort      = localPort;
    _fStarted = true;

    // see if there is a socket to listen on
    if(_cPending >= _cPendingMax)
    {
        if(pStatus != NULL) *pStatus = DNETcK::ExceededMaxPendingAllowed;
        return(false);
    }

    // we know we have a socket slot to listen on
    resumeListening();

    // see if we are listening
    if(_rghUDP[_cPending] >=  INVALID_UDP_SOCKET)
    {
        if(pStatus != NULL) *pStatus = DNETcK::SocketError;
        _fStarted = false;
        return(false);
    }

     return(true);
}

/***	bool UdpServer::isListening(void)
**      bool UdpServer::isListening(DNETcK::STATUS * pStatus)
**
**	Synopsis:   
**      Returns whether it is listening right now or not.
**
**	Parameters:
**      pStatus     A pointer to the status of the listening state. 
**
**	Return Values:
**      true        If it is currently listening on a socket
**      false       If it can't listen on a socket, get status for why
**
**	Errors:
**      None
**
**  Notes:
**      If this returns true, it is listening right now.
**      If it returns false, it is not listening right now.
**
*/
bool UdpServer::isListening(void)
{
    return(isListening(NULL));
}
bool UdpServer::isListening(DNETcK::STATUS * pStatus)
{
    if(!_fStarted)
    {
        if(pStatus != NULL) *pStatus = DNETcK::NeedToCallStartListening;
        return(false);
    }
    else if(_cPending >= _cPendingMax)
    {
        if(pStatus != NULL) *pStatus = DNETcK::ExceededMaxPendingAllowed;
        return(false);
    }
    else if(!_fListening)
    {
        if(pStatus != NULL) *pStatus = DNETcK::NeedToResumeListening;
        return(false);
    }
    else if(_rghUDP[_cPending] >= INVALID_UDP_SOCKET)
    {
        if(pStatus != NULL) *pStatus = DNETcK::SocketError;
        return(false);
    }
    else if(!DNETcK_IsMacConnected(pStatus))
    {
        return(false);
    }

    if(pStatus != NULL) *pStatus = DNETcK::Listening;
    return(true);
}

/***	void UdpServer::stopListening(void)
**
**	Synopsis:   
**      This stops listening on the server port, but does not shutdown UdpServer
**
**	Parameters:
**     None
**
**	Return Values:
**      None
**
**	Errors:
**      None
**
**  Notes:
**      To resume listening, just call ResumeListening
**      This is a soft stop listening in that only the server stops listening on the port
**      however the instance is still valid and you can continue to accept pending client.
**      and you can resume the listening.
**
*/
void UdpServer::stopListening(void)
{
   // DO NOT blow away pending clients.
   // that will be done on a close()
   // update the pending count so we have them all.
    availableClients();

   // blow away any listening socket
   if(_cPending < _cPendingMax && _rghUDP[_cPending] < INVALID_UDP_SOCKET)
    {
        UDPIsGetReady(_rghUDP[_cPending]);
        UDPDiscard();
        UDPClose(_rghUDP[_cPending]);
        ExchangeCacheBuffer(_rghUDP[_cPending], NULL, 0);
        _rghUDP[_cPending] = INVALID_UDP_SOCKET;
    }

    // no longer listening
    _fListening = false;
}

/***	void UdpServer::resumeListening(void)
**
**	Synopsis:   
**      Resumes listening on a UdpServer that did a StopListening
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
**      If StartListening was never called, this does nothing
**      If it is already listening, it does nothing
*/
void UdpServer::resumeListening(void)
{
    if(!_fStarted)
    {
        return;
    }

    // say we want to listen, may fail, but we will pick it up when we can.
    _fListening = true;

    // make sure we have room to to put our handle in
    if(_cPending >= _cPendingMax)
    {
        return;
    }

    // if we need to start listening on a socket
    if(_rghUDP[_cPending] >= INVALID_UDP_SOCKET)
    {
        // do not need to check to see if Ethernet is initialized because I can assign sockets before then
        // _rghUDP[_cPending] = UDPOpen(_localPort, NULL, 0);       // this is the old MLA
        _rghUDP[_cPending] = UDPOpenEx(NULL, 0, _localPort, 0);     // 2nd param is UDP_OPEN_SERVER = 0
    
        if(_rghUDP[_cPending] < INVALID_UDP_SOCKET)
        {
            // As for the iBuff, when we get data on the socket, the last entry 
            // will be where we put pending Client so just use this iBuff as the cache buffer
            ExchangeCacheBuffer(_rghUDP[_cPending], GetBufferLocation(_cPending), GetBufferSize());
        }
    }
}

/***	int UdpServer::availableClients(void)
**
**	Synopsis:   
**      Checks to see how many pending clients are availabe to be accepted.
**
**	Parameters:
**      None
**
**	Return Values:
**      The number of waiting UdpClients to be accepted.
**
**	Errors:
**      None
**
**  Notes:
**
**      This is the workhorse of the UdpServer Class
**      It will update pending clients if a datagram is detected
**      It will attempt to start listening if a socket comes avalialbe for listening
**      It will clean up disconnected clients
*/
int UdpServer::availableClients(void)
{
    int i = 0;

    EthernetPeriodicTasks();

    if(isListening() && UdpClientAvailable(_rghUDP[_cPending]) > 0)
    {
        // remember, we listen on the last hUDP in the buffer.
        _cPending++;
    }

    // The AcceptClient could have removed someone from the list
    // do this first, so if we resume we have a socket to listen
    while(i <_cPending)
    {
        // see if we need to clear this one
        if(_rghUDP[i] >= INVALID_UDP_SOCKET)
        {
            int j = i;
            byte iBuff = _iBuff[i];     // this buffer came available

            // shift everyone from this location down.
            // include everything possible (_cPendingMax), even things beyond _cPending
            // because I want to shift the listening one at _rghUDP[_cPending] as well
            for(; j < _cPendingMax-1; j++)
            {
                _rghUDP[j] = _rghUDP[j+1];
                _iBuff[j] = _iBuff[j+1];
            }
            
            // we know the very last slot is open, because we just removed one and shifted down
            // make sure we have an invalid socket number in there. 
            // also, since we shifted, the last 2 slots will have the same iBuff index
            // but we just opened up a new index that we just removed, move it to the last socket slot
            // we know this last socket slot is open, if we are not listening, then this will be picked
            // as our listening slot when we do the resume below          
            _rghUDP[_cPendingMax-1] = INVALID_UDP_SOCKET;
            _iBuff[_cPendingMax-1] = iBuff;

            // reduce the pending count as we just removed one
            _cPending--;
        }

        // if nothing was shifted, look at the next value; if we shifted
        // don't increment as we want to look at he shifted one.
        else
        {
            i++;
        }
    }

    // if we are supposed to be listening, then try to listening
    if(_fListening)
    {
         resumeListening();
    }

    return(_cPending);
}

/***	bool UdpServer::acceptClient(UdpClient * pUdpClient)
**      bool UdpServer::acceptClient(UdpClient * pUdpClient, int index) 
**      bool UdpServer::acceptClient(UdpClient * pUdpClient, DNETcK::STATUS * pStatus) 
**      bool UdpServer::acceptClient(UdpClient * pUdpClient, int index, DNETcK::STATUS * pStatus)
**
**	Synopsis:   
**      Removes a pending UdpClient by accepting it from the server.
**
**	Parameters:
**      pUdpClient  A pointer to a newly constructed client UdpClient; the datagram cache must have
**                  been specified when the UdpClient was instantiated.
**
**      index       This is the zero based index of the client you want to accept. index must be less
**                      than what AvailableClients.
**
**      pStatus     A pointer to receive the status of AcceptClient
**      
**	Return Values:
**      true if the UdpClient was returned
**      false if the UdpClient could not be created
**
**	Errors:
**      None
**
**  Notes:
**
**     This call should be made immediately after AvailableClients or GetAvailableClientsRemoteEndPoint.
**      Do not run PeriodicTasks between AvailableClients and this call because the client count could change.
**      In order to keep the indexes within the range of AvailableClients and consistent with what GetAvailableClientsRemoteEndPoint
**      returned, do not run AvailableClients or PeriodicTasks inbetween them. That is, do not run any stack task as that 
**      would change the AvailableClients count and order.
**  
**      When a UdpClient is accepted, the datagram cache is copied from the server datagram cache space to UdpClient's
**      datagram cache space. If the client's space is less than the server space, some datagrams may be discarded. However
**      any remaining datagrams will be full unchopped datagrams. It is possible in extreme cases where all datagrams were
**      discarded that your client will have an Available ready to read datagram byte count of zero.
*/
bool UdpServer::acceptClient(UdpClient * pUdpClient)
{
    return(acceptClient(pUdpClient, 0, NULL));
}
bool UdpServer::acceptClient(UdpClient * pUdpClient, int index)
{
    return(acceptClient(pUdpClient, index, NULL));
}
bool UdpServer::acceptClient(UdpClient * pUdpClient, DNETcK::STATUS * pStatus)      
{
    return(acceptClient(pUdpClient, 0, pStatus));
}
bool UdpServer::acceptClient(UdpClient * pUdpClient, int index, DNETcK::STATUS * pStatus) 
{
    int cb = 0;

    // do not call AvailableClients here because that could shift the indexes around
    // and then we would not be returning the index that someone wants.
    if(pUdpClient == NULL)
    {
        if(pStatus != NULL) *pStatus = DNETcK::ClientPointerIsNULL;
        return(false);
    }
    else if(pUdpClient->_hUDP < INVALID_UDP_SOCKET)
    {
         if(pStatus != NULL) *pStatus = DNETcK::SocketAlreadyAssignedToClient;
        return(false);
    }
    else if(_cbCache < UdpClient::cbDatagramCacheMin)
    {
        if(pStatus != NULL) *pStatus = DNETcK::UDPCacheToSmall;
        return(false);
    }
    else if(index >= _cPending)
    {
        if(pStatus != NULL) *pStatus = DNETcK::IndexOutOfBounds;
        return(false);
    }

    // looks like we have a valid socket at our index
    // We are just making sure we have a properly initialized client
    pUdpClient->close();
    pUdpClient->construct(pUdpClient->_rgbCache, pUdpClient->_cbCache, pUdpClient->_msARPWait, pUdpClient->_cARPRetries);

    // set the hUDP and get ready to complete making the UdpClient
    pUdpClient->_hUDP = _rghUDP[index];
    pUdpClient->_classState = DNETcK::Finalizing;

    // about the only thing that can happen in finalize is that I can get an OutOfMemory error
    if(!pUdpClient->isEndPointResolved(DNETcK::msImmediate, pStatus))
    {
        // we do not want to remove this from the pending list on error
        // maybe you can send me another UdpClient where you specify the 
        // buffer and it won't fail. There is really nothing wrong with the socket
        return(false);
    }

    // at this point we want to copy the cache over to the UdpClient cache
    // right now we are using the servers cache, and we want to move that
    // to the UdpClient cache. 
    // the ExchangeCacheBuffer will toss UPD datagrams that don't fit 
    // in the new buffer. It is possible for the buffer to be empty if all
    // datagrams are too big to fit in the exchanged buffer
    ExchangeCacheBuffer( pUdpClient->_hUDP, pUdpClient->_rgbCache,  pUdpClient->_cbCache);
 
    // We got the UdpClient, no remove this from our pending list
    // AvailableClient will do that if the socket is invalid
    _rghUDP[index] = INVALID_UDP_SOCKET;
    availableClients();
    return(true);
}

/***	bool UdpServer::getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP, int index)
**      bool UdpServer::getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP, MAC * pRemoteMAC, int index)
**      bool UdpServer::getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP)
**
**	Synopsis:   
**      Returns endpoint information about a UdpClient waiting to be accepted.
**
**	Parameters:
**      pRemoteEP  A pointer to an IPEndPoint to receive the remote endpoint information for the client
**                  waiting to be accepted. This may be NULL if you don't want the info
**
**      pRemoteMAC  A pointer to a MAC to receive the remote MAC address of the client waiting to be accepted.
**                      This will be the MAC address as known by ARP and will only have MAC address of machines
**                      on your local area network. If the IP is not local, the MAC will most probably be the MAC
**                      of your router. This may be NULL if you don't want the info
**
**     index        This is a zero based index and must be less than what AvailableClients returns. It
**                      selects the Client within the server list that you want remote endpoint information on.
**
**	Return Values:
**      true    The local endpoint was returned.
**      false   An error occured, most likely your index was out of range
**
**	Errors:
**      None
**
**  Notes:
**
**      This allows you to determine if you want to accept this endpoint before removing it from 
**      the server. It allows you to accept other clients first and out of order.
**
**      This call should be made immediately after AvailableClients and immediately before
**      AcceptClient so that the servers pending list does not change due to another datagram coming in
**      from another remote endpoint.
**
*/
bool UdpServer::getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP)
{
    return(getAvailableClientsRemoteEndPoint(pRemoteEP, NULL, 0));
}
bool UdpServer::getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP, int index)
{
    return(getAvailableClientsRemoteEndPoint(pRemoteEP, NULL, index));
}
bool UdpServer::getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP, MAC * pRemoteMAC, int index)
{
    IPEndPoint remoteEP;
    unsigned short localPort;
    MAC macRemote;

    if(index >= _cPending)
    {
        return(false);
    } 

    // careful not to run PendingClients or IsListening so index order does not change
    GetUdpSocketEndPoints(_rghUDP[index], &remoteEP.ip, &macRemote, &remoteEP.port, &localPort);

    if(pRemoteEP != 0)
    {
        *pRemoteEP = remoteEP;
    }

    if(pRemoteMAC != 0)
    {
        *pRemoteMAC = macRemote;
    }
}

/***	bool UdpServer::getListeningEndPoint(IPEndPoint *pLocalEP)
**
**	Synopsis:   
**      Gets the endpoint that the server is listening on.
**      This is both the IP and port the server is on.
**
**	Parameters:
**      pLocalEP  A pointer to an IPEndPoint to receive the local endpoint information.
**
**	Return Values:
**      true    The local endpoint was returned.
**      false   The local endpoint is not known, usually because you did not start listening
**
**	Errors:
**      None
**
**  Notes:
**
**      If false is returned, then *pLocalEP will be garbage.
**
*/
bool UdpServer::getListeningEndPoint(IPEndPoint *pLocalEP)
{
    // if Ethernet has not been initialized, this will return false
    pLocalEP->port = _localPort;
    return(DNETcK::getMyIP(&pLocalEP->ip));
}
