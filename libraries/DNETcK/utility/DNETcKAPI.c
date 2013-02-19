/************************************************************************/
/*																		*/
/*	DNETcKAPI.c	--  DNETcK interface APIs to implement the              */
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

#define THIS_IS_STACK_APPLICATION

// Include all headers for any enabled TCPIP Stack functions
#include "TCPIP Stack/TCPIP.h"

// not needed, but makes VS intellisense more reasonable
#undef __cplusplus

// Include functions specific to this stack application
#include "../DNETcK.h"
#include "./DNETcKAPI.h"

#if defined(DWIFIcK_WiFi_Network)
    #include "../../DWIFIcK/DWIFIcK.h"
    #include "../../DWIFIcK/utility/DWIFIcKAPI.h"
#endif

extern unsigned int MAX_TCP_SOCKETS;

// Private helper functions.
// These may or may not be present in all applications.
static void InitAppConfig(void);
static void InitializeBoard(void);
static void UpdateUDPCache(void);

// a rounded calculation of ticks per milsecond.
#define TICKS_PER_MILSECOND ((TICKS_PER_SECOND + 500) / 1000)

// Used for Wi-Fi assertions
#define WF_MODULE_NUMBER   WF_MODULE_MAIN_DEMO

// Declare AppConfig structure and some other supporting stack variables
APP_CONFIG AppConfig;

/****************************************************************************
// ChipKIT Client APIs
// Here is where the chipKIT static variables and typedefs are implemented
//***************************************************************************/

typedef struct _UDPCacheEntry
{
    byte *  rgbBuffer;
    WORD    cbBuffer;
    WORD    iStart;
    WORD    cbInUse;                // total including headers
} UDPCacheEntry;

#define NormalizeIndex(a, b) ((a) % (b))
#define DataGramSize(a, b, c) (((WORD) a[NormalizeIndex(b, c)]) + (((WORD) a[NormalizeIndex(b+1, c)]) << 8))
#define Min(a, b) ((a) < (b) ? (a) : (b))
#define Max(a, b) ((a) > (b) ? (a) : (b))

static UDPCacheEntry UDPCache[MAX_UDP_SOCKETS];
static const char * szDNSNameResolving = NULL;
static STATUS statusDNS = DNSUninitialized;
static IP_ADDR DNSLastResolvedHostIP;

static bool fIsEthernetEngineStopped = TRUE;
static bool fDatagramPartiallyRead = FALSE;
static bool fMacIsSet = FALSE;
static bool fMACInitialized = FALSE;

/*
	void _general_exception_handler(unsigned cause, unsigned status)
	{
		Nop();
		Nop();
	}
*/
/****************************************************************************
Function:
  static void InitializeBoard(void)

Description:
  This routine initializes the hardware.  It is a generic initialization
  routine for many of the Microchip development boards, using definitions
  in HardwareProfile.h to determine specific initialization.

Precondition:
  None

Parameters:
  None - None

Returns:
  None

Remarks:
  None
***************************************************************************/
static void InitializeBoard(void)
{	
    DNETcKInitNetworkHardware();
}

/****************************************************************************
Function:
 bool DNETcK_IsMacConnected(STATUS * pStatus);

Description:
    Determines if the underlying mac is connected, like a WiFi MAC

Precondition:
  Mac is initialized

Parameters:
    pStatus - A pointer to a status to return of the status of the MAC

Returns:
  True if the MAC is connected, false otherwise

Remarks:
  None
***************************************************************************/
bool DNETcK_IsMacConnected(STATUS * pStatus)
{
#if defined(DWIFIcK_WiFi_Network)
    return(WFcK_IsConnected(0, pStatus));
#else

    if(MACIsLinked())
    {
        if(pStatus != NULL) *pStatus = Connected;
        fMACInitialized = TRUE;
        return(TRUE);
    }
    else if(fMACInitialized)
    {
        if(pStatus != NULL) *pStatus = LostMACLinkage;
        return(FALSE);
    }
    else
    {
        if(pStatus != NULL) *pStatus = WaitingMACLinkage;
        return(FALSE);
    }
#endif
}

/***	EthernetSetMAC(const byte *rgbMac)
**
**	Synopsis:   
**      Sets the MAC address to be used
**
**	Parameters:
**      rgbMac    A pointer to the mac address to use
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
// MAC Address Serialization using a MPLAB PM3 Programmer and 
// Serialized Quick Turn Programming (SQTP). 
// The advantage of using SQTP for programming the MAC Address is it
// allows you to auto-increment the MAC address without recompiling 
// the code for each unit.  To use SQTP, the MAC address must be fixed
// at a specific location in program memory.  Uncomment these two pragmas
// that locate the MAC address at 0x1FFF0.  Syntax below is for MPLAB C 
// Compiler for PIC18 MCUs. Syntax will vary for other compilers.
//#pragma romdata MACROM=0x1FFF0
//static ROM BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};
static BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};
//#pragma romdata

bool EthernetSetMAC(const byte *rgbMac)
{
    if(fMacIsSet)
    {
        return(FALSE);  // already set
    }

    // copy the MAC address into our default space
    memcpy((void*)SerializedMACAddress, (void*)rgbMac, sizeof(SerializedMACAddress));
    fMacIsSet = TRUE;
    return(TRUE);
}

/*********************************************************************
 * Function:        void InitAppConfig(void)
 *
 * PreCondition:    MPFSInit() is already called.
 *
 * Input:           None
 *
 * Output:          Write/Read non-volatile config variables.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            This should only be called once!
 ********************************************************************/
