/************************************************************************/
/*																		*/
/*	DNETcK.cpp      The Digilent IP static Network Classes For          */
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
/*	This the Static DNETcK Class implementation file     				*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	11/20/2011(KeithV): Created											*/
/*																		*/
/************************************************************************/
#include "./DNETcK.h"
#include "./utility/DNETcKAPI.h"

/***	STATIC CLASS VARIABLES
**
** These are the static variable that are defined in the DNETcK Class
** for reasons beyond my comprehension, this is how you allocate and intialize
** static member variables in a C++ Class
**
*/
const MAC DNETcK::zMAC = {0,0,0,0,0,0};
const IPv4 DNETcK::zIPv4 = {0,0,0,0};

unsigned long DNETcK::_msDefaultTimeout = DNETcK::msDefaultBlockTime;
bool DNETcK::_fBegun = false;  
bool DNETcK::_fIsInitialized = false;

MAC DNETcK::_mac = zMAC;
IPv4 DNETcK::_ip = zIPv4;
IPv4 DNETcK::_ipGateway = zIPv4;
IPv4 DNETcK::_subMask = zIPv4;
IPv4 DNETcK::_ipDns1 = zIPv4;
IPv4 DNETcK::_ipDns2 = zIPv4;

/***	bool DNETcK::begin(void)
**      bool DNETcK::begin(const IPv4& ip)
**      bool DNETcK::begin(const IPv4& ip, const IPv4& ipGateway)
**      bool DNETcK::begin(const IPv4& ip, const IPv4& ipGateway, const IPv4& subnetMask)
**      bool DNETcK::begin(const IPv4& ip, const IPv4& ipGateway, const IPv4& subnetMask, const IPv4& ipDns1)
**      bool DNETcK::begin(const IPv4& ip, const IPv4& ipGateway, const IPv4& subnetMask, const IPv4& ipDns1, const IPv4& ipDns2)
**
**	Synopsis:   
**      This initializes the Ethernet hardware and the IP Stack
**      For both UDP and TCP. This will return immediately after 
**      hardware initialization and isInitialized should be called
**      if it is important to know that the stack is up and running.
**      usually it is not needed to explicity call isInitialized as most
**      other calls will just fail if IP is not initialized. The most common
**      time consuming feature is obtaining your network information from DHCP
**      and this could take several seconds to complete. DNS will not work
**      until DHCP has completed.
**
**	Parameters:
**      mac         The hardware address to use. If not specified or 6 zero's the chipKIT assigned MAC is used.
**      ip          The static IP address to use, if not specifed or all zero's, DHCP is used to get your IP address.
**      ipGateway   The ip address of the gateway to use. If not specifed ip[0]:ip[1]:0.1 is used. This is ignored if DHCP is used. 
**      subnetMask  The subnet mask of the local network, if not specified 255.255.255.0 is used. This is ignored if DHCP is used. 
**      ipDns1      1 of 2 DNS servers to specify, if not specifed ipGateway is used. This is ignored if DHCP is used.
**      ipDns2      The 2nd of 2 DNS servers to specify, if not specified 0.0.0.0 is used. This is ignored if DHCP is used;
**  
**	Return Values:
**      None
**
**	Errors:
**      None
**
**  Notes:
**
**      If Begin has already be called, then this returns immediately ignoring all inputs.
**      If End is called, then call Begin will set new IP parameters and re-initialize the IP stack.
*/
bool DNETcK::begin()
{
    return(begin(zIPv4));
}
bool DNETcK::begin(const IPv4& ip)
{
    IPv4 ipGateway = ip;
    ipGateway.rgbIP[3] = 1;
    return(begin(ip, ipGateway));
}
bool DNETcK::begin(const IPv4& ip, const IPv4& ipGateway)
{
    return(begin(ip, ipGateway, (IPv4) {255,255,255,0}));
}
bool DNETcK::begin(const IPv4& ip, const IPv4& ipGateway, const IPv4& subnetMask)
{
    return(begin(ip, ipGateway, subnetMask, ipGateway));
}
bool DNETcK::begin(const IPv4& ip, const IPv4& ipGateway, const IPv4& subnetMask, const IPv4& ipDns1)
{
    return(begin(ip, ipGateway, subnetMask, ipDns1, zIPv4));
}
bool DNETcK::begin(const IPv4& ip, const IPv4& ipGateway, const IPv4& subnetMask, const IPv4& ipDns1, const IPv4& ipDns2)
{
    if(_fBegun)
    {
        return(false);
    }

    _fBegun = true;
    _ip = ip;
    _ipGateway = ipGateway;
    _subMask = subnetMask;
    _ipDns1 = ipDns1;
    _ipDns2 = ipDns2;
    EthernetBegin(ip.rgbIP, ipGateway.rgbIP, _subMask.rgbIP, _ipDns1.rgbIP, _ipDns2.rgbIP);
    return(true);
}

