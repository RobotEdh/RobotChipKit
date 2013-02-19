/************************************************************************/
/*																		*/
/*	DWIFIcKAPI.c--  DWIFIcK interface APIs to implement the             */
/*                  the interface between the DWIFIcK C++ and the       */
/*					Microchip WIFI MAL                                  */
/*																		*/
/************************************************************************/
/*	Author: 	Keith Vogel 											*/
/*	Copyright 2012, Digilent Inc.										*/
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
/*	This library is explicity targeting the Digilent chipKIT    		*/
/*	software compatible product line and the MRF24WB0MA WiFi MAC.       */
/*																		*/
/*	This module exposes API's to be used in conjuction with 			*/
/*	DWIFIcK.cpp to implement an interface between the C++ and        	*/
/*	the Microchip WIFI MAL											    */
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	1/31/2012(KeithV): Created											*/
/*																		*/
/************************************************************************/

#define THIS_IS_STACK_APPLICATION

// Include all headers for any enabled TCPIP Stack functions
#include "TCPIP Stack/TCPIP.h"

// not needed, but makes VS intellisense more reasonable
#undef __cplusplus

#if defined ( EZ_CONFIG_SCAN )
#include "TCPIP Stack/WFEasyConfig.h"
#endif /* EZ_CONFIG_SCAN */

// Include functions specific to this stack application
#include "../../DNETcK/DNETcK.h"
#include "../DWIFIcK.h"
#include "../../DNETcK/utility/DNETcKAPI.h"
#include "./DWIFIcKAPI.h"

// ugly! but the MAL WiFi code was written to assert on simple errors
// we have to get back to the calling function to return an error.
#include <setjmp.h>
static jmp_buf jmpbuf;
static BOOL    fSetJump = FALSE;

// Used for Wi-Fi assertions
#define WF_MODULE_NUMBER   WF_MODULE_MAIN_DEMO

// Declare AppConfig structure and some other supporting stack variables
extern APP_CONFIG AppConfig;

// our local variables
static BOOL gfScanDone = FALSE;
static BOOL gfScanInProgress = FALSE;
static WORD gcScan = 0;
static BYTE gConnState = 0;
static BYTE gConnectionProfileID = 0;

/*****************************************************************************
  Function:
	void InitWiFiAppConfig(void)

  Summary:
	Puts the default WiFi parameters in AppConfig

  Description:
 
  Precondition:

  Parameters:
	None
 
  Returns:
	NONE

  Remarks:
 ***************************************************************************/
void InitWiFiAppConfig(void)
{
    	// Load the default SSID Name
		WF_ASSERT(sizeof(MY_DEFAULT_SSID_NAME) <= sizeof(AppConfig.MySSID));
		memcpypgm2ram(AppConfig.MySSID, (ROM void*)MY_DEFAULT_SSID_NAME, sizeof(MY_DEFAULT_SSID_NAME));
		AppConfig.SsidLength = sizeof(MY_DEFAULT_SSID_NAME) - 1;
	
	    AppConfig.SecurityMode = MY_DEFAULT_WIFI_SECURITY_MODE;
	    AppConfig.WepKeyIndex  = MY_DEFAULT_WEP_KEY_INDEX;
	        
	    #if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_OPEN)
	        memset(AppConfig.SecurityKey, 0x00, sizeof(AppConfig.SecurityKey));
	        AppConfig.SecurityKeyLength = 0;
	
	    #elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_40
	        memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_WEP_KEYS_40, sizeof(MY_DEFAULT_WEP_KEYS_40) - 1);
	        AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_WEP_KEYS_40) - 1;
	
	    #elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_104
			memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_WEP_KEYS_104, sizeof(MY_DEFAULT_WEP_KEYS_104) - 1);
			AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_WEP_KEYS_104) - 1;
	
	    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_KEY)       || \
	            (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_KEY)      || \
	            (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_KEY)
			memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_PSK, sizeof(MY_DEFAULT_PSK) - 1);
			AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_PSK) - 1;
	
	    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_PASS_PHRASE)     || \
	            (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_PASS_PHRASE)    || \
	            (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE)
	        memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_PSK_PHRASE, sizeof(MY_DEFAULT_PSK_PHRASE) - 1);
	        AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_PSK_PHRASE) - 1;
	
	    #else 
	        #error "No security defined"
	    #endif /* MY_DEFAULT_WIFI_SECURITY_MODE */
}

/*****************************************************************************
  Function:
	void DWIFIcK_Init(void)

  Summary:
	Initializes the MRF24WB0M to a working state

  Description:
 
  Precondition:

  Parameters:
	None
 
  Returns:
	NONE

  Remarks:
 ***************************************************************************/