static void InitAppConfig(void)
{	
	while(1)
	{
		// Start out zeroing all AppConfig bytes to ensure all fields are 
		// deterministic for checksum generation
		memset((void*)&AppConfig, 0x00, sizeof(AppConfig));
		
        // not sure I actually needs these, but I do them to be safe.
         //AppConfig.Flags.bIsDHCPEnabled = FALSE;
		AppConfig.Flags.bIsDHCPEnabled = TRUE;  // I think I DO need this, Can't find where it is initialized.
 		AppConfig.Flags.bInConfigMode = TRUE;   // don't think I need this, done in StackInit

        // this sets the MAC address and should only be called once!
		memcpy((void*)&AppConfig.MyMACAddr, (void*)SerializedMACAddress, sizeof(AppConfig.MyMACAddr));
        fMacIsSet = TRUE;   // this is the final assignment of the MAC, it is a done deal!

        AppConfig.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
		AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
		AppConfig.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | MY_DEFAULT_MASK_BYTE4<<24ul;
		AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
		AppConfig.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2<<8ul | MY_DEFAULT_GATE_BYTE3<<16ul | MY_DEFAULT_GATE_BYTE4<<24ul;
		AppConfig.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2<<8ul  | MY_DEFAULT_PRIMARY_DNS_BYTE3<<16ul  | MY_DEFAULT_PRIMARY_DNS_BYTE4<<24ul;
		AppConfig.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2<<8ul  | MY_DEFAULT_SECONDARY_DNS_BYTE3<<16ul  | MY_DEFAULT_SECONDARY_DNS_BYTE4<<24ul;
	
		// Load the default NetBIOS Host Name
		memcpypgm2ram(AppConfig.NetBIOSName, (ROM void*)MY_DEFAULT_HOST_NAME, 16);
		FormatNetBIOSName(AppConfig.NetBIOSName);
	
		#if defined(DWIFIcK_WiFi_Network)
            InitWiFiAppConfig();
		#endif

		break;
	}
}

/****************************************************************************
  Function:
    void DNETcK_Init(void)

  Description:
    Initialized the WiFi engine

  Precondition:
 
  Parameters:
    NONE

  Returns:
    NONE

  Remarks:  
    We only do this once, even if we have ended our Network connection
    By not allowing this to run again, we keep our in memory connection profiles
    AND we actually keep the IP stack running.
  ***************************************************************************/
void DNETcK_Init(void)
{
    static bool fIsInit = FALSE;

    // clear my UDP cache and DNS state so PeriodicTasks will run correctly
    memset(UDPCache, 0, sizeof(UDPCache));

    // make sure we only call this once!
    if(fIsInit)
    {
        return;
    }
    fIsInit = TRUE;

    szDNSNameResolving = NULL;
    DNSLastResolvedHostIP.Val = 0;
    statusDNS = DNSUninitialized;
    fDatagramPartiallyRead = FALSE;
    fMACInitialized = FALSE;

    // Init the static memory in the stack subsytems
    // this is in addition to StackInit()
    //InitNBNSStaticMemory();
    //InitSNTPStaticMemory();
    //InitDNSStaticMemory();
    //InitRebootStaticMemory();

	// Initialize application specific hardware
	InitializeBoard();

	// Initialize stack-related hardware components that may be 
	// required by the UART configuration routines
    TickInit();

	// Initialize Stack and application related NV variables into AppConfig.
	InitAppConfig();

    // Initialize core stack layers (MAC, ARP, TCP, UDP) and
	// application modules (HTTP, SNMP, etc.)
    // this also sets the MAC address
    StackInit();
 
    // start with DHCP disabled.
    DHCPDisable(0);
 
   #if defined(DWIFIcK_WiFi_Network)
       DWIFIcK_Init();
    #endif
}


/****************************************************************************
  Function:
    void EthernetBegin(const byte *rgbMac, const byte *rgbIP, const byte *rgbGateWay, const byte *rgbSubNet, const byte *rgbDNS1, const byte *rgbDNS2)

  Description:
    This routine impements the Arduino Ethernet.Begin Method. This initializes the
	board, start supporting tasks, builds a default application configuration data structure,
	overrides the configuration structure if static IPs or assigned MACs are specified,
	and starts the Ethernet stack.

  Precondition:
    None

  Parameters:
    rgbMac 	- If all 6 bytes are zero, than use the internal MCU programed MAC address
			as defined by Microchip. It will be a unique MAC address in the Microchip range. 
			The range will be somewhere starting with 00:04:A3:XX:XX:XX

			If non-zero, the specified MAC address will be used.

	rgbIP 	-	If all 4 bytes are zero, then DHCP is used and rest of the parameters are ignored

			If an IP is specified then DHCP is not used and the IP represents a static IP address to use. The 
			remainng parameters have value.

	rgbGateWay 	- 4 bytes IP address of the gateway to use. Only valid if rgbIP is specified
	rgbSubNet	- 4 byte mask representing the subnet mask.Only valid if rgbIP is specified
	rgbDNS1		- 4 byte IP address of the primary DNS server. Only valid if rgbIP is specified. This value may be 0s if not required
	rgbDNS2		- 4 byte IP address of the secondary DNS server. Only valid if rgbIP is specifed. This value may be 0s if not required

  Returns:

    None

  Remarks:
    None
  ***************************************************************************/
void EthernetBegin(const byte *rgbIP, const byte *rgbGateWay, const byte *rgbSubNet, const byte *rgbDNS1, const byte *rgbDNS2)
{

    // If Ethernet is running, don't start it again.
    if(!fIsEthernetEngineStopped)
    {
        return;
    }
    fIsEthernetEngineStopped = FALSE;

    // there is a chance that begin can come before wfConnect(), need to test.
    // if so, we need to make sure we WF connect here.
    DNETcK_Init();

	// if we are not to use DHCP; fill in what came in.
	if((rgbIP[0] | rgbIP[1] | rgbIP[2] | rgbIP[3]) != 0)
	{
        // not using DHCP, turn it OFF
        DHCPDisable(0);

		memcpy(&AppConfig.MyIPAddr, rgbIP, 4);
		memcpy(&AppConfig.MyGateway, rgbGateWay, 4);
		memcpy(&AppConfig.MyMask,rgbSubNet, 4);
		memcpy(&AppConfig.PrimaryDNSServer, rgbDNS1, 4);
		memcpy(&AppConfig.SecondaryDNSServer, rgbDNS2, 4);
		
        // set the default values as well
		AppConfig.DefaultIPAddr = AppConfig.MyIPAddr;
		AppConfig.DefaultMask = AppConfig.MyMask;
	}
    else
    {
        // we want to start over
        DHCPInit(0);
        DHCPEnable(0);
   }
}