/***	void DNETcK::end(void)
**
**	Synopsis:   
**      Stops and releases the IP Stack and disables the Ethernet hardware.
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
**      This disables the Ethernet hardware. Begin can be called to re-initialize the hardware
**      and set new IP parameters.
*/
void DNETcK::end(void)
{
    _fIsInitialized = false;
    _fBegun = false;
    EthernetEnd();

    _mac = zMAC;
    _ip = zIPv4;
    _ipGateway = zIPv4;
    _subMask = zIPv4;
    _ipDns1 = zIPv4;
    _ipDns2 = zIPv4;
}

/***	bool DNETcK::isInitialized(void)
**      bool DNETcK::isInitialized(DNETcK::STATUS * pStatus)
**      bool DNETcK::isInitialized(unsigned long msBlockMax)
**      bool DNETcK::isInitialized(unsigned long msBlockMax, DNETcK::STATUS * pStatus)
**
**	Synopsis:   
**      Indicates if the Ethernet hardware and IP stack are initialized.
**
**	Parameters:
**      pStatus     returns initialization progress status.
**      msBlockMax  Will cause isInitialized to block waiting for either successful initalization or 
**                      for the time to elapse. If not specified DNETck::_msDefaultTimeout is used.
**                      if you want to return immediately than DNETck::msImmediate should be specified.
**
**	Return Values:
**      TRUE    if the stack is initialized
**      FALSE   if the stack is still initializing.
**
**	Errors:
**      None
**
**  Notes:
**
**      In general, this will return false until DHCP has finished supplying 
**      the network information. If DHCP, this will return true pretty much immediately.
*/
bool DNETcK::isInitialized(void)
{
    return(isInitialized(_msDefaultTimeout, NULL));
}
bool DNETcK::isInitialized(DNETcK::STATUS * pStatus)
{
    return(isInitialized(_msDefaultTimeout, pStatus));
}
bool DNETcK::isInitialized(unsigned long msBlockMax)
{
    return(isInitialized(msBlockMax, NULL));
}
bool DNETcK::isInitialized(unsigned long msBlockMax, DNETcK::STATUS * pStatus)
{
    if(!_fIsInitialized)
    {
        _fIsInitialized = EthernetisInitialized(msBlockMax, pStatus);

        // now populate our IP address from the MAL
        if(_fIsInitialized)
        {
            EthernetGetMACandIPs(_mac.rgbMAC, _ip.rgbIP, _ipGateway.rgbIP, _subMask.rgbIP, _ipDns1.rgbIP, _ipDns2.rgbIP);
        }

        return(_fIsInitialized);
    }

    // we may lose the MAC
    else
    {
        return(EthernetisInitialized(msBlockMax, pStatus));
    }
 }

/***	void DNETcK::periodicTasks(void)
**
**	Synopsis:   
**      Executes needed periodic stack tasks. This should be run once thru the loop() 
**      code to keep the stack alive.
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
**      This is a critical function and should be called often so incoming UDP/TCP 
**      messages are not missed.
**      
*/
void DNETcK::periodicTasks(void)
{

    // get our IPs set up.
    if(!_fIsInitialized)
    {
        isInitialized(msImmediate);
    }

    EthernetPeriodicTasks();
}

/***	unsigned long DNETcK::setDefaultBlockTime(unsigned long msDefaultBlockTimeT)
**
**	Synopsis:   
**      Sets the default timeout values used when timeout values are not explicitly supplied
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
**      This value is initialized to DNETcK::msDefaultBlockTime (15 seconds)
**      
*/
unsigned long DNETcK::setDefaultBlockTime(unsigned long msDefaultBlockTimeT)
{
    unsigned long msBefore = _msDefaultTimeout;

    _msDefaultTimeout = msDefaultBlockTimeT;

    return(msBefore);
}

