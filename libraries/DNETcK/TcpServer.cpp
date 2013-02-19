/************************************************************************/
/*																		*/
/*	TcpServer.cpp  The Digilent TcpServer Classes For the chipKIT       */
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
/*	This implements the TcpServer Class                     			*/
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
TcpServer&  TcpServer::operator=(TcpServer& tcpServer){return(tcpServer);}
TcpServer::TcpServer(TcpServer& tcpServer){};

/***	TcpServer Constructors
**
**  Notes:
**      
**      Initialize a new TcpServer Instance and specify
**      maximum number of sockets that it will use.
**      Only 1 socket is listening at a time on the port
**      however there may be many sockets pending and accept.
**      Once all sockets are consumed by unaccepted clients
**      TcpServer will stop listening on the port until
**      a client is accepted and a socket opens up.
**
*/
TcpServer::TcpServer()
{
    construct(_cMaxPendingDefault);
}
TcpServer::TcpServer(int cMaxPendingClients)
{
    construct(cMaxPendingClients);
}
void TcpServer::construct(int cMaxPendingClients)
{
    clear();
    _cPendingMax           = cMaxPendingClients;            

}

/***	void TcpServer::clear(void)
**
**	Synopsis:   
**      initializes the TcpServer to it intial state.
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
void TcpServer::clear(void)
{
    _fStarted               = false;
    _fListening             = false;
    _localPort              = 0;
    _cPending               = 0;

    // _cPendingMax;  Don't fill in, this is on the constructor

    // init the array
    for(int i = 0; i<_cMaxPendingAllowed; i++)
    {
        _rghTCP[i] = INVALID_SOCKET;
    }
}

/***	TcpServer Destructor
**
**  Notes:
**      
**    Shuts down all sockets and releases all resources  
**      
**
*/
TcpServer::~TcpServer()
{
    close();
}

/***	void TcpServer::close(void)
**
**	Synopsis:   
**      Stops Listening and closes all unaccepted sockets/connections
**      and clears everything back to it's originally constructed state.
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
**      Returns the TcpServer instance to 
**      a state just as if the instance had just been
**      constructed. It also, Close all connections
**      and releases all resources (sockets).
**
*/
void TcpServer::close(void)
{
    stopListening();

    for(int i = 0; i<_cMaxPendingAllowed; i++)
    {
        if(_rghTCP[i] < INVALID_SOCKET)
        {
            // clean out the buffer
            TCPDiscard(_rghTCP[i]);

            // release the resources back to the MAL
            TCPClose(_rghTCP[i]);

            // invalidate the socket
            _rghTCP[i] = INVALID_SOCKET;
        }
    }

    clear();
}