void DWIFIcK_Init(void)
{
    static UINT8 channelList[] = MY_DEFAULT_CHANNEL_LIST;
    UINT8 cpMask = 0;
    int i = 0;

    // WiFi initialization stuff
    WF_CASetScanType(MY_DEFAULT_SCAN_TYPE);                 // passive
    WF_CASetChannelList(channelList, sizeof(channelList));  // all channels
    WF_CASetListRetryCount(MY_DEFAULT_LIST_RETRY_COUNT);    // 5
   	WF_CASetBeaconTimeout(40);                              // how many becon missing before beacon timeout action taken
    WF_CASetBeaconTimeoutAction(WF_ATTEMPT_TO_RECONNECT);   // try to reconnect.
    //WF_CASetBeaconTimeoutAction(WF_DO_NOT_ATTEMPT_TO_RECONNECT);   // do not reconnect.

 	WF_CASetEventNotificationAction(MY_DEFAULT_EVENT_NOTIFICATION_LIST);    // all notificaitons
    
#if defined(WF_USE_POWER_SAVE_FUNCTIONS)
    if (MY_DEFAULT_PS_POLL == WF_ENABLED)
    {    
       	/* Enable low power (PS-Poll) mode */
        WF_PsPollEnable(TRUE);
    }    
    else
    {
       	/* disable low power (PS-Poll) mode */
      	WF_PsPollDisable();
    }    
#endif

    // want to disconnect
    if(WFisConnected())  // put this in if disconnect can assert if no connection is made
    {
        WF_CMDisconnect();
    }

    // release all of the in memory profiles
    WF_CPGetIds(&cpMask);
    for(i=1; i<=2; i++)
    {
        if((cpMask & 0x1) == 0x1)
        {
            WF_CPDelete(i);
        }
        cpMask = cpMask >> 1;
    }
}

/*****************************************************************************
 * FUNCTION: int WFcK_Connect(byte secrityMode, const char * szSsid, const byte * rgbKey, int cbKey, int iKey, STATUS * pStatus)
 *
 * RETURNS:  The connection ID if the profile was created and connection was started, 0 if no profile or connection was made
 *
 * PARAMS:  secrityMode     --  A DWIFIcK::SECURITY type. Like WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE or the like
 *          szSsid          -- a zero terminated string representing the SSID of the WiFi network to connect to.
 *          rgbKey          -- This is:
 *                              NULL: for no security 
 *                              zero terminated string for a passphrass for WPA(2)
 *                              A byte array containing the 32 bytes of key for WPA(2)
 *                              The 20 byte WEP40 key
 *                              The 52 byte WEP104 key
 *          cbKey               The lenght of rgbKey in bytes
 *          iKey                if WEP40 or WEP104, the key index for the key to use
 *          pStatus             Optional (may be NULL) status parameter as to the success of the call (DNETcK::STATUS)
 *
 *  NOTES:   We only support 1 connected profile at a time.
 *          This function will return immediately, you must wait for WF_EVENT_CONNECTION_SUCCESSFUL before doing any other WiFi or Network calls
 *
 *
 *****************************************************************************/
int WFcK_Connect(byte secrityMode, const char * szSsid, const byte * rgbKey, int cbKey, int iKey, STATUS * pStatus)
{
    int conID = 0;      // must int to zero so upper bits are zero

    if(gfScanInProgress)
    {
       if(pStatus != NULL) *pStatus = WFStillScanning;
       return(0);   
    }

    // just make sure we are init
    DNETcK_Init();

    // see if we are already connected, get out; must disconnect first.
    if(gConnectionProfileID != 0)
    {
        if(pStatus != NULL) *pStatus = ConnectionAlreadyDefined;
        return(FALSE);
    }

	AppConfig.SsidLength = strlen(szSsid);
	memcpy(AppConfig.MySSID, szSsid, AppConfig.SsidLength);
	AppConfig.SecurityMode = secrityMode;
	AppConfig.SecurityKeyLength = cbKey;
	memcpy(AppConfig.SecurityKey, rgbKey, AppConfig.SecurityKeyLength);
    AppConfig.WepKeyIndex = iKey;

    /* create a Connection Profile */
    fSetJump = TRUE;
    if(setjmp(jmpbuf))
    {
        fSetJump = FALSE;
        if(pStatus != NULL) *pStatus = WFAssertHit;
        return(0);
    }

    if(pStatus != NULL) *pStatus = WFUnableToGetConnectionID;
    WF_CPCreate(&conID);
    fSetJump = FALSE;

    // we made a profile, now connect to it; make sure NOT to use defaults
    if(conID != 0)
    {
        WFcK_Connect_By_ProfileID(conID, pStatus);
    }

	return(gConnectionProfileID);
}   