/***	unsigned long DNETcK::secondsSinceEpoch(void)
**      unsigned long DNETcK::secondsSinceEpoch(DNETcK::STATUS * pStatus)
**
**	Synopsis:   
**
**      Returns the number of seconds since Epoch (Jan 1, 1970).
**
*
**	Parameters:
**      pStatus     A pointer to receive the status of the clock. If the clock is
**                  not initialized or DNS is not available the time returned is
**                  seconds since powerup (TimeSincePowerUp), and not epoch time (TimeSinceEpoch).
**              
**
**	Return Values:
**      The number since Jan 1 1970; or epoch
**
**	Errors:
**      Returns seconds since powerup if epoch time could not be obtained.
**          
*/
unsigned long DNETcK::secondsSinceEpoch(void)
{
    secondsSinceEpoch(NULL);
}
unsigned long DNETcK::secondsSinceEpoch(DNETcK::STATUS * pStatus)
{
    unsigned long secEpoch = SNTPGetUTCSeconds();

    if(pStatus != NULL)
    {
        // as of this writing it is after 2002, which is approximately 1000000000 seconds
        // if we get a number that big, we are getting epoch time.
        if(secEpoch > 1000000000)
        {
            *pStatus = DNETcK::TimeSinceEpoch;
        }
        else
        {
            *pStatus = DNETcK::TimeSincePowerUp;
        }
    }
    return(secEpoch);
}


/***	bool DNETcK::setMyMac(const MAC& mac)
**
**	Synopsis:   
**      Sets the MAC address to be used
**
**	Parameters:
**      mac    The mac address to use
**
**	Return Values:
**      true    if the MAC address was set
**      false   if the MAC address has has already been set
**
**
**  Notes:
**
**      For this to take effect, it must be called before DNETcK::begin() or DWIFIcK::connect()
**      that is, it must be called before MacInit().
**      This call can only be called once!
**      
*/
bool DNETcK::setMyMac(const MAC& mac)
{
    _mac = mac;
    return(EthernetSetMAC(mac.rgbMAC));
}

/***	bool DNETcK::getMyMac(MAC *pMAC)
**
**	Synopsis:   
**      Returns the currently active MAC address being used
**
**	Parameters:
**      pMAC    A pointer to a MAC variable to receive the MAC address
**
**	Return Values:
**      true    if the MAC address is valid
**      false   if the MAC address has not be set yet
**
**	Errors:
**      If returns false, the MAC address is not valid
**
**  Notes:
**
**      This value will not be accruate until the Ethernet and IP Stack is fully initialized 
**      
*/
bool DNETcK::getMyMac(MAC *pMAC)
{
    // get it set up if not already
    isInitialized(msImmediate);  
    if(_fIsInitialized)
    {
        *pMAC = _mac;
        return(true);
    }
    return(false);
}

/***	bool DNETcK::getMyIP(IPv4 *pIP)
**
**	Synopsis:   
**      Returns the currently active IP address being used
**
**	Parameters:
**      pIP    A pointer to an IPv4 variable to receive the IP address
**
**	Return Values:
**      true    if the IP address is valid
**      false   if the IP address has not be set yet
**
**	Errors:
**      If returns false, the IP address is not valid
**
**  Notes:
**
**      This value will not be accruate until the Ethernet and IP Stack is fully initialized 
**      
*/
bool DNETcK::getMyIP(IPv4 *pIP)
{
    // get it set up if not already
    isInitialized(msImmediate);  
    if(_fIsInitialized)
    {
        *pIP = _ip;
        return(true);
    }
    return(false);
}

/***	bool DNETcK::getGateway(IPv4 *pIPGateway)
**
**	Synopsis:   
**      Returns the currently active Gateway address being used
**
**	Parameters:
**      pIP    A pointer to an IPv4 variable to receive the IP address
**
**	Return Values:
**      true    if the IP address is valid
**      false   if the IP address has not be set yet
**
**	Errors:
**      If returns false, the IP address is not valid
**
**  Notes:
**
**      This value will not be accruate until the Ethernet and IP Stack is fully initialized 
**      
*/
bool DNETcK::getGateway(IPv4 *pIPGateway)
{
    // get it set up if not already
    isInitialized(msImmediate);  
    if(_fIsInitialized)
    {
        *pIPGateway = _ipGateway;
        return(true);
    }
    return(false);
}

/***	bool DNETcK::getSubnetMask(IPv4 *pSubnetMask)
**
**	Synopsis:   
**      Returns the currently active subnet Mask being used
**
**	Parameters:
**      pIP    A pointer to an IPv4 variable to receive the subnet mask
**
**	Return Values:
**      true    if the subnet mask is valid
**      false   if the subnet mask has not be set yet
**
**	Errors:
**      If returns false, the subnet mask is not valid
**
**  Notes:
**
**      This value will not be accruate until the Ethernet and IP Stack is fully initialized 
**      
*/
bool DNETcK::getSubnetMask(IPv4 *pSubnetMask)
{
    // get it set up if not already
    isInitialized(msImmediate);  
    if(_fIsInitialized)
    {
        *pSubnetMask = _subMask;
        return(true);
    }
    return(false);
}