/*****************************************************************************
  Function:
	void EthernetEnd(void)

  Summary:
	Terminates all stack functions

  Description:
 
  Precondition:

  Parameters:
	None

  Returns:
	None

  Remarks:
    This stops all stack tasks and turns off the PHY

 ***************************************************************************/
void EthernetEnd(void)
{
     int i = 0;
     UDP_SOCKET hUDP = 0;

    fIsEthernetEngineStopped = TRUE;

    // clear out the TCPIP sockets in use
    for(i = 0; i < MAX_TCP_SOCKETS; i++)
    {
        TCPClose(i);
        StackTask();
    }

    // close all of our UDP sockets
    for(hUDP = 0; hUDP < MAX_UDP_SOCKETS; hUDP++)
    {
        if(UDPCache[hUDP].rgbBuffer != NULL)
        {
            UDPClose(hUDP);
            StackTask();
        }
    }

    // make sure our UDP array is zeroed out.
	memset(UDPCache, 0, sizeof(UDPCache));

    DHCPDisable(0);
}

/****************************************************************************
  Function:
    void EthernetPeriodicTasks(void)

  Description:
    This routine will run the periodic tasks needed to keep the Ethernet
	stack alive and to run the tasks such as ping or DHCP as part of supporting the stack.

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    This funciton needs to be called on a regular basis in order to service
	incoming TCPIP / UDP tasks. If it is not called the stack will freeze.
	Most Arduino interface APIs specified in the file call ChipKITPeriodicTasks 
	implicitly so that it is called at the right time to execution the stack
	functions. But this routine is made available to the sketch so that the
	sketch can keep the stack alive while the sketch is idle.
  ***************************************************************************/
void EthernetPeriodicTasks(void)
{
    static bool fInPeriodicTasks = FALSE;

    // do not recursively execute this function.
    if(fInPeriodicTasks)
    {
        return;
    }
    fInPeriodicTasks = TRUE;

   	// This task performs normal stack task including checking
   	// for incoming packet, type of packet and calling
    // appropriate stack entity to process it.
    StackTask();

    // an annoying thing is that the MAL will not hold on to the
    // UDP buffer for another iteration of StackTask, so we must
    // buffer the UDP data so we don't lose it.
	// ChipKITUDPUpdateBufferCache();
    if( !fIsEthernetEngineStopped)
    {
        UpdateUDPCache();
    }

    // This tasks invokes each of the core stack application tasks
    StackApplications();

    // keep the DNS resolving, try to get it to resolve and release the DNS engine
    if(!fIsEthernetEngineStopped)
    {
        if(statusDNS == DNSResolving  ||  statusDNS == DNSIsBusy)
        {
            EthernetIsDNSResolved(szDNSNameResolving, NULL, 0, NULL);
        }
    }

    fInPeriodicTasks = FALSE;
}

/****************************************************************************
// ChipKIT Client APIs
// Here is where the chipKIT Arduino software compatible underlying
// APIs are implemented.
//***************************************************************************/

/*****************************************************************************
  Function:
	void PutDataGramSize(WORD cbSize, WORD iSize, byte * rgbBuffer, WORD cbBuffer)

  Summary:
	Puts the datagram size in the UDP Cache where the 2 byte size may wrap the cache

  Description:
 
  Precondition:

  Parameters:
	cbSize - The size of the following datagram, not including this 2 byte size field
    iSize - index into the cache buffer to put the size, iSize only has to be the modulo of the index
    rgbBuffer - the UDP cache buffer
    cbBuffer - The size of the UDP cache
 
  Returns:
	None

  Remarks:
    This will nomalize iSize before indexing in the cache; the size can span the end
    of the cache and wrap to the start.

 ***************************************************************************/
static void PutDataGramSize(WORD cbSize, WORD iSize, byte * rgbBuffer, WORD cbBuffer)
{
    rgbBuffer[NormalizeIndex(iSize, cbBuffer)] = (byte) (cbSize & 0x00FF);
    iSize++;
    rgbBuffer[NormalizeIndex(iSize, cbBuffer)] = (byte) ((cbSize & 0xFF00) >> 8);
}

/*****************************************************************************
  Function:
	void UpdateUDPEntryCache(UDP_SOCKET hUDP, UDPCacheEntry * pUDPE)

  Summary:
	For a specific socket, adds any incoming datagrams from the MAL to the socket cache

  Description:
 
  Precondition:

  Parameters:
	hUDP - The socket to update the cache with
    pUDPE - A pointer to the socket cache.

  Returns:
	None

  Remarks:
    This needs to be called with each pass of periodicTasks() to keep the socket caches up to date.

 ***************************************************************************/