/*****************************************************************************
 * FUNCTION: bool WFcK_Connect_By_ProfileID(int conID, STATUS * pStatus)
 *
 * RETURNS:  True if the connection process was started. False otherwise
 *
 * PARAMS:  conID     --  The profile to use. If zero is specified, the defualt WiFi parameter from WF_Config.h are used.
 *          pStatus   --  Optional (may be NULL) status parameter as to the success of the call (DNETcK::STATUS)
 *
 *  NOTES:   We only support 1 connected profile at a time.
 *          This function will return immediately, you must wait for WF_EVENT_CONNECTION_SUCCESSFUL before doing any other WiFi or Network calls
 *
 *
 *****************************************************************************/
bool WFcK_Connect_By_ProfileID(int conID, STATUS * pStatus)
{
    unsigned int cpMask = 0;

   // if the connection ID is zero, then pull in the defaults.
    if(conID == 0)
    {
        InitWiFiAppConfig();
        return(WFcK_Connect(AppConfig.SecurityMode, AppConfig.MySSID, AppConfig.SecurityKey, AppConfig.SecurityKeyLength, AppConfig.WepKeyIndex, pStatus));
    }

    if(gfScanInProgress)
    {
       if(pStatus != NULL) *pStatus = WFStillScanning;
       return(0);   
    }

    // just make sure we are init
    DNETcK_Init();

    if(pStatus != NULL) *pStatus = WFUnableToGetConnectionID;

    fSetJump = TRUE;
    if(setjmp(jmpbuf))
    {
        fSetJump = FALSE;
        if(pStatus != NULL) *pStatus = WFAssertHit;
        return(FALSE);
    }

    // make sure this is a profile that exists
    WF_CPGetIds(&cpMask);
    if(conID == 0 || conID > WF_MAX_NUM_CONNECTION_PROFILES || (((cpMask >> (conID-1)) & 0x1) != 0x1))
    {
        if(pStatus != NULL) *pStatus = WFInvalideConnectionID;
        fSetJump = FALSE;
        return(FALSE);
    }

    // WF_CPSetNetworkType(gConnectionProfileID, scanResult.bssType);
    WF_CPSetNetworkType(conID, MY_DEFAULT_NETWORK_TYPE);

    WF_CPSetSsid(conID, 
                    AppConfig.MySSID, 
                    AppConfig.SsidLength);
    
    WF_CPSetSecurity(conID,
                        AppConfig.SecurityMode,
                        AppConfig.WepKeyIndex,   /* only used if WEP enabled */
                        AppConfig.SecurityKey,
                        AppConfig.SecurityKeyLength);

    gConnState = 0;
    WF_CMConnect(conID);

    if(pStatus != NULL) *pStatus = WaitingConnect;

    gConnectionProfileID = conID;
  
    fSetJump = FALSE;
}

/*****************************************************************************
  Function:
	bool WFcK_IsConnected(int connectionID, STATUS * pStatus)

  Summary:
	Determines if we are currently connencted to an AP.

  Description:
 
  Precondition:

  Parameters:
	connectionID    -- The connection or Profile ID, this may be any supported by the hardware
    pStatus         -- A pointer to a DNETcK::STATUS to recieve the status of the WiFi connection.

  Returns:
	TRUE if connected, FALSE if not currently connected.

  Remarks:
 
 ***************************************************************************/
bool WFcK_IsConnected(int conID, STATUS * pStatus)
{
    STATUS status= NotConnected;
    BYTE connState = gConnState; // defeat volitility from the ISR setting this
 
    // if requesting the active connection
    if(conID == 0)
    {
        conID = gConnectionProfileID;
    }

    if(conID == gConnectionProfileID)
    {
        switch(connState)
        {

        // initial state, never been set by the callback
        case 0:

            // however we have established a profile to connect to
            if(conID != 0)
            {
                status = WaitingConnect;
            }

            // haven't done anything yet
            else
            {
                status = NotConnected;
            }
            break;

        case WF_EVENT_CONNECTION_SUCCESSFUL:
        case WF_EVENT_CONNECTION_REESTABLISHED:
            status = Connected;
            break;

        case WF_EVENT_CONNECTION_TEMPORARILY_LOST:
            status = WaitingReConnect;
            break;

        case WF_EVENT_CONNECTION_FAILED:
        case WF_EVENT_CONNECTION_PERMANENTLY_LOST:
        default:
            status = NotConnected;
            break;
        }
    }

    // not even talking about the connected profile
    else
    {
        status = NotConnected;
    }

    if(pStatus != NULL) *pStatus = status;
    return(status == Connected);
 }

/*****************************************************************************
  Function:
	void WFcK_Disconnect(int conID)

  Summary:
	Disconnects the conID from the WiFi AP. 

  Description:
 
  Precondition:

  Parameters:
	connectionID    -- The connection or Profile ID to disconnect from the WiFi; if zero the current connection is disconnected.
 
  Returns:
	NONE

  Remarks:
    If not currently connected, it deletes the connection profile if it is a RAM profile.
    THIS MAY NOT BE WHAT WE WANT TO DO!
 ***************************************************************************/
