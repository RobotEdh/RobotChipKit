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
#ifndef _DWIFICK_H
#define  _DWIFICK_H

#ifndef _DNETCK_H
    #error DNETcK.h must be included before DWIFIcK.h
#endif

// a nice conditional to switch off of in user sketches
#define USING_WIFI 1

#define WF_MAX_PASS_PHRASE  64

#ifdef __cplusplus

#else
    #define SECURITY int
    #define WFSCAN int
#endif

#ifdef __cplusplus

class DWIFIcK {
private:

public:

    const static int    INVALID_CONNECTION_ID = 0;
    const static int    WF_MAX_SSID_LENGTH = 32;
    const static int    WF_BSSID_LENGTH = 6;
    const static int    WF_MAX_NUM_RATES = 8;
    const static int    WF_SCAN_ALL     = 0xff;

    typedef enum
    {
        WF_INFRASTRUCTURE = 1,
        WF_ADHOC = 2
    } NETWKTYPE;

    typedef enum
    {
        WF_ACTIVE_SCAN  = 1,
        WF_PASSIVE_SCAN = 2,
    } WFSCAN;

    typedef enum
    {
        WF_SECURITY_OPEN                         = 0,
        WF_SECURITY_WEP_40                       = 1,
        WF_SECURITY_WEP_104                      = 2,
        WF_SECURITY_WPA_WITH_KEY                 = 3,
        WF_SECURITY_WPA_WITH_PASS_PHRASE         = 4,
        WF_SECURITY_WPA2_WITH_KEY                = 5,
        WF_SECURITY_WPA2_WITH_PASS_PHRASE        = 6,
        WF_SECURITY_WPA_AUTO_WITH_KEY            = 7,
        WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE    = 8
    } SECURITY;

    typedef enum
    {
        WF_CSTATE_NOT_CONNECTED                 = 1,
	    WF_CSTATE_CONNECTION_IN_PROGRESS        = 2,
	    WF_CSTATE_CONNECTED_INFRASTRUCTURE      = 3,
        WF_CSTATE_CONNECTED_ADHOC               = 4,
	    WF_CSTATE_RECONNECTION_IN_PROGRESS      = 5,
	    WF_CSTATE_CONNECTION_PERMANENTLY_LOST   = 6
    } CONNECTSTATE;

#endif  // __cplusplus
 
    // WPA and WPA2 KEY, used for both
    typedef struct
    {
        byte rgbKey[32];
    } WPA2KEY;

    typedef struct
    {
        union
        {
            byte rgbKey[5];
            char asciiKey[5];
        } key[4];
    } WEP40KEY;

    typedef struct
    {
        union
        {
            byte rgbKey[13];
            char asciiKey[13];
        } key[4];
    } WEP104KEY;

    typedef struct
    {
        char            szSsid[WF_MAX_SSID_LENGTH];
        SECURITY        securityType;
        byte            channel;
        byte            signalStrength;
        byte            cBasicRates;
        byte            dtimPeriod;
        unsigned short  atimWindow;
        unsigned short  beconPeriod;
        unsigned int    basicRates[WF_MAX_NUM_RATES];
        byte            ssidMAC[WF_BSSID_LENGTH];
    } SCANINFO;

    typedef struct
    {
        SECURITY        securityType;
        struct
        {
            int index;
            int cbKey;
            union
            {
                WPA2KEY      wpa;
                WEP40KEY    wep40;
                WEP104KEY   wep104;
                char        szPassPhrase[WF_MAX_PASS_PHRASE];
                byte        rgbKey[1];
           };
        } key;
    } SECINFO;

typedef struct 
{
    unsigned int    pollingInterval;
    unsigned int    minChannelTime;
    unsigned int    maxChannelTime;
    unsigned int    probeDelay;
    WFSCAN          scanType;
    byte            minSignalStrength;
    byte            connectRetryCount;
    byte            beaconTimeout;
    byte            scanCount;
} CONFIGINFO;

#ifdef __cplusplus

    static int connect(int connectionID);                                                                                           // use a profile
    static int connect(int connectionID, DNETcK::STATUS * pStatus);                                                                 // use a profile
    static int connect(const char * szSsid);                                                                                        // no security
    static int connect(const char * szSsid, DNETcK::STATUS * pStatus);                                                              // no security
    static int connect(const char * szSsid, WEP40KEY& keySet, int iKey);                                                            // WEP 40
    static int connect(const char * szSsid, WEP40KEY& keySet, int iKey, DNETcK::STATUS * pStatus);                                  // WEP 40
    static int connect(const char * szSsid, WEP104KEY& keySet, int iKey);                                                           // WEP 104   
    static int connect(const char * szSsid, WEP104KEY& keySet, int iKey, DNETcK::STATUS * pStatus);                                 // WEP 104   
    static int connect(const char * szSsid, const char * szPassPhrase);                                                             // WPA2
    static int connect(const char * szSsid, const char * szPassPhrase, DNETcK::STATUS * pStatus);                                   // WPA2
    static int connect(const char * szSsid, WPA2KEY& key);                                                                           // WPA2
    static int connect(const char * szSsid, WPA2KEY& key, DNETcK::STATUS * pStatus);                                                 // WPA2
    static int connect(SECURITY security, const char * szSsid, const char * szPassPhrase);                                          // explicit
    static int connect(SECURITY security, const char * szSsid, const char * szPassPhrase, DNETcK::STATUS * pStatus);                // explicit
    static int connect(SECURITY security, const char * szSsid, const byte * rgbKey, int cbKey, int iKey);                           // explicit
    static int connect(SECURITY security, const char * szSsid, const byte * rgbKey, int cbKey, int iKey, DNETcK::STATUS * pStatus); // explicit

    static bool isConnected(int connectionID);
    static bool isConnected(int connectionID, unsigned long msBlockMax);
    static bool isConnected(int connectionID, DNETcK::STATUS * pStatus);
    static bool isConnected(int connectionID, unsigned long msBlockMax, DNETcK::STATUS * pStatus);

    static void disconnect(int connectionID);
    static bool getSecurityInfo(int connectionID, SECINFO * psecInfo);

    static bool beginScan(void);
    static bool beginScan(int connectionID);
    static bool beginScan(WFSCAN scanType);
    static bool beginScan(int connectionID, WFSCAN scanType);

    static bool isScanDone(int * cNetworks);
    static bool isScanDone(int * cNetworks, unsigned long msBlockMax);
    static bool isScanDone(int * cNetworks, DNETcK::STATUS * pStatus);
    static bool isScanDone(int * cNetworks, unsigned long msBlockMax, DNETcK::STATUS * pStatus);

    static bool getScanInfo(int iNetwork, SCANINFO * pscanInfo);
    static bool getConfigInfo(CONFIGINFO * pConfigInfo);
};

#endif  // __cplusplus

#endif  //  _DWIFICK_H
