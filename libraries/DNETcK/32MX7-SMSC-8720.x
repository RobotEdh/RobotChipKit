/************************************************************************/
/*																		*/
/*	MAX32-SMSC-8720.x                                                   */
/*																		*/
/*	Configure the hardware for the chipKIT 8720 LAN Network PHY			*/
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
/*	5/1/2012(KeithV): Created											*/
/*																		*/
/************************************************************************/

#ifndef _32MX7_SMSC_8720_X
#define _32MX7_SMSC_8720_X

// Define a macro describing this hardware set up (used in other files)
#define PIC32_ENET_SK_DM320004

// External National PHY configuration
#define	PHY_RMII				// external PHY runs in RMII mode
// #define	PHY_CONFIG_ALTERNATE	// alternate configuration used
#define	PHY_ADDRESS			0x0	// the address of the National DP83848 PHY

#define DNETcKInitNetworkHardware() {TRISEbits.TRISE9 = 0;LATEbits.LATE9 = 1;}

#endif  //  _32MX7_SMSC_8720_X