void WFcK_Disconnect(int conID)
{
    STATUS status = NotConnected;

    // if requesting the active connection 
    if(conID == 0)
    {
        conID = gConnectionProfileID;
    }

    // if we are connected, disconnect
    WFcK_IsConnected(conID, &status);
    if(status != NotConnected)
    {
        WF_CMDisconnect();
    }

    gConnectionProfileID = 0;
    gConnState = 0;

    // if this has a RAM based profile, delete it as well.
    // WE MAY WISH TO REMOVE THIS CODE! WE ARE NOT ASSUMING SAVING OF PROFILES.... YET!
    if(conID == 1 || conID == 2)
    {         
        WF_CPDelete(conID);
    }

    // the follow should absolutely NOT be needed! But because the MRF24WB0MA
    // and WiFi MAL are so prone to getting hosed up, it is best to just 
    // restart everyting. This will even send a reset to the hardware if the reset
    // line is hooked up.
    MACInit();
    gConnectionProfileID = 0;
    gConnState = 0;
    DWIFIcK_Init();
}

/*****************************************************************************
  Function:
	bool WFcK_GetSecurityInfo(int connectionID, SECINFO * psecInfo)

  Summary:
	Get the current security info for the connection profile

  Description:
 
  Precondition:

  Parameters:
	connectionID    -- The connection or Profile ID to get the key info for
    psecInfo        -- A pointer to a SECINFO structure as defined in DWIFIcK::SECINFO
 
  Returns:
	True if the information was obtained, FALSE otherwise

  Remarks:
 ***************************************************************************/
bool WFcK_GetSecurityInfo(int conID, SECINFO * psecInfo)
{
    UINT8 cpMask = 0;

    // if requesting the active connection
    if(conID == 0)
    {
        conID = gConnectionProfileID;
    }

    // make sure this is a profile we have.
    WF_CPGetIds(&cpMask);
    if(conID <= 0 || conID > WF_MAX_NUM_CONNECTION_PROFILES  ||  ((cpMask >> (conID-1)) & 0x1) != 0x1)
    {
        return(FALSE);
    }
       
    memset(psecInfo, 0, sizeof(SECINFO));

    fSetJump = TRUE;
    if(setjmp(jmpbuf))
    {
        fSetJump = FALSE;
        return(FALSE);
    }

    WF_CPGetSecurity(conID, &psecInfo->securityType, &psecInfo->key.index, psecInfo->key.wpa.rgbKey, &psecInfo->key.cbKey);

    fSetJump = FALSE;
    return(TRUE);
}

/*****************************************************************************
  Function:
	bool WFcK_ScanNetworks(int connectionID, byte scanType)

  Summary:
	Scans the network for available SSID.

  Description:
 
  Precondition:

  Parameters:
	connectionID    -- look for a specific profile ID, or look for anything if WF_SCAN_ALL is specified.
    scanType        -- Same as WFSCAN, only we use the MAL definitions in this code; WF_ACTIVE_SCAN or WF_PASSIVE_SCAN
 
  Returns:
	True if the scan started, false otherwise

  Remarks:
    We have to wait for a scan to finish before we can do any other hardware stuff, so this is a harsh call
    in that once connected this can cause problems or the connection can cause problem for this
    That is why once we are conneced, this call will fail, we can't run it.
 ***************************************************************************/
bool WFcK_ScanNetworks(int conID, byte scanType)
{
    UINT8 cpMask = 0;

    DNETcK_Init();
   
    // can not run this once connected.
    // check the WiFi connect as it may be reconnecting and our connect would return FALSE and we 
    // don't want to hose up the reconnect process
    if(WFisConnected())
    {
        return(FALSE);
    }

    // scan all if set to zero
    if(conID == 0)
    {
        conID = WF_SCAN_ALL;
    }

    // make sure this is a profile we have.
    WF_CPGetIds(&cpMask);
    if(conID != WF_SCAN_ALL && (conID == 0 || conID > WF_MAX_NUM_CONNECTION_PROFILES  ||  ((cpMask >> (conID-1)) & 0x1) != 0x1))
    {
        return(FALSE);
    }

    fSetJump = TRUE;
    if(setjmp(jmpbuf))
    {
        // clear all of our scan variables
        // we did not succeed
        gfScanInProgress = TRUE;
        gfScanDone = FALSE;
	    gcScan = 0;

        fSetJump = FALSE;
        return(FALSE);
    }

    gfScanInProgress = TRUE;
    gfScanDone = FALSE;
	gcScan = 0;
	WF_CASetScanType(scanType);
 	WF_Scan(conID);

    fSetJump = FALSE;
    return(TRUE);
}