/***	bool DNETcK::getDns1(IPv4 *pIPDns1)
**
**	Synopsis:   
**      Returns the currently active 1st DNS address being used
**
**	Parameters:
**      pIP    A pointer to an IPv4 variable to receive the IP address
**
**	Return Values:
**      true    if the IP address is valid
**      false   if the IP address has not be set yet
**
**	Errors:
**      If returns false, the IP address is not valid
**
**  Notes:
**
**      This value will not be accruate until the Ethernet and IP Stack is fully initialized 
**      
*/
bool DNETcK::getDns1(IPv4 *pIPDns1)
{
    // get it set up if not already
    isInitialized(msImmediate);  
    if(_fIsInitialized)
    {
        *pIPDns1 = _ipDns1;
        return(true);
    }
    return(false);
}

/***	bool DNETcK::getDns2(IPv4 *pIPDns2)
**
**	Synopsis:   
**      Returns the currently active 2nd DNS address being used
**
**	Parameters:
**      pIP    A pointer to an IPv4 variable to receive the IP address
**
**	Return Values:
**      true    if the IP address is valid
**      false   if the IP address has not be set yet
**
**	Errors:
**      If returns false, the IP address is not valid
**
**  Notes:
**
**      This value will not be accruate until the Ethernet and IP Stack is fully initialized 
**      
*/
bool DNETcK::getDns2(IPv4 *pIPDns2)
{
    // get it set up if not already
    isInitialized(msImmediate);  
    if(_fIsInitialized)
    {
        *pIPDns2 = _ipDns2;
        return(true);
    }
    return(false);
}

/***	void DNETcK::requestARPIpMacResolution(const IPv4& ip)
**
**	Synopsis:   
**      Broadcasts a local network ARP request.
**
**	Parameters:
**      ip    The IP address of the MAC requested
**
**	Return Values:
**      None
**
**	Errors:
**      None
**
**  Notes:
**
**      This is a blind request posted on the local area network
**      there is no guarantee that anyone will respond.
**      
*/
void DNETcK::requestARPIpMacResolution(const IPv4& ip)
{
    EthernetRequestARPIpMacResolution((byte *) &ip);
}

/***	bool DNETcK::isARPIpMacResolved(const IPv4& ip, MAC * pMAC)
**      bool DNETcK::isARPIpMacResolved(const IPv4& ip, MAC * pMAC, unsigned long msBlockMax)
**
**	Synopsis:   
**      Checks to see if anyone responded with a MAC address for the requested IP
**
**	Parameters:
**      ip    The IP address of the MAC requested
**      pMAC    A pointer to a MAC variable to receive the MAC address
**      msBlockMax The maximum number of msec to wait before returning. By default _msDefaultTimeout is used.
**
**	Return Values:
**      true if the MAC address was obtained
**      false if nothing came back yet
**
**	Errors:
**      If the underlying Ethernet hardware is not initialized, this will return false    
**
**  Notes:
**
**      This is a blind request posted on the local area network
**      there is no guarantee that anyone will respond.
**      
*/
bool DNETcK::isARPIpMacResolved(const IPv4& ip, MAC * pMAC)
{
    return(isARPIpMacResolved(ip, pMAC, _msDefaultTimeout));
}
bool DNETcK::isARPIpMacResolved(const IPv4& ip, MAC * pMAC, unsigned long msBlockMax)
{
    if(!isInitialized(msBlockMax))
    {
        return(false);
    }

    // there is a bug in the MAL that does not allow you to ARP yourself.
    // we will fix it here, but only if we have been initialized.
    if(ip.u32IP == _ip.u32IP)
    {
        *pMAC = _mac;
        return(true);
    }
    // otherwise we will ask the MAL to do an ARP.
    else
    {
        return(EthernetIsARPIpMacResolved((byte *) &ip, (byte *) pMAC, msBlockMax));
    }
}

/***	void DNETcK::terminateDNS(void)
**
**	Synopsis:   
**      Terminate any currently active DNS lookup.
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
**      This is brutal as it will terminate system lookups as well as any
**      you may have started. Only 1 DNS lookup may be in progress at a time
**      so unless you successfully started a DNS, this will blow away someone
**      else's lookup.
**      
*/
void DNETcK::terminateDNS(void)
{
    void EthernetDNSTerminate();
}

