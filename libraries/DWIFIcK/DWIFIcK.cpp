/************************************************************************/
/*																		*/
/*	DWIFIcK.cpp     The Digilent WIFI static class For the chipKIT      */
/*                  software compatible product line. This includes the */
/*					Arduino compatible chipKIT boards as well as  	    */
/*					the Digilent Cerebot cK boards.						*/
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
/*	This the Static DWIFIcK Class implementation file     				*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	1/31/2012(KeithV): Created											*/
/*																		*/
/************************************************************************/
#include "../DNETcK/DNETcK.h"
#include "DWIFIcK.h"
#include "../DNETcK/utility/DNETcKAPI.h"
#include "./utility/DWIFIcKAPI.h"

/*****************************************************************************
  Function:
    int DWIFIcK::connect(int connectionID)
    int DWIFIcK::connect(int connectionID, DNETcK::STATUS * pStatus)
    int DWIFIcK::connect(const char * szSsid)
    int DWIFIcK::connect(const char * szSsid, DNETcK::STATUS * pStatus)
    int DWIFIcK::connect(const char * szSsid, WEP40KEY& keySet, int iKey)
    int DWIFIcK::connect(const char * szSsid, WEP40KEY& keySet, int iKey, DNETcK::STATUS * pStatus)
    int DWIFIcK::connect(const char * szSsid, WEP104KEY& keySet, int iKey)
    int DWIFIcK::connect(const char * szSsid, WEP104KEY& keySet, int iKey, DNETcK::STATUS * pStatus)
    int DWIFIcK::connect(const char * szSsid, const char * szPassPhrase)
    int DWIFIcK::connect(const char * szSsid, const char * szPassPhrase, DNETcK::STATUS * pStatus)
    int DWIFIcK::connect(const char * szSsid, WPA2KEY& key)
    int DWIFIcK::connect(const char * szSsid, WPA2KEY& key, DNETcK::STATUS * pStatus)
    int DWIFIcK::connect(SECURITY security, const char * szSsid, const char * szPassPhrase)
    int DWIFIcK::connect(SECURITY security, const char * szSsid, const char * szPassPhrase, DNETcK::STATUS * pStatus)
    int DWIFIcK::connect(SECURITY security, const char * szSsid, const byte * rgbKey, int cbKey, int iKey)
    int DWIFIcK::connect(SECURITY security, const char * szSsid, const byte * rgbKey, int cbKey, int iKey, DNETcK::STATUS * pStatus)

  Description:
	Sets up a connection to the WiFi AP

  Parameters:
    connectionID    - The connection ID or Profile already stored on the MRF24WB0M to use. If 0 is specified the default parameters as in WF_Config.h are used.
    security        - One of the security key types as defined in the DWIFIcK::SECURITY enum type.
    szSsid          - A zero terminate string of the WiFi SSID to connect to.
    rgbKey          - an array of bytes that contains the key information. This may have characters or a string if appropriate for the sercurity type used.
    cbKey           - The number of bytes in rgbKey.
    keySet          - The 4 WEP keysets, if WEP40 or WEP104 is used.
    iKey            - 0 based index into the WEP key set to specify the key to use
    key             - The WPA(2) key (32 bytes).
    szPassPhrase    - The WPA(2) passphrase if a key is not used
    pStatus         - A point to a DNETcK::STATUS to return the status of the connect process.
  
  Returns:
	The connection/Profile ID that is in use. If 0 is returned the connection process failed.

  Remarks:
    This method returns immediately and the connection process is started. Some form of isConnected should be called to determine if the connection is made
    If the WPA(2) key must be calculated, this process will start and will typically take 30 seconds to complete.

 ***************************************************************************/
