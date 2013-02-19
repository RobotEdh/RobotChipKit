/************************************************************************/
/*																		*/
/*	WiFi HardwareProfile.x                                              */
/*																		*/
/*	This will be included by HardwareProfile.h only if DWIFIcK is    	*/
/*	on the compiler -I include path first.								*/
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
/*  Revision History:													*/
/*																		*/
/*	4/26/2012(KeithV): Created											*/
/*																		*/
/************************************************************************/

#ifndef WIFI_HARDWARE_PROFILE_X
#define WIFI_HARDWARE_PROFILE_X
#include "Compiler.h"

#define WF_MAX_SPI_FREQ		(10000000ul)	// Hz

// Network hardware
#define DWIFIcK_WiFi_Network
#include <NetworkProfile.x>
#include <CommonHardwareProfile.h>

#if defined(DNETcK_LAN_Hardware)
    #error Incompatible LAN hardware library included, specify WiFi hardware.
#elif !defined(DWIFIcK_WiFi_Hardware)
    #error A network hardware library must be included.
#endif

#endif // WIFI_HARDWARE_PROFILE_X