static void UpdateUDPEntryCache(UDP_SOCKET hUDP, UDPCacheEntry * pUDPE)
{
    WORD cbReady = 0;
    DWORD cbT = 0;
    DWORD iEnd = 0;

    if(pUDPE->rgbBuffer == NULL)
    {
        return;
    }

    // see what we need to read
    cbReady = UDPIsGetReady(hUDP);
    cbT = cbReady + pUDPE->cbInUse + sizeof(WORD);

    // if there is nothing to read, we are done
    if(cbReady == 0) 
    {
        return;
    }

    // too big for us to cache it, just dump it; but don't purge existing data in the cache
    // or we have freezed the cache and we don't have room for this datagram
    else if(cbReady + sizeof(WORD) > pUDPE->cbBuffer  || (fDatagramPartiallyRead && cbT > pUDPE->cbBuffer))
    {
        void UDPDiscard();
        return;
    }

    // or we need to dump some old datagrams
    else if(cbT > pUDPE->cbBuffer) 
    {
        DWORD iStart = pUDPE->iStart;
        DWORD cbDump = cbT - pUDPE->cbBuffer;
        DWORD cbDumped = 0;
        DWORD cbDataGram = 0;

        // we want to maintian the integrity of datagrams, do not chop them
        // so when we dump, dump the whole datagram.            
        do
        {
            cbDataGram = DataGramSize(pUDPE->rgbBuffer, iStart, pUDPE->cbBuffer) + sizeof(WORD);
            cbDumped += cbDataGram;
            iStart += cbDataGram;
        } while(cbDump > cbDumped);
        
        pUDPE->iStart = NormalizeIndex(iStart, pUDPE->cbBuffer);          
        pUDPE->cbInUse -= cbDumped;
    }

    // at this point we have the room in the cache, and we can save the whole datagram and header

    // put the datagram length in, in two steps in case we are wrapping in the cache
    iEnd = NormalizeIndex(pUDPE->iStart + pUDPE->cbInUse, pUDPE->cbBuffer);
    PutDataGramSize(cbReady, iEnd, pUDPE->rgbBuffer, pUDPE->cbBuffer);
    iEnd += sizeof(WORD);
    pUDPE->cbInUse += sizeof(WORD);

    // see how much we can read in the first pass.
    iEnd = NormalizeIndex(iEnd, pUDPE->cbBuffer);
    cbT = pUDPE->cbBuffer - iEnd;
    cbT = Min(cbT, cbReady);

    // 1st pass read
    UDPGetArray(&pUDPE->rgbBuffer[iEnd], cbT);
    cbReady -= cbT;
    pUDPE->cbInUse += cbT;

    // the rest of the bytes should be able to read in the 2nd pass
    if(cbReady > 0)
    {
        iEnd = NormalizeIndex(iEnd + cbT, pUDPE->cbBuffer);
        UDPGetArray(&pUDPE->rgbBuffer[iEnd], cbReady);
        pUDPE->cbInUse += cbReady;
    }  
}

/*****************************************************************************
  Function:
	static void UpdateUDPCache(void)

  Summary:
	enumerates all sockets held by DNETcK and updates the socket cache for each socket

  Description:
 
  Precondition:

  Parameters:
	None
 
  Returns:
	None

  Remarks:
    This needs to be called with each pass of periodicTasks() to keep the socket caches up to date.

 ***************************************************************************/
static void UpdateUDPCache(void)
{
    byte hUDP = 0;

    for(hUDP = 0; hUDP < MAX_UDP_SOCKETS; hUDP++)
    {
        UpdateUDPEntryCache(hUDP, &UDPCache[hUDP]);
    }
}

/*****************************************************************************
  Function:
	byte * ExchangeCacheBuffer(byte hUDP, byte *rgbBufferNew, unsigned short cbBufferNew)

  Summary:
	changes a socket cache for a socket

  Description:
 
  Precondition:

  Parameters:
	hUDP    - the socket we are dealing with
    rgbBufferNew - the new socket cache to use
    cbBufferNew - the size fo the new socket cache
 
  Returns:
	The old socket cache buffer pointer

  Remarks:
    This needs to be called with each pass of periodicTasks() to keep the socket caches up to date.

 ***************************************************************************/
byte * ExchangeCacheBuffer(byte hUDP, byte *rgbBufferNew, unsigned short cbBufferNew)
{
    UDPCacheEntry * pUDPE = &UDPCache[hUDP];

    byte * rgbBuffOld = pUDPE->rgbBuffer;
    WORD cbLeftToCheck = pUDPE->cbInUse;;
    WORD iOldBuff = 0;

    WORD cbInUseNew = 0;
    WORD iNewBuff = 0;
    WORD cbRemaining = cbBufferNew;

    WORD cbDataGram = 0;

    // if nothing to copy or blanking out 
    if(rgbBufferNew == NULL || pUDPE->rgbBuffer == NULL || pUDPE->cbInUse == 0  || pUDPE->cbBuffer == 0)
    {
        pUDPE->cbInUse = 0; 
    }

    else
    {

        // copy over datagrams that fit
        iOldBuff = NormalizeIndex(pUDPE->iStart, pUDPE->cbBuffer);
        while(cbLeftToCheck > 0)
        {
            cbDataGram = DataGramSize(pUDPE->rgbBuffer, iOldBuff, pUDPE->cbBuffer) + sizeof(WORD);
            if(cbDataGram < cbRemaining)
            {
                WORD i = 0;

                // copy the bytes
                for(i = 0; i < cbDataGram; i++)
                {
                    rgbBufferNew[iNewBuff] = pUDPE->rgbBuffer[iOldBuff];
                    iNewBuff++;
                    iOldBuff = NormalizeIndex(iOldBuff+1, pUDPE->cbBuffer);
                }
          
                cbInUseNew += cbDataGram;
                cbRemaining -= cbDataGram;
            }
            cbLeftToCheck -= cbDataGram;
        }

        // fix up the new pointers
        pUDPE->cbInUse = cbInUseNew; 
    }

    // everything is fixed up to the start of the cache
    pUDPE->rgbBuffer = rgbBufferNew;
    pUDPE->cbBuffer = cbBufferNew;
    pUDPE->iStart = 0;

    return(rgbBuffOld);
}