int DWIFIcK::connect(int connectionID)
{
    return(connect(connectionID, NULL));
}
int DWIFIcK::connect(int connectionID, DNETcK::STATUS * pStatus)
{
    return(WFcK_Connect_By_ProfileID(connectionID, pStatus));
}
int DWIFIcK::connect(const char * szSsid)
{
    return(connect(WF_SECURITY_OPEN, szSsid, NULL, 0, 0, NULL));
}
int DWIFIcK::connect(const char * szSsid, DNETcK::STATUS * pStatus)
{
    return(connect(WF_SECURITY_OPEN, szSsid, NULL, 0, 0, pStatus));
}
int DWIFIcK::connect(const char * szSsid, WEP40KEY& keySet, int iKey)
{
    return(connect(WF_SECURITY_WEP_40, szSsid, keySet.key[0].rgbKey, sizeof(keySet), iKey, NULL));
}
int DWIFIcK::connect(const char * szSsid, WEP40KEY& keySet, int iKey, DNETcK::STATUS * pStatus)
{
    return(connect(WF_SECURITY_WEP_40, szSsid, keySet.key[0].rgbKey, sizeof(keySet), iKey, pStatus));
}
int DWIFIcK::connect(const char * szSsid, WEP104KEY& keySet, int iKey)
{
    return(connect(WF_SECURITY_WEP_104, szSsid, keySet.key[0].rgbKey, sizeof(keySet), iKey, NULL));
}
int DWIFIcK::connect(const char * szSsid, WEP104KEY& keySet, int iKey, DNETcK::STATUS * pStatus)
{
    return(connect(WF_SECURITY_WEP_104, szSsid, keySet.key[0].rgbKey, sizeof(keySet), iKey, pStatus));
}
int DWIFIcK::connect(const char * szSsid, const char * szPassPhrase)
{
    return(connect(WF_SECURITY_WPA2_WITH_PASS_PHRASE, szSsid, szPassPhrase, NULL));
}
int DWIFIcK::connect(const char * szSsid, const char * szPassPhrase, DNETcK::STATUS * pStatus)
{
    return(connect(WF_SECURITY_WPA2_WITH_PASS_PHRASE, szSsid, szPassPhrase, pStatus));
}
int DWIFIcK::connect(const char * szSsid, WPA2KEY& key)
{
    return(connect(WF_SECURITY_WPA2_WITH_KEY, szSsid, key.rgbKey, sizeof(key), 0, NULL));
}
int DWIFIcK::connect(const char * szSsid, WPA2KEY& key, DNETcK::STATUS * pStatus)
{
    return(connect(WF_SECURITY_WPA2_WITH_KEY, szSsid, key.rgbKey, sizeof(key), 0, pStatus));
}
int DWIFIcK::connect(SECURITY security, const char * szSsid, const char * szPassPhrase)
{
    return(connect(security, szSsid, (const byte *) szPassPhrase, strlen(szPassPhrase), 0, NULL));
}
int DWIFIcK::connect(SECURITY security, const char * szSsid, const char * szPassPhrase, DNETcK::STATUS * pStatus)
{
    return(connect(security, szSsid, (const byte *) szPassPhrase, strlen(szPassPhrase), 0, pStatus));
}
int DWIFIcK::connect(SECURITY security, const char * szSsid, const byte * rgbKey, int cbKey, int iKey)
{
    return(connect(security, szSsid, rgbKey, cbKey, iKey, NULL));
}
int DWIFIcK::connect(SECURITY security, const char * szSsid, const byte * rgbKey, int cbKey, int iKey, DNETcK::STATUS * pStatus)
{
    return(WFcK_Connect((byte) security, szSsid, rgbKey, cbKey, iKey, pStatus));
}