/***	bool TcpServer::startListening(unsigned short localPort)
**      bool TcpServer::startListening(unsigned short localPort, DNETcK::STATUS * pStatus)
**
**	Synopsis:   
**      Starts listening for connections on the specified port
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
bool TcpServer::startListening(unsigned short localPort)
{
    return(startListening(localPort, NULL));
}
bool TcpServer::startListening(unsigned short localPort, DNETcK::STATUS * pStatus)
{
    // make sure we haven't already started a connect process
    if(_fStarted)
    {
        if(pStatus != NULL) *pStatus = DNETcK::AlreadyListening;
        return(false);
    }

    // we started listening; or could be listening 
    _localPort  = localPort;
    _fStarted   = true;

    // see if we can listen on anything.
    if(_cPending >= _cPendingMax)
    {
        if(pStatus != NULL) *pStatus = DNETcK::ExceededMaxPendingAllowed;
        return(false);
    }

    // we know we have a socket slot to listen on
    resumeListening();

    // see if we are listening
    if(_rghTCP[_cPending] >=  INVALID_SOCKET)
    {
        _fStarted = false;
        if(pStatus != NULL) *pStatus = DNETcK::SocketError;
        return(false);
    }

    if(pStatus != NULL) *pStatus = DNETcK::Listening;
    return(true);
}

/***	bool TcpServer::isListening(void)
**      bool TcpServer::isListening(DNETcK::STATUS * pStatus)
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
bool TcpServer::isListening(void)
{
    return(isListening(NULL));
}
bool TcpServer::isListening(DNETcK::STATUS * pStatus)
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
    else if(_rghTCP[_cPending] >= INVALID_UDP_SOCKET)
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

/***	void TcpServer::stopListening(void)
**
**	Synopsis:   
**      This stops listening on the server port, but does not shutdown TcpServer
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
void TcpServer::stopListening(void)
{
    // DO NOT blow away pending clients.
    // that will be done on a close()
    // update the pending count so we have them all.
    availableClients();

    if(_cPending < _cPendingMax && _rghTCP[_cPending] < INVALID_SOCKET)
    {
        // clean out the buffer
        TCPDiscard(_rghTCP[_cPending]);

        // release the resources back to the MAL
        TCPClose(_rghTCP[_cPending]);

        // only invalidate if this is the listening socket
        _rghTCP[_cPending] = INVALID_SOCKET;
    }

    // no longer listening
    _fListening = false;
}

/***	void TcpServer::resumeListening(void)
**
**	Synopsis:   
**      Resumes listening on a TcpServer that did a StopListening
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
void TcpServer::resumeListening(void)
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

    // if we are not currently listening, so attempt to
    // start listening
    if(_rghTCP[_cPending] >= INVALID_SOCKET)
    {
        // do not need to check to see if Ethernet is initialized because I can assign sockets before then
        _rghTCP[_cPending] = TcpServerStartListening(_localPort);
    }
}

/***	int TcpServer::availableClients(void)
**
**	Synopsis:   
**      Checks to see how many pending clients are availabe to be accepted.
**
**	Parameters:
**      None
**
**	Return Values:
**      The number of waiting TcpClients to be accepted.
**
**	Errors:
**      None
**
**  Notes:
**
**      This is the workhorse of the TcpServer Class
**      It will update pending clients if a connection is detected
**      It will attempt to start listening if a socket comes avalialbe for listening
**      It will clean up disconnected clients
*/
int TcpServer::availableClients(void)
{
    int i = 0;

    EthernetPeriodicTasks();

    if(isListening() && TCPIsConnected(_rghTCP[_cPending]))
    {
        _cPending++;      
    }

    // now look for not Connected ones and remove them out of the list
    while(i < _cPending)
    {
        // see if we need to clear this one
        if((_rghTCP[i] >= INVALID_SOCKET) || (!TCPIsConnected(_rghTCP[i]) && (TCPIsGetReady(_rghTCP[i]) == 0)) )
        {
            int j = i;

            // shift everyone from this location down.
            // include everything possible (_cPendingMax), even things beyond _cPending
            // because I want to shift the listening one at _rghUDP[_cPending] as well
            for(; j < _cPendingMax-1; j++)
            {
                _rghTCP[j] = _rghTCP[j+1];
            }

            // make sure the last one in the list is made an invalid socket
            _rghTCP[_cPendingMax-1] = INVALID_SOCKET;

            // reduce the pending count
            _cPending--;
        }
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

/***	bool TcpServer::acceptClient(TcpClient * pTcpClient)
**      bool TcpServer::acceptClient(TcpClient * pTcpClient, int index) 
**      bool TcpServer::acceptClient(TcpClient * pTcpClient, DNETcK::STATUS * pStatus)
**      bool TcpServer::acceptClient(TcpClient * pTcpClient, int index, DNETcK::STATUS * pStatus)
**
**	Synopsis:   
**      Removes a pending TcpClient by accepting it from the server.
**
**	Parameters:
**      pTcpClient  A pointer to a newly constructed client TcpClient
**
**      index       This is the zero based index of the client you want to accept. index must be less
**                      than what AvailableClients.
**
**      pStatus     A pointer to receive the status of AcceptClient
**      
**	Return Values:
**      true if the TcpClient was returned
**      false if the TcpClient could not be created
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
*/
bool TcpServer::acceptClient(TcpClient * pTcpClient)
{
    return(acceptClient(pTcpClient, 0, NULL));
}
bool TcpServer::acceptClient(TcpClient * pTcpClient, int index)     
{
    return(acceptClient(pTcpClient, index, NULL));
}
bool TcpServer::acceptClient(TcpClient * pTcpClient, DNETcK::STATUS * pStatus)
{
    return(acceptClient(pTcpClient, 0, pStatus));
}
bool TcpServer::acceptClient(TcpClient * pTcpClient, int index, DNETcK::STATUS * pStatus)
{
    // careful not to run PendingClients or IsListening so that the index
    // doesn't move on us.
    if(pTcpClient == NULL)
    {
        if(pStatus != NULL) *pStatus = DNETcK::ClientPointerIsNULL;
        return(false);
    }
    else if(pTcpClient->_hTCP < INVALID_SOCKET)
    {
         if(pStatus != NULL) *pStatus = DNETcK::SocketAlreadyAssignedToClient;
        return(false);
    }
    else if(_cPending == 0)
    {
        if(pStatus != NULL) *pStatus = DNETcK::NoPendingClients;
        return(false);
    }
    else if(index >= _cPending)
    {
        if(pStatus != NULL) *pStatus = DNETcK::IndexOutOfBounds;
        return(false);
    } 
 
    // looks like we have a valid socket at our index
    // We are just making sure we have a properly initialized client
    pTcpClient->close();
    pTcpClient->construct();
 
    // okay, I think I can make a TcpClient.
    pTcpClient->_hTCP = _rghTCP[index];
    pTcpClient->_classState = DNETcK::WaitingConnect; // this will allow IsConnected to finish out the client
    pTcpClient->_fEndPointsSetUp = false;

    // this will readjust our list to the right size
    _rghTCP[index] = INVALID_SOCKET;
    availableClients();

    // Now fix up the remote endpoints and stuff
    // so this gets returned all intialized for the user
    // this will call StackTask(), so keep this at the end
    // of this routine.
    pTcpClient->isConnected(DNETcK::msImmediate, pStatus); 
    return(true);
}
   
/***	bool TcpServer::getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP, int index)
**      bool TcpServer::getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP, MAC * pRemoteMAC, int index)
**
**	Synopsis:   
**      Returns endpoint information about a TcpClient waiting to be accepted.
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
**      AcceptClient so that the servers pending list does not change due to another connection.
**
*/
bool TcpServer::getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP)
{
    return(getAvailableClientsRemoteEndPoint(pRemoteEP, NULL, 0));
}
bool TcpServer::getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP, int index)
{
    return(getAvailableClientsRemoteEndPoint(pRemoteEP, NULL, index));
}
bool TcpServer::getAvailableClientsRemoteEndPoint(IPEndPoint *pRemoteEP, MAC * pRemoteMAC, int index)
{
    IPEndPoint remoteEP;
    unsigned short localPort;
    MAC macRemote;

    if(index >= _cPending)
    {
        return(false);
    } 

    // careful not to run PendingClients or IsListening so index order does not change
    GetTcpSocketEndPoints(_rghTCP[index], &remoteEP.ip, &macRemote, &remoteEP.port, &localPort);

    if(pRemoteEP != 0)
    {
        *pRemoteEP = remoteEP;
    }

    if(pRemoteMAC != 0)
    {
        *pRemoteMAC = macRemote;
    }
}

/***	bool TcpServer::getListeningEndPoint(IPEndPoint *pListeningEP)
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
bool TcpServer::getListeningEndPoint(IPEndPoint *pListeningEP)
{
    // if Ethernet has not been initialized, this will return false
    pListeningEP->port = _localPort;
    return(DNETcK::getMyIP(&pListeningEP->ip));
}