/*****************************************************************************
  Function:
	void EthernetGetMACandIPs(byte *rgbMac, byte *rgbIP, byte *rgbGateWay, byte *rgbSubNet, byte *rgbDNS1, byte *rgbDNS2)

  Summary:
	Once the stack is initialized, this will return all of the network address known by the stack

  Description:
 
  Precondition:

  Parameters:
	rgbMac - the current MAC address in use
	rgbIP - the current IP (MyIP) address in use
	rgbGateWay - the gateway IP address in use
	rgbSubNet - the subnet mask in use
	rgbDNS1 - The IP address of the primary DNS server to use
	rgbDNS2 - The IP address of the secondary DNS server to use

  Returns:
	None

  Remarks:
    This will return junk if the stack is not initialized yet

 ***************************************************************************/
void EthernetGetMACandIPs(byte *rgbMac, byte *rgbIP, byte *rgbGateWay, byte *rgbSubNet, byte *rgbDNS1, byte *rgbDNS2)
{
    memcpy(rgbMac, &AppConfig.MyMACAddr, 6);
    memcpy(rgbIP, &AppConfig.MyIPAddr, 4);
    memcpy(rgbGateWay, &AppConfig.MyGateway, 4);
    memcpy(rgbSubNet, &AppConfig.MyMask, 4);
    memcpy(rgbDNS1, &AppConfig.PrimaryDNSServer, 4);
    memcpy(rgbDNS2, &AppConfig.SecondaryDNSServer, 4);
}

/*****************************************************************************
  Function:
	bool EthernetisInitialized(unsigned long msBlockMax, STATUS * pStatus)

  Summary:
	Determines if the stack is initialized and ready to go

  Description:
 
  Precondition:

  Parameters:
	msBlockMax - Max amount of time to block before returning if the stack has not initiailzed
    pStatus - A pointer to a status to return of where in the process stack initailization is

  Returns:
	true it the stack is initialized and ready to go, false if still processing or an error

  Remarks:
  
 ***************************************************************************/
bool EthernetisInitialized(unsigned long msBlockMax, STATUS * pStatus)
{
    STATUS status = NetworkInitialized;

    // if the engine is not running, then say we are stopped.
    if(fIsEthernetEngineStopped)
    {
        if(pStatus != NULL) *pStatus = NetworkNotInitialized;
        return(FALSE);
    }

    // this is just to make the zero time as fast as possible
    if(msBlockMax == 0)
    {
        if(DNETcK_IsMacConnected(&status))
        {
            if(DHCPIsEnabled(0) && !DHCPIsBound(0))
            {
                 status = DHCPNotBound;
            }
            else
            {
                status = NetworkInitialized;
            }
        }
    }

    // arp will not work right until DHCP finishes
	// if DHCP won't configure after the timeout; then return the error
	// maybe later it will configure, but until then, things might not work right.
    else
    {
        DWORD tStart = TickGet();
        DWORD tWait = TICKS_PER_MILSECOND * msBlockMax;

	    while(!DNETcK_IsMacConnected(NULL)  || (DHCPIsEnabled(0) && !DHCPIsBound(0)))
	    {
            if(hasTimeElapsed(tStart, tWait, TickGet()))
            {
                if(DNETcK_IsMacConnected(&status))
                {
                    if(DHCPIsEnabled(0) && !DHCPIsBound(0))
                    {
                         status = DHCPNotBound;
                    }
                    else
                    {
                        status = NetworkInitialized;
                    }
                }
                break;
            }
            EthernetPeriodicTasks();
        }
    }

    // only fill in the status if we got it
    if(pStatus != NULL)
    {
        *pStatus = status;
    }

    return(status == NetworkInitialized);
}

/*****************************************************************************
  Function:
	bool EthernetIsDNSResolved(const char * szHostName, byte * pIP, unsigned long msBlockMax, STATUS * pStatus)

  Summary:
	Determines if the current DNS operation is complete or not

  Description:
 
  Precondition:

  Parameters:
	szHostName - the hostname to resolve, this must stay valid until completion of the process
    iIP         - On successful completion, a pointer to a IPv4 to receive the IP address for the hostname
    msBlockMax  - The maximum amount of time to block before returning before resolution completes
    pStatus     - a pointer to a status that indicates where in the resolution process we are

  Returns:
	true if the hostname is resolved and the IP address has been filled in, false if still resolving or an error

  Remarks:
    The IP will be junk until the resolution process is complete.

 ***************************************************************************/