/*****************************************************************************
  Function:
    bool DWIFIcK::isConnected(int connectionID)
    bool DWIFIcK::isConnected(int connectionID, unsigned long msBlockMax)
    bool DWIFIcK::isConnected(int connectionID, DNETcK::STATUS * pStatus)
    bool DWIFIcK::isConnected(int connectionID, unsigned long msBlockMax, DNETcK::STATUS * pStatus)

  Description:
	Checks to see if a connection is actively made to the AP (router).

  Parameters:
    connectionID    - The connection ID or Profile already stored on the MRF24WB0M to use. If 0 is specified the default parameters as in WF_Config.h are used.
    msBlockMax      - The amount of time to wait before returning without completion. If not specified, the value as set by setDefaultBlockTime() is used.
    pStatus         - A point to a DNETcK::STATUS to return the status of the connection process.
  
  Returns:
	True if the connection is made, false if it is still in progress or an error occured.

  Remarks:

 ***************************************************************************/
bool DWIFIcK::isConnected(int connectionID)
{
    return(isConnected(connectionID, DNETcK::_msDefaultTimeout, NULL));
}
bool DWIFIcK::isConnected(int connectionID, unsigned long msBlockMax)
{
    return(isConnected(connectionID, msBlockMax, NULL));
}
bool DWIFIcK::isConnected(int connectionID, DNETcK::STATUS * pStatus)
{
    return(isConnected(connectionID, DNETcK::_msDefaultTimeout, pStatus));
}
bool DWIFIcK::isConnected(int connectionID, unsigned long msBlockMax, DNETcK::STATUS * pStatus)
{
    unsigned long tStart = 0;
    unsigned short cNet = 0;
    unsigned int conState = 0;

    // if it is connected, determine that quickly and exit
    if(WFcK_IsConnected(connectionID, pStatus))
    {
        return(true);
    }

    // loop until connected, or the timeout
    tStart = millis();
    do {
            // in order to know what connection state we have
            // we must run the stack tasks
            EthernetPeriodicTasks();

            if(WFcK_IsConnected(connectionID, pStatus))
            {
                return(true);
            }
  
    } while(!hasTimeElapsed(tStart, msBlockMax, millis()));

    return(false);
}

/*****************************************************************************
  Function:
    void DWIFIcK::disconnect(int conID)
 
  Description:
	Drops the connection to the AP

  Parameters:
    conID    - Disconnects the WiFi and frees the connection profile if it is an in memory profile
  
  Returns:
	NONE

  Remarks:

 ***************************************************************************/
void DWIFIcK::disconnect(int conID)
{
    WFcK_Disconnect(conID);
}

/*****************************************************************************
  Function:
    bool DWIFIcK::getSecurityInfo(int connectionID, SECINFO * psecInfo)
 
  Description:
	Gets security/key information about the specified connection ID / Profile

  Parameters:
    connectionID    - Get the security info (key/passphrase) for the connection profile. Specify 0 for the actively connected connection
    psecInfo        - a pointer to a SECINFO to recieve the security info
  
  Returns:
	True if the security info is returned, false if not.. usually becasue an invalid connectionID was given.

  Remarks:

 ***************************************************************************/
bool DWIFIcK::getSecurityInfo(int connectionID, SECINFO * psecInfo)
{
    return(WFcK_GetSecurityInfo(connectionID, psecInfo));
}


/*****************************************************************************
  Function:
    bool DWIFIcK::beginScan(void)
    bool DWIFIcK::beginScan(WFSCAN scanType)
    bool DWIFIcK::beginScan(int connectionID)
    bool DWIFIcK::beginScan(int connectionID, WFSCAN scanType)
 
  Description:
	Scan for a WiFi SSID based on profile or connection ID.

  Parameters:
    connectionID    - Scan for the network defined by the specifed profile, or all networks if 0/WF_SCAN_ALL is specified or omitted
    scanType        - WF_ACTIVE_SCAN or WF_PASSIVE_SCAN or WF_PASSIVE_SCAN if omitted
  
  Returns:
	True if the the scan got started, false if not.. usually becasue an invalid connectionID was given.

  Remarks:
    We have to wait for a scan to finish before we can do any other hardware stuff, so this is a harsh call
    in that once connected this can cause problems or the connection can cause problem for this
    That is why once we are conneced, this call will fail, we can't run it.

 ***************************************************************************/
