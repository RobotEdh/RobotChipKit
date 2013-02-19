/************************************************************************/
/*																		*/
/*	DNETcKAPI.h	--  DNETcK interface APIs to implement the              */
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
#ifndef _DWIFICKAPI_H
#define _DWIFICKAPI_H

#ifdef __cplusplus
extern "C"
{
 
    // typically these are MAL core C APIs where I fool with paramters to make them available in C++
    // I don't need prototypes in C as the MAL header files provide them.
    void WF_CPDelete(byte CpId);
    void WF_CMDisconnect(void);

    // BE VERY CAREFUL, while byte arrays are passed as IP address, they must be machine aligned or a processor alignment 
    // fault will occur. Address of IPv4 are passed as IPv4 is properly aligned.

    // initalization routines. The MAL is very bad at initializing it static memory
    // in there int routines, I have added this to init the static/global variables
    // to the "just booted" state. This allows end() and begin() to work.
    bool WFcK_GetScanInfo(int index, DWIFIcK::SCANINFO * pscanInfo);
    bool WFcK_GetSecurityInfo(int connectionID, DWIFIcK::SECINFO * psecInfo);
    bool WFcK_IsConnected(int connectionID, DNETcK::STATUS * pStatus);
    int WFcK_Connect(byte secrityMode, const char * szSsid, const byte * rgbKey, int cbKey, int iKey, DNETcK::STATUS * pStatus);
    bool WFcK_ScanNetworks(int connectionID, DWIFIcK::WFSCAN scanType);
    bool WFcK_Connect_By_ProfileID(int conID, DNETcK::STATUS * pStatus);
    bool WFcK_GetConfigInfo(DWIFIcK::CONFIGINFO * pConfigInfo);

#else
    bool WFcK_GetScanInfo(int index, SCANINFO * pscanInfo);
    bool WFcK_GetSecurityInfo(int connectionID, SECINFO * psecInfo);
    bool WFcK_IsConnected(int connectionID, STATUS * pStatus);
    int WFcK_Connect(byte secrityMode, const char * szSsid, const byte * rgbKey, int cbKey, int iKey, STATUS * pStatus);
    bool WFcK_ScanNetworks(int connectionID, byte scanType);
    bool WFcK_Connect_By_ProfileID(int conID, STATUS * pStatus);
    bool WFcK_GetConfigInfo(CONFIGINFO * pConfigInfo);


#endif

    void InitWiFiAppConfig(void);
    void DWIFIcK_Init(void);
    bool WFcK_IsScanDone(unsigned short * pcNetworks);
    void WFcK_Disconnect(int conID);

#ifdef __cplusplus
}
#endif


#define BAUD_RATE       (19200)		// bps

#endif // _DWIFICKAPI_H