bool EthernetIsDNSResolved(const char * szHostName, byte * pIP, unsigned long msBlockMax, STATUS * pStatus)
{
    DWORD tStart = 0;
    DWORD tWait = msBlockMax * TICKS_PER_MILSECOND;
    static bool fRecursive = FALSE;

    EthernetPeriodicTasks();

    if(!EthernetisInitialized(msBlockMax, pStatus))
    {
        return(FALSE);
    }

    if(szHostName == NULL)
    {
       if(pStatus != NULL) *pStatus = DNSHostNameIsNULL;
       return(FALSE);
    }

    // don't allow recursion, just get out
    if(fRecursive)
    {
        if(pStatus != NULL) *pStatus = DNSRecursiveExit;
        return(FALSE);
    }

    // FROM HERE ON THIS ROUTINE MUST EXIT AT THE END
    // SO THE RECURSION FLAG CAN BE RESET, NOT JUST RETURN
    fRecursive = TRUE;

    tStart = TickGet();
    do
    {
        EthernetPeriodicTasks();

        switch(statusDNS)
        {

        case DNSLookupSuccess:

            if(szDNSNameResolving != NULL && strcmp(szDNSNameResolving, szHostName) == 0)
            {
                if(pIP != NULL)
                {
                    *((IP_ADDR *) pIP) = DNSLastResolvedHostIP;
                }
                break;
            }
 
        // if we get here, we are on a new request
        default:
                szDNSNameResolving = szHostName;
                DNSLastResolvedHostIP.Val = 0;

        case DNSIsBusy:

            // someone else may be resolving a DNS lookup
            // and this could be a stack application such as SNTP (in fact this is the case many times)
            // we need to wait a while for it to clear, and we need to run EthernetPeriodicTasks(void)
            // in order to make sure the DNS lock gets released; but EthernetPeriodicTasks(void) calls
            // EthernetIsDNSResolved and that can call recursion, so we must block recursion in EthernetIsDNSResolved.
  
            // it is very important that once
            // we aquire the DNS lock, that 
            // statusDNS stays at DNSResolving
            // until the DNS lock is released
            // this tells me that my engine is attempting a resolve
            // and not something else in the system trying to do a DNS lookup
            if(DNSBeginUsage())
            {
                DNSResolve((byte *) szHostName, DNS_TYPE_A);
                statusDNS = DNSResolving;
            }
            else 
            {
                statusDNS = DNSIsBusy;
            }
            break;
 
        // if we get here, we are resolving
        case DNSResolving:

            if(DNSIsResolved(&DNSLastResolvedHostIP))
            {
                if(DNSEndUsage())
                {
                    statusDNS = DNSLookupSuccess;
                    if(pIP != NULL)
                    {
                        *((IP_ADDR *) pIP) = DNSLastResolvedHostIP;
                    }
                }
                else
                {
                    statusDNS = DNSResolutionFailed;
                }
            }
            break;
        } 

    } while(statusDNS != DNSLookupSuccess && statusDNS != DNSResolutionFailed && !hasTimeElapsed(tStart, tWait, TickGet()));

    // return status if requsted
    if(pStatus != NULL) *pStatus = statusDNS;
    fRecursive = FALSE;

    return(statusDNS == DNSLookupSuccess);
}

/****************************************************************************
  Function:
    void EthernetRequestARPIpMacResolution(const byte * pIP)

  Description:
    Sends an UDP ARP request on the local network to resolve an IP to a MAC

  Precondition:
 
  Parameters:
    pIP - a pointer to an IPv4 to get the MAC for

  Returns:
    None

  Remarks:  
    None
  ***************************************************************************/
void EthernetRequestARPIpMacResolution(const byte * pIP)
{
    if(pIP != NULL)
    {
        IP_ADDR ip = *(IP_ADDR *) pIP;

        // if it is not the broadcast IP
        if(ip.Val != 0xFFFFFFFF)
        {
            ARPResolve(&ip);
        }
     }
    EthernetPeriodicTasks();
}

/****************************************************************************
  Function:
    bool EthernetIsARPIpMacResolved(const byte * pIP, byte * pMAC, unsigned long msBlockMax)

  Description:
    Determines if the response to the ARP request has come in and we have the MAC resolved

  Precondition:
 
  Parameters:
    pIP - a pointer to an IPv4 to get the MAC for
    pMAC - a pointer to a MAC to receive the resolved MAC address

  Returns:
    None

  Remarks:  
    None
  ***************************************************************************/
bool EthernetIsARPIpMacResolved(const byte * pIP, byte * pMAC, unsigned long msBlockMax)
{
	DWORD tStart = TickGet();
    DWORD tWait = TICKS_PER_MILSECOND * msBlockMax;
    IP_ADDR ip;

    // have to give me an IP and MAC buffer
    if(pIP == NULL && pMAC == NULL)
    {
        return(FALSE);
    }

    // get a local variable for the IP
    ip = *(IP_ADDR *) pIP;

    // this is the broadcast IP, so return the Broadcast MAC
    if(ip.Val == 0xFFFFFFFF)
    {
        memset(pMAC, 0xFF, sizeof(MAC_ADDR));
        return(TRUE);
    }

    // resolve the IP address to get a MAC
    while(!ARPIsResolved(&ip, (MAC_ADDR *) pMAC))
    {
        EthernetPeriodicTasks();

        if(hasTimeElapsed(tStart, tWait, TickGet()))
        {
            return(FALSE);
        }      
    }
 
    return(TRUE);
}

/****************************************************************************
  Function:
    void EthernetDNSTerminate(void)

  Description:
    Forcefully teriminates a DNS resolution and frees the DNS lock

  Precondition:
 
  Parameters:

  Returns:
    None

  Remarks:  
    None
  ***************************************************************************/
void EthernetDNSTerminate(void)
{
    // if we are the owner of the DNS engine, then
    // we will have the DNS lock and that is identified
    // by being in the DNSResolving state
    if(statusDNS == DNSResolving)
    {
        // just blow the lock away
        DNSEndUsage();
        statusDNS = DNSUninitialized;
    }
    EthernetPeriodicTasks();
}

/****************************************************************************
  Function:
    byte TcpClientConnectByName(const char * szHostName, unsigned short port)

  Description:
    Gets a TcpSocket using a hostname

  Precondition:
 
  Parameters:
    szHostName - the hostname to connect to
    port        - the port to connect to

  Returns:
    The socket if one was gotten, INVALID_SOCKET if not 

  Remarks:  
    None
  ***************************************************************************/
byte TcpClientConnectByName(const char * szHostName, unsigned short port)
{
    TCP_SOCKET hTCP = TCPOpen((DWORD) szHostName, TCP_OPEN_RAM_HOST, (WORD) port, TCP_PURPOSE_DEFAULT);
    EthernetPeriodicTasks();
    return(hTCP);
}

/****************************************************************************
  Function:
    byte TcpClientConnectByEndPoint(const byte * pIP, unsigned short port)

  Description:
    Gets a TcpSocket using an IP address

  Precondition:
 
  Parameters:
    pIP - a pointer to an IPv4 to connect to
    port        - the port to connect to

  Returns:
    The socket if one was gotten, INVALID_SOCKET if not 

  Remarks:  
    None
  ***************************************************************************/