/*****************************************************************************
  Function:
	BOOL WFcK_IsScanDone(unsigned short * pcNetworks)

  Summary:
	Returns the number of networks found after the requested scan.

  Description:
 
  Precondition:

  Parameters:
	pcNetworks    -- A pointer to recieve the number of networks or SSIDs found
 
  Returns:
	True if the scan done, false if not scaning, or is not done.

  Remarks:
 ***************************************************************************/
BOOL WFcK_IsScanDone(unsigned short * pcNetworks)
{
	if(pcNetworks != NULL) *pcNetworks = gcScan;
	return(gfScanDone);	
}

/*****************************************************************************
  Function:
	bool WFcK_GetScanInfo(int index, SCANINFO * pscanInfo)

  Summary:
	Gets the scan info for the given index into the count of SSIDs found by WFcK_IsScanDone

  Description:
 
  Precondition:

  Parameters:
	index       -- 0 based index and must be less than what WFcK_IsScanDone returned.
    pscanInfo   -- a pointer to SCANINFO structure to recieve the scan data for the SSID indexed
 
  Returns:
	True if the scan info was return, false otherwise; probably index out of range or scan not done.

  Remarks:
    WF_SECURITY_WEP_40 refers to either WEP40 or WEP104 since there is no distinction

 ***************************************************************************/
bool WFcK_GetScanInfo(int index, SCANINFO * pscanInfo)
{
    tWFScanResult scanResult;
    int i = 0;

    if(!WFcK_IsScanDone(NULL) || index >= gcScan)
    {
        return(FALSE);
    }

    fSetJump = TRUE;
    if(setjmp(jmpbuf))
    {
        fSetJump = FALSE;
        return(FALSE);
    }

    memset(pscanInfo, 0, sizeof(SCANINFO));

    WF_ScanGetResult(index, &scanResult);

    memcpy(pscanInfo->szSsid, scanResult.ssid, scanResult.ssidLen);
    pscanInfo->szSsid[scanResult.ssidLen] = 0;

    pscanInfo->channel          = scanResult.channel;
    pscanInfo->signalStrength   = scanResult.rssi;
    pscanInfo->beconPeriod      = scanResult.beaconPeriod;   
    pscanInfo->cBasicRates      = scanResult.numRates;
    pscanInfo->dtimPeriod       = scanResult.DtimPeriod;
    pscanInfo->atimWindow       = scanResult.atimWindow;

    // get the MAC
    memcpy(pscanInfo->ssidMAC, scanResult.bssid, WF_BSSID_LENGTH);
 
    // get all of the supported rates
    for(i=0; i<pscanInfo->cBasicRates; i++)
    {
        pscanInfo->basicRates[i] = 500000 * (0x7F & scanResult.basicRateSet[i]);
    }

    // encryption type, ignore the key type or length; just gets the encryption algorithm 
    if((0x10 & scanResult.apConfig) == 0x00)
    {
        pscanInfo->securityType = WF_SECURITY_OPEN;
    }
    else if((0xC0 & scanResult.apConfig) == 0xC0)
    {
        pscanInfo->securityType = WF_SECURITY_WPA_AUTO_WITH_KEY;
    }
    else if((0x80 & scanResult.apConfig) == 0x80)
    {
        pscanInfo->securityType = WF_SECURITY_WPA2_WITH_KEY;
    }
    else if((0x40 & scanResult.apConfig) == 0x40)
    {
        pscanInfo->securityType = WF_SECURITY_WPA_WITH_KEY;
    }
    else 
    {
        pscanInfo->securityType = WF_SECURITY_WEP_40;
    }
 
    	/*--------------*/
		/* Scan Results */
		/*--------------*/
		//typedef struct
		//{
		//    UINT8      bssid[WF_BSSID_LENGTH]; // Network BSSID value
		//    UINT8      ssid[WF_MAX_SSID_LENGTH]; // Network SSID value
			
			/**
			    Access point configuration
			    <table>
			    Bit 7       Bit 6       Bit 5       Bit 4       Bit 3       Bit 2       Bit 1       Bit 0
			    -----       -----       -----       -----       -----       -----       -----       -----
			    WPA2        WPA         Preamble    Privacy     Reserved    Reserved    Reserved    IE
			    </table>
			      
			    <table>
			    IE        1 if AP broadcasting one or more Information Elements, else 0
			    Privacy   0 : AP is open (no security)
			                1: AP using security,  if neither WPA and WPA2 set then security is WEP.
			    Preamble  0: AP transmitting with short preamble
			                1: AP transmitting with long preamble
			    WPA       Only valid if Privacy is 1.
			                0: AP does not support WPA
			                1: AP supports WPA
			    WPA2      Only valid if Privacy is 1.
			                0: AP does not support WPA2
			                1: AP supports WPA2
			    </table>
			    */
			//UINT8      apConfig;
			//UINT8      reserved;
			//UINT16     beaconPeriod; // Network beacon interval          
			//UINT16     atimWindow; // Only valid if bssType = WF_INFRASTRUCTURE
			
			/**
			    List of Network basic rates.  Each rate has the following format:
			      
				Bit 7
			    * 0 – rate is not part of the basic rates set
			    * 1 – rate is part of the basic rates set
			
				Bits 6:0 
			    Multiple of 500kbps giving the supported rate.  For example, a value of 2 
			    (2 * 500kbps) indicates that 1mbps is a supported rate.  A value of 4 in 
			    this field indicates a 2mbps rate (4 * 500kbps).
			    */
			//UINT8      basicRateSet[WF_MAX_NUM_RATES]; 
			//UINT8      rssi; // Signal strength of received frame beacon or probe response
			//UINT8      numRates; // Number of valid rates in basicRates
			//UINT8      DtimPeriod; // Part of TIM element
			//UINT8      bssType; // WF_INFRASTRUCTURE or WF_ADHOC
			//UINT8      channel; // Channel number
			//UINT8      ssidLen; // Number of valid characters in ssid
			
		//} tWFScanResult; 

    fSetJump = FALSE;
    return(TRUE);
}