bool DWIFIcK::beginScan(void)
{
    return(beginScan(WF_SCAN_ALL, WF_PASSIVE_SCAN));
}
bool DWIFIcK::beginScan(WFSCAN scanType)
{
    return(beginScan(WF_SCAN_ALL, scanType));
}
bool DWIFIcK::beginScan(int connectionID)
{
    return(beginScan(connectionID, WF_PASSIVE_SCAN));
}
bool DWIFIcK::beginScan(int connectionID, WFSCAN scanType)
{
    return(WFcK_ScanNetworks(connectionID, scanType));
}


/*****************************************************************************
  Function:
    bool DWIFIcK::isScanDone(int * pcNetworks)
    bool DWIFIcK::isScanDone(int * pcNetworks, unsigned long msBlockMax)
    bool DWIFIcK::isScanDone(int * pcNetworks, DNETcK::STATUS * pStatus)
    bool DWIFIcK::isScanDone(int * pcNetworks, unsigned long msBlockMax, DNETcK::STATUS * pStatus)
 
  Description:
	Checks to see if scanning for networks is done.

  Parameters:
    pcNetworks      - a pointer to an int to recieve the number of networks found.
    msBlockMax      - The amount of time to wait before returning without completion. If not specified, the value as set by setDefaultBlockTime() is used.
    pStatus         - A point to a DNETcK::STATUS to return the status of the connection process.
 
  Returns:
	True if the scan is done, false if not.. check the status for a hard error.

  Remarks:
    This call can be made at anytime after the scan has started or completed. Even aftera connection is made.
 ***************************************************************************/
bool DWIFIcK::isScanDone(int * pcNetworks)
{
    return(isScanDone(pcNetworks, DNETcK::_msDefaultTimeout, NULL));
}
bool DWIFIcK::isScanDone(int * pcNetworks, unsigned long msBlockMax)
{
    return(isScanDone(pcNetworks, msBlockMax, NULL));
}
bool DWIFIcK::isScanDone(int * pcNetworks, DNETcK::STATUS * pStatus)
{
    return(isScanDone(pcNetworks, DNETcK::_msDefaultTimeout, pStatus));
}
bool DWIFIcK::isScanDone(int * pcNetworks, unsigned long msBlockMax, DNETcK::STATUS * pStatus)
{
    unsigned long tStart = 0;
    unsigned short cNet = 0;

    // just to be nice
    if(pcNetworks!= NULL) *pcNetworks = 0;

    // loop until connected, or the timeout
    tStart = millis();
    do {
            // in order to know what connection state we have
            // we must run the stack tasks
            EthernetPeriodicTasks();

            if(WFcK_IsScanDone(&cNet))
            {
                if(pcNetworks!= NULL) *pcNetworks = cNet;
                if(pStatus != NULL) *pStatus = DNETcK::Success;
                return(true);
            }
  
    } while(!hasTimeElapsed(tStart, msBlockMax, millis()));

    if(pStatus != NULL) *pStatus = DNETcK::WFStillScanning;
    return(false);
}

/*****************************************************************************
  Function:
    bool DWIFIcK::getScanInfo(int iNetwork, SCANINFO * pscanInfo)

    Description:
	    Returns the the AP (router) info for the specifed network scanned.

  Parameters:
    iNetwork      - a zero based index that is less than the number of networks returned by isScanDone.
    pscanInfo     - A point to a SCANINFO to return the AP (router) information.

  Returns:
	True if the AP information is returned, false if there is no information or the scan is not done.

  Remarks:
    This may be called at any time after isScanDone succeeds
 ***************************************************************************/
bool DWIFIcK::getScanInfo(int iNetwork, SCANINFO * pscanInfo)
{
    return(WFcK_GetScanInfo(iNetwork, pscanInfo));
}

bool  DWIFIcK::getConfigInfo(CONFIGINFO * pConfigInfo)
{
    return(WFcK_GetConfigInfo(pConfigInfo));
}