byte TcpClientConnectByEndPoint(const byte * pIP, unsigned short port)
{
    TCP_SOCKET hTCP = TCPOpen(((IP_ADDR *) pIP)->Val, TCP_OPEN_IP_ADDRESS, (WORD) port, TCP_PURPOSE_DEFAULT);
    EthernetPeriodicTasks();
    return(hTCP);
}

/****************************************************************************
  Function:
    byte TcpServerStartListening(unsigned short port)

  Description:
    Gets a TcpSocket to listen on

  Precondition:
 
  Parameters:
    port        - the port to listen on

  Returns:
    The socket if one was gotten, INVALID_SOCKET if not 

  Remarks:  
    None
  ***************************************************************************/
byte TcpServerStartListening(unsigned short port)
{
    TCP_SOCKET hTCP = TCPOpen(0, TCP_OPEN_SERVER, (WORD) port, TCP_PURPOSE_DEFAULT);
    EthernetPeriodicTasks();
    return(hTCP);
}

/****************************************************************************
  Function:
    byte UdpClientSetEndPoint(const byte * pIP, const byte * pMAC, unsigned short remotePort, unsigned short localPort)

  Description:
    Gets a UDP Socket with the specified remote/local endpoint

  Precondition:
 
  Parameters:
    pIP         - A pointer to an IPv4 to use as the remote endpoint IP
    pMAC        - A pointer to a MAC containing the local subnets machine's MAC to send datagrams to
    remotePort  - the remote endpoint port
    localPort   - the local port to use, if zero, one will be assigned.

  Returns:
    The socket if one was gotten, INVALID_UDP_SOCKET if not 

  Remarks:  
    None
  ***************************************************************************/
byte UdpClientSetEndPoint(const byte * pIP, const byte * pMAC, unsigned short remotePort, unsigned short localPort)
{
    NODE_INFO nodeInfo;
    UDP_SOCKET hUDP = INVALID_UDP_SOCKET;

    nodeInfo.IPAddr = *((IP_ADDR *) pIP);       // must be aligned
    nodeInfo.MACAddr = *((MAC_ADDR *) pMAC);    // must be aligned

    // if this is a broadcast address, then broadcast
    if(nodeInfo.IPAddr.Val == 0xFFFFFFFF)
    {
         hUDP = UDPOpen(localPort, NULL, remotePort);
    }
    else
    {
        hUDP = UDPOpen(localPort, &nodeInfo, remotePort);
    }

    EthernetPeriodicTasks();
    return(hUDP);
}

/*****************************************************************************
  Function:
	void GetUdpSocketEndPoints(TCP_SOCKET hTCP, IP_ADDR * premoteIP, MAC_ADDR * pRemoteMAC, WORD * pRemotePort, WORD * pLocalPort)

  Summary:
	Returns the local and remote endpoints set up for a socket

  Description:
    This function should be called after TCPIsConnected passes so that MyTCB will
    return the remote endpoint data (instead of a host name).
    This will grovel down into the TCB structure and get the connected endpoints, that is remoteIP, MAC and port.
    as well as returning the local port used to talk to the remote host.

  Precondition:
	TCP is initialized and the socket is connected.

  Parameters:
	hTCP - The socket we want the remote information about
    pRemoteIP - a pointer to and IP_ADDR to receive the remote ip connected to.
    pRemoteMAC - a pointer to a MAC_ADDR to receive the remoete MAC address connected to.
    pRemotePort - pointer to a WORD to receive the remote port connected to.
    pLocalPort - pointer to a WORD to receive the local port in this connection

  Returns:
	None

  Remarks:
    Clearly there is no checking, it just should work; if not junk will come back.

 ***************************************************************************/
void GetUdpSocketEndPoints(UDP_SOCKET hUDP, IP_ADDR * pRemoteIP, MAC_ADDR * pRemoteMAC, WORD * pRemotePort, WORD * pLocalPort)
{

    // most of the out variables are already correct, so if I can not find the socket info
    // do not tamper with the output variables
	if(hUDP >= MAX_UDP_SOCKETS)
	{
		return;
	}

    // fill this all in
    *pRemoteIP = UDPSocketInfo[hUDP].remote.remoteNode.IPAddr;
    *pRemoteMAC = UDPSocketInfo[hUDP].remote.remoteNode.MACAddr;
    *pRemotePort = UDPSocketInfo[hUDP].remotePort;
    *pLocalPort = UDPSocketInfo[hUDP].localPort;
}

/****************************************************************************
  Function:
    unsigned short UdpClientAvailable(byte hUDP)

  Description:
    Gets the number of bytes in the next datagram in the datagram cache on this socket

  Precondition:
 
  Parameters:
    hUDP        - The socket to check the datagram cache from

  Returns:
    The number of bytes in the next datagram in the cache

  Remarks:  
    None
  ***************************************************************************/
unsigned short UdpClientAvailable(byte hUDP)
{
    // run the the stack
    EthernetPeriodicTasks();

    // not a valid request
    if(hUDP >= MAX_UDP_SOCKETS || UDPCache[hUDP].rgbBuffer == NULL || UDPCache[hUDP].cbInUse == 0)
    {
        return(0);
    }

    return(DataGramSize(UDPCache[hUDP].rgbBuffer, UDPCache[hUDP].iStart, UDPCache[hUDP].cbBuffer));
}

/****************************************************************************
  Function:
    void UdpClientEmptyNextDataGram(byte hUDP)

  Description:
    Purges the next datagram out of the socket's datagram cache

  Precondition:
 
  Parameters:
    hUDP        - The socket to get the cache from

  Returns:
    None

  Remarks:  
    None
  ***************************************************************************/
