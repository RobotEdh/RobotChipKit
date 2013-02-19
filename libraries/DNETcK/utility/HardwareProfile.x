/************************************************************************/
/*																		*/
/*	LAN HardwareProfile.x                                               */
/*																		*/
/*	This will be included by HardwareProfile.h only if DWIFIcK is not	*/
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
/*	5/2/2012(KeithV): Created											*/
/*																		*/
/************************************************************************/

#ifndef LAN_HARDWARE_PROFILE_X
#define LAN_HARDWARE_PROFILE_X
#include "Compiler.h"

// Network hardware
#define DNETcK_LAN_Network
#include <NetworkProfile.x>
#include <CommonHardwareProfile.h>

#if defined(DWIFIcK_WiFi_Hardware)
    #error Incompatible WiFi hardware library included, specify LAN hardware.
#elif !defined(DNETcK_LAN_Hardware)
    #error A network hardware library must be included.
#endif

#endif // LAN_HARDWARE_PROFILE_X