/*****************************************************************************
  Function:
	bool WFcK_GetConfigInfo(CONFIGINFO * pConfigInfo)

  Summary:
	Gets the WiFi configuration information currently in use

  Description:
 
  Precondition:

  Parameters:
    pConfigInfo   -- a pointer to CONFIGINFO structure to recieve the configuration data 
  Returns:
	True if the configuration info was return, false otherwise; probably you passed a NULL for the pConfigInfo

  Remarks:
    This method may be poorly implemented at this time. Eventually there will be a corresponding SetConfigInfo
    But for now you would have to copy the WF_Config.h file to your sketch and replace the defaults to change
    this parameters; as thes are currently predefined. By changing these defaults, there may be unexpected results 
    as the DWIFIcK has not be tested with anything else.

 ***************************************************************************/
bool WFcK_GetConfigInfo(CONFIGINFO * pConfigInfo)
{
    tWFCAElements elements;

    if(pConfigInfo == NULL)
    {
        return(FALSE);
    }

    WF_CAGetElements(&elements);

    pConfigInfo->pollingInterval     = elements.listenInterval;
    pConfigInfo->minChannelTime      = elements.minChannelTime;
    pConfigInfo->maxChannelTime      = elements.maxChannelTime;
    pConfigInfo->probeDelay          = elements.probeDelay;
    pConfigInfo->scanType            = elements.scanType;
    pConfigInfo->minSignalStrength   = elements.rssi;
    pConfigInfo->connectRetryCount   = elements.listRetryCount;
    pConfigInfo->beaconTimeout       = elements.beaconTimeout;
    pConfigInfo->scanCount           = elements.scanCount;

    return(TRUE);

    /**
      This parameter is only used when PS Poll mode is enabled.  See 
      WF_PsPollEnable().  Number of 100ms intervals between instances when the 
      MRF24WB0M wakes up to received buffered messages from the network.  Range
      is from 1 (100ms) to 6553.5 sec (~109 min).

      Note that the 802.11 standard defines the listen interval in terms of 
      Beacon Periods, which are typically 100ms.  If the MRF24WB0M is communicating 
      to a network with a network that has Beacon Periods that is not 100ms it 
      will round up (or down) as needed to match the actual Beacon Period as 
      closely as possible.

      Important Note: If the listenInterval is modified while connected to a 
      network the MRF24WB0M will automatically reconnect to the network with the 
      new Beacon Period value.  This may cause a temporary loss of data packets.
      */
    // UINT16  listenInterval;
    /**
      WF_ACTIVE_SCAN (Probe Requests sent out) or WF_PASSIVE_SCAN (listen only)

      Default: WF_ACTIVE_SCAN
      */
    // UINT8   scanType;
    /**
      Specifies RSSI restrictions when connecting.  This field is only used if:
      1.  The Connection Profile has not defined a SSID or BSSID, or  
      2.  An SSID is defined in the Connection Profile and multiple AP’s are discovered with the same SSID.
    
      <table>
        0       Connect to the first network found
        1-254   Only connect to a network if the RSSI is greater than or equal to the specified value.
        255     Connect to the highest RSSI found (default)
      </table>
      */
    // UINT8   rssi;
    /**
      <b>Note: Connection Profile lists are not yet supported.  This array should be set to all FF’s.</b>
      */
    //UINT8   connectionProfileList[WF_CP_LIST_LENGTH];
    /**
      This field is used to specify the number of retries for the single 
      connection profile before taking the connection lost action.

      Range 1 to 254 or WF_RETRY_FOREVER (255)

      Default is 3
      */
    // UINT8   listRetryCount;
    /**
      There are several connection-related events that can occur.  The Host has 
      the option to be notified (or not) when some of these events occur.  This 
      field controls event notification for connection-related events.
      <table>
        Bit 7   Bit 6   Bit 5   Bit 4   Bit 3   Bit 2   Bit 1   Bit 0
        -----   -----   -----   -----   -----   -----   -----   -----
        Not     Not     Not     Event   Event   Event   Event   Event
         used    used    used    E       D       C       B       A
      </table>
      The defines for each bit are shown below.
      <table>
        Event Code  #define
        ----------  -------
        A           WF_NOTIFY_CONNECTION_ATTEMPT_SUCCESSFUL  
        B           WF_NOTIFY_CONNECTION_ATTEMPT_FAILED
        C           WF_NOTIFY_CONNECTION_TEMPORARILY_LOST
        D           WF_NOTIFY_CONNECTION_PERMANENTLY_LOST
        E           WF_NOTIFY_CONNECTION_REESTABLISHED
      </table>
      If a bit is set, then the host will be notified if the associated event 
      occurs.  If the bit is not set then the host will not be notified if the 
      associated event occurs.   A #define, WF_NOTIFY_ALL_EVENTS, exists as a 
      shortcut to allow notification for all events. 

      Note that if an event is not in the above bit mask the application will 
      always be notified of the event via the call to WF_ProcessEvent().

      Default: WF_NOTIFY_ALL_EVENTS
      */
    // UINT8   eventNotificationAction;
    /**
      Specifies the action the Connection Manager should take if a Connection is 
      lost due to a Beacon Timeout.  
      If this field is set to WF_ATTEMPT_TO_RECONNECT then the number of attempts 
      is limited to the value in listRetryCount.

      Choices are:
      WF_ATTEMPT_TO_RECONNECT or WF_DO_NOT_ATTEMPT_TO_RECONNECT

      Default: WF_ATTEMPT_TO_RECONNECT
      */
    // UINT8   beaconTimeoutAction;
    /**
      Designates what action the Connection Manager should take if it receives a 
      Deauthentication message from the AP.  

      If this field is set to WF_ATTEMPT_TO_RECONNECT then the number of attempts 
      is limited to the value in listRetryCount.

      Choices are:
      WF_ATTEMPT_TO_RECONNECT or WF_DO_NOT_ATTEMPT_TO_RECONNECT

      Default: WF_ATTEMPT_TO_RECONNECT
      */
    // UINT8   deauthAction;
    /**
      List of one or more channels that the MRF24WB0M should utilize when 
      connecting or scanning.  If numChannelsInList is set to 0 then this 
      parameter should be set to NULL.

      Default: All valid channels for the regional domain of the MRF24WB0M (set 
      at manufacturing).
    */
    // UINT8   channelList[WF_CHANNEL_LIST_LENGTH];
    /**
      Number of channels in channelList.  If set to 0 then the MRF24WB0M will 
      populate the list with all valid channels for the regional domain.

      Default: The number of valid channels for the regional domain of the 
      MRF24WB0M (set at manufacturing).
      */
    // UINT8   numChannelsInList;
    /**
      Specifies the number of beacons that can be missed before the action 
      described in beaconTimeoutAction is taken.

      <table>
        0       * Not monitor the beacon timeout condition
                 * Will not indicate this condition to Host
        1-255   Beacons missed before disconnect event occurs and beaconTimeoutAction 
                 occurs.  If enabled, host will receive an event message indicating 
                 connection temporarily or permanently lost, and if retrying, a 
                 connection successful event.
      </table>
      Default: 0 (no monitoring or notification of beacon timeout)
      */
    // UINT8   beaconTimeout;
    /**
      The number of times to scan a channel while attempting to find a particular 
      access point.

      Default: 1
      */
    //UINT8   scanCount;
    //UINT8   pad1; 
    /**
      The minimum time (in milliseconds) the connection manager will wait for a 
      probe response after sending a probe request.  If no probe responses are 
      received in minChannelTime then the connection manager will go on to the 
      next channel, if any are left to scan, or quit.

      Default: 200ms
      */
    // UINT16  minChannelTime;
    /**
      If a probe response is received within minChannelTime then the connection 
      manager will continue to collect any additional probe responses up to 
      maxChannelTime before going to the next channel in the channelList.  Units 
      are in milliseconds.

      Default: 400ms
      */
    // UINT16  maxChannelTime;
    /**
      The number of microseconds to delay before transmitting a probe request 
      following the channel change event.

      Default: 20us
    */
    // UINT16  probeDelay;
}