void UdpClientEmptyNextDataGram(byte hUDP)
{
    WORD cbDataGram = 0;

    // not a valid request
    if(hUDP >= MAX_UDP_SOCKETS || UDPCache[hUDP].rgbBuffer == NULL || UDPCache[hUDP].cbInUse == 0)
    {
        return;
    }

    // get the length to the next datagarm
    cbDataGram = DataGramSize(UDPCache[hUDP].rgbBuffer, UDPCache[hUDP].iStart, UDPCache[hUDP].cbBuffer) + sizeof(WORD);

    // fixup the buffer to remove this data gram
    UDPCache[hUDP].cbInUse -= cbDataGram;
    UDPCache[hUDP].iStart = NormalizeIndex(UDPCache[hUDP].iStart + cbDataGram, UDPCache[hUDP].cbBuffer);

    // we are clean to a new datagram
    fDatagramPartiallyRead = FALSE;
}

/****************************************************************************
  Function:
    unsigned short UdpClientRemoveBytesFromDataGram(byte hUDP, unsigned short cbRemove)

  Description:
    Removes a specifed number of bytes from the next datagram

  Precondition:
 
  Parameters:
    hUDP        - The socket to get the cache for
    cbRemove    - the number of bytes to remove from the next datagram

  Returns:
    The number of bytes left in the datagram still to be read

  Remarks:  
    May be a partial datagram or the whole datagram
  ***************************************************************************/
unsigned short UdpClientRemoveBytesFromDataGram(byte hUDP, unsigned short cbRemove)
{
    WORD cbDataGram = 0;

    // not a valid request
    if(hUDP >= MAX_UDP_SOCKETS || UDPCache[hUDP].rgbBuffer == NULL || UDPCache[hUDP].cbInUse == 0)
    {
        return;
    }

    cbDataGram = DataGramSize(UDPCache[hUDP].rgbBuffer, UDPCache[hUDP].iStart, UDPCache[hUDP].cbBuffer);
    cbRemove = Min(cbRemove, cbDataGram);

    // we are removing the whole datagram
    if(cbRemove == cbDataGram)
    {
        // we are removing the header, remove it as well
        cbDataGram += sizeof(WORD);

        // fixup the buffer to remove this data gram
        UDPCache[hUDP].cbInUse -= cbDataGram;
        UDPCache[hUDP].iStart = NormalizeIndex(UDPCache[hUDP].iStart + cbDataGram, UDPCache[hUDP].cbBuffer);

        // we are clean to a new datagram
        fDatagramPartiallyRead = FALSE;

        return(0);
    }

    // it is a partial datagram
    else
    {
    WORD cbLeft = cbDataGram - cbRemove;

    // fixup the buffer to remove this data gram
    UDPCache[hUDP].cbInUse -= cbRemove;
    UDPCache[hUDP].iStart = NormalizeIndex(UDPCache[hUDP].iStart + cbRemove, UDPCache[hUDP].cbBuffer);

    // iStart points to were the size needs to be put, but we must write the updated size there.
    PutDataGramSize(cbLeft, UDPCache[hUDP].iStart, UDPCache[hUDP].rgbBuffer, UDPCache[hUDP].cbBuffer);


    // partially read datagram, free the cache if we overflow
    fDatagramPartiallyRead = TRUE;

    // return how much is left in there
    return(cbLeft);
    }
}

/****************************************************************************
  Function:
    unsigned short UdpClientPeek(byte hUDP, byte *rgbPeek, unsigned short cbPeekMax, unsigned short iIndex)

  Description:
    Peeks bytes out of the next datagram starting at the specified index

  Precondition:
 
  Parameters:
    hUDP        - The socket to get the cache for
    rgbPeek     - a pointer to a buffer to receive the bytes peeked
    cbPeekMax   - the maximum size of the receive buffer
    iIndex      - how many bytes into the datagram to index before copying peeked bytes.

  Returns:
    The acutal number of bytes peeked.

  Remarks:  
    
  ***************************************************************************/
unsigned short UdpClientPeek(byte hUDP, byte *rgbPeek, unsigned short cbPeekMax, unsigned short iIndex)
{
    WORD cbDataGram = 0;
    WORD iStart = 0;
    WORD cbRead = 0;

    // not a valid request
    if(hUDP >= MAX_UDP_SOCKETS || UDPCache[hUDP].rgbBuffer == NULL || UDPCache[hUDP].cbInUse == 0)
    {
        return(0);
    }

    cbDataGram = DataGramSize(UDPCache[hUDP].rgbBuffer, UDPCache[hUDP].iStart, UDPCache[hUDP].cbBuffer);
 
    // see if we are peeking with an offset
    if(iIndex >= cbDataGram)
    {
        return(0);
    }
    else 
    {
        cbDataGram -= iIndex;
    }
   cbPeekMax = Min(cbPeekMax, cbDataGram);

    // Skip the size and get to the data
    iStart = NormalizeIndex(UDPCache[hUDP].iStart + iIndex + sizeof(WORD), UDPCache[hUDP].cbBuffer);

    // Read what I can until the end of the buffer
    cbRead = Min(UDPCache[hUDP].cbBuffer - iStart, cbPeekMax);
    memcpy(rgbPeek, &UDPCache[hUDP].rgbBuffer[iStart], cbRead);

    // and then from the start of the buffer read the rest
    if(cbRead < cbPeekMax)
    {
        // this should calculate to zero; as we wrapping in the cache
        iStart = NormalizeIndex(iStart + cbRead, UDPCache[hUDP].cbBuffer);

        // read the rest of the data
        memcpy(&rgbPeek[cbRead], &UDPCache[hUDP].rgbBuffer[iStart], cbPeekMax - cbRead);
    }

    // return how many bytes read
    return(cbPeekMax);
}

