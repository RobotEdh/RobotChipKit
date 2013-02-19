/************************************************************************/
/*																		*/
/*	TCPIPConfig.x                                                       */
/*																		*/
/*	Default TCPIP config for specific WiFi board configurations.   	    */
/*	This will only include WiFi hardware that physically is mounted		*/
/*	on the board. If the board has no WiFi network hardware, the		*/
/*	board will not be specified in this file							*/
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

#ifndef DEFAULT_WIFI_TCPIPConfig_X
#define DEFAULT_WIFI_TCPIPConfig_X

// board specific stuff
#error No default WiFi network hardware for this board; include a WiFi network hardware libarary.

#endif // DEFAULT_WIFI_TCPIPConfig_X