/*==========================================================================*/
/*                                  CALLBACKs                               */
/*==========================================================================*/

/*****************************************************************************
 * FUNCTION: WF_ProcessEvent
 *
 * RETURNS:  None
 *
 * PARAMS:   event      -- event that occurred
 *           eventInfo  -- additional information about the event.  Not all events
 *                         have associated info, in which case this value will be
 *                         set to WF_NO_ADDITIONAL_INFO (0xff)
 *
 *  NOTES:   The Host application must NOT directly call this function.  This 
 *           function is called by the WiFi Driver code when an event occurs
 *           that may need processing by the Host CPU.  
 *
 *           No other WiFi Driver function should be called from this function, with the
 *           exception of WF_ASSERT.  It is recommended that if the application wishes to be 
 *           notified of an event that it simply set a flag and let application code in the 
 *           main loop handle the event.  
 *
 *           WFSetFuncState must be called when entering and leaving this function.  
 *           When WF_ASSERT is enabled this allows a runtime check if any illegal WF functions 
 *           are called from within this function.
 *
 *           For events that the application is not interested in simply leave the
 *           case statement empty.
  *
 *           Customize this function as needed for your application.
 *****************************************************************************/
void WF_ProcessEvent(UINT8 event, UINT16 eventInfo, UINT8 *extraInfo)
{
    #if defined(STACK_USE_UART)
    char buf[8];
    #endif
 
    /* this function tells the WF driver that we are in this function */
    WFSetFuncState(WF_PROCESS_EVENT_FUNC, WF_ENTERING_FUNCTION);
      
    switch (event)
    {
        /*--------------------------------------*/
        case WF_EVENT_CONNECTION_SUCCESSFUL:
        /*--------------------------------------*/  
			gConnState = WF_EVENT_CONNECTION_SUCCESSFUL;
            break;
        
        /*--------------------------------------*/            
        case WF_EVENT_CONNECTION_FAILED:
        /*--------------------------------------*/
            /* eventInfo will contain value from tWFConnectionFailureCodes */
			gConnState = WF_EVENT_CONNECTION_FAILED;
            break; 
            
        /*--------------------------------------*/
        case WF_EVENT_CONNECTION_TEMPORARILY_LOST:
        /*--------------------------------------*/
            /* eventInfo will contain value from tWFConnectionLostCodes */
			gConnState = WF_EVENT_CONNECTION_TEMPORARILY_LOST;
            break;
            
        /*--------------------------------------*/
        case WF_EVENT_CONNECTION_PERMANENTLY_LOST:            
        /*--------------------------------------*/
            /* eventInfo will contain value from tWFConnectionLostCodes */
		    gConnState = WF_EVENT_CONNECTION_PERMANENTLY_LOST;
            break;

        /*--------------------------------------*/    
        case WF_EVENT_CONNECTION_REESTABLISHED:
        /*--------------------------------------*/
			gConnState = WF_EVENT_CONNECTION_REESTABLISHED;
            break;
            
        /*--------------------------------------*/
        case WF_EVENT_SCAN_RESULTS_READY:
        /*--------------------------------------*/  
//            #if defined ( EZ_CONFIG_SCAN )
//            WFScanEventHandler(eventInfo);
//			#endif /* EZ_CONFIG_SCAN */
			gfScanDone = TRUE;
            gfScanInProgress = FALSE;
			gcScan = eventInfo;
            break;
            
        /*--------------------------------------*/                            
        case WF_EVENT_RX_PACKET_RECEIVED:
        /*--------------------------------------*/                        
             break;
            
//        case WF_EVENT_KEY_CALCULATION_COMPLETE:
        default:
            WF_ASSERT(FALSE);  /* unknown event */
            break;
    }        
    
    /* Informs the WF driver that we are leaving this function */
    WFSetFuncState(WF_PROCESS_EVENT_FUNC, WF_LEAVING_FUNCTION);
}    
 
/*
*********************************************************************************************************
*                                   WF_AssertionFailed()
*    
* Description : Called by a WiFi library function when an assert occurs.
*
* Argument(s) : moduleNumber - module number (located in WFApi.h)
*                
*               lineNumber   - line number within module where assert occurred.
*
* Return(s)   : None
*
* Caller(s)   : WF Driver
*
* Notes:      : (1) If the WF_ASSERT macro is enabled (via the WF_DEBUG define in WF_Config.h) this is the 
*                   function that gets called when WF_ASSERT() fails.
*         
*               (2) Customize this function as desired to handle assertion errors
*
*               (3) This is a real pain of a function as I really want an error code, thus the longjmp
*
*********************************************************************************************************
*/
#if defined(WF_DEBUG)
#define WIFI_ASSERT_STRING "WiFi Assert     M:"
void WF_AssertionFailed(UINT8 moduleNumber, UINT16 lineNumber) 
{
    if(fSetJump)
    {
        longjmp(jmpbuf, 1);     // go back to someone who called us so we can return an error.
    }
    else
    {
        while(1);       // ugle if we get here as we have no idea what went wrong without debugging.
    }
}    
#endif /* WF_DEBUG */