/***	bool DNETcK::isDNSResolved(const char * szHostName, IPv4 * pIP)
**      bool DNETcK::isDNSResolved(const char * szHostName, IPv4 * pIP, DNETcK::STATUS * pStatus)
**      bool DNETcK::isDNSResolved(const char * szHostName, IPv4 * pIP, unsigned long msBlockMax)
**      bool DNETcK::isDNSResolved(const char * szHostName, IPv4 * pIP, unsigned long msBlockMax, DNETcK::STATUS * pStatus)
**
**
**	Synopsis:   
**      Starts, and checs to see if a DNS lookup is finished.
**
**	Parameters:
**      szHostName  A pointer to a zero terminated string containing the hostname to lookup. This string must remain
**                      valid for the duration of the lookup.
**      pIP         A point to receive the resolved IP address representing the hostname.
**      pStatus     A point to receive the current status of the DNS lookup
**      msBlockMax  The maximum amount of time to wait attempting to lookup the DNS hostname before returning.
**                      Returning does not stop the DNS resolution, it only returns from the call. You can
**                      can call IsDNSResolved as many times as you want until the hostname is resolved, or 
**                      until the DNS engine returns an error.
**
**	Return Values:
**      true    if the DNS hostname was resolved to an IP and pIP is valid
**      false   if the DNS resolution is not finished, or an error occured (check pStatus).
**
**	Errors:
**      Check pStatus
**
**  Notes:
**
**      This attempt a DNS name lookup. If there are no DNS ip address specifed, this will fail
**      After a period of time, the underlying DNS will fail and you will get a DNSResolutionFailed
**      resolution failure. Just because msBlockMax has expired, does not mean that the resolution has failed.
**      
*/
bool DNETcK::isDNSResolved(const char * szHostName, IPv4 * pIP)
{
    return(isDNSResolved(szHostName, pIP, _msDefaultTimeout, NULL));
}
bool DNETcK::isDNSResolved(const char * szHostName, IPv4 * pIP, DNETcK::STATUS * pStatus)
{
    return(isDNSResolved(szHostName, pIP, _msDefaultTimeout, pStatus));
}
bool DNETcK::isDNSResolved(const char * szHostName, IPv4 * pIP, unsigned long msBlockMax)
{
   return(isDNSResolved(szHostName, pIP, msBlockMax, NULL));
}
bool DNETcK::isDNSResolved(const char * szHostName, IPv4 * pIP, unsigned long msBlockMax, DNETcK::STATUS * pStatus)
{

    // make sure the network is initialized
    if(!DNETcK::isInitialized(msBlockMax, pStatus))
    {
        return(false);
    }
    
    return(EthernetIsDNSResolved(szHostName, (byte *) pIP, msBlockMax, pStatus));
}

/***	bool DNETcK::isStatusAnError(DNETcK::STATUS status)
**
**	Synopsis:   
**      Indicates if the status returned is an error or just a status.
**      If this returns false, then you do not need to take action on the call other than to keep trying / waiting
**      If this returns true, the status indicates a hard error and corrective action is needed to proceed.
**
**	Parameters:
**      status The status code to determine if it is a hard error
**
**	Return Values:
**      true if the status is a hard error
**      false if the status is NOT a hard error, such as a progress status.
**
**	Errors:
**      None
**
**  Notes:
**      This is to aid in determining if a status returned is a hard error
**      and to abandon any continual attempt to wait for completion.
**      
*/
bool DNETcK::isStatusAnError(DNETcK::STATUS status)
{
    switch(status)
    {
    case DNETcK::Success:
    case DNETcK::DHCPNotBound:
    case DNETcK::WaitingMACLinkage:
    case DNETcK::TimeSincePowerUp:
    case DNETcK::DNSIsBusy:
    case DNETcK::DNSResolving:
    case DNETcK::DNSLookupSuccess:
    case DNETcK::DNSRecursiveExit:
    case DNETcK::Connected:
    case DNETcK::WaitingConnect:
    case DNETcK::WaitingReConnect:
    case DNETcK::WriteTimeout:
    case DNETcK::NoDataToRead:
    case DNETcK::Listening:
    case DNETcK::ARPResolving:
    case DNETcK::AcquiringSocket:
    case DNETcK::Finalizing:
    case DNETcK::EndPointResolved:
    case DNETcK::NoPendingClients:
    case DNETcK::ExceededMaxPendingAllowed:
    case DNETcK::WFStillScanning:
        return(false);
        break;

    default:
        return(true);            
    }
}


