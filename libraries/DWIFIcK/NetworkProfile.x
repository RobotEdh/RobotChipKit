/************************************************************************/
/*																		*/
/*	NetworkProfile.x                                                    */
/*																		*/
/*	Network Hardware vector file for the MRF24WB0MA PmodWiFi            */
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

#ifndef PMODWIFI_NETWORK_PROFILE_X
#define PMODWIFI_NETWORK_PROFILE_X

#define DWIFIcK_WiFi_Hardware
#define DNETcK_MRF24WB0MA

// board specific stuff
#if defined(_BOARD_UNO_)

    #include <Uno32-MRF24WB0MA.x>

#elif defined(_BOARD_UC32_) || defined(_BOARD_CEREBOT_MX3CK_512_)

    #include <uC32-MRF24WB0MA.x>

#elif defined (_BOARD_MEGA_)

    #include <Max32-MRF24WB0MA.x>

#elif defined (_BOARD_CEREBOT_MX3CK_)

    #include <MX3cK-MRF24WB0MA.x>

#elif defined (_BOARD_CEREBOT_MX4CK_)

    #include <MX4cK-MRF24WB0MA.x>

#elif defined (_BOARD_CEREBOT_MX7CK_)

    #include <MX7cK-MRF24WB0MA.x>

#elif defined (_BOARD_FUBARINO_SD_)

    #include <FubarSD-MRF24WB0MA.x>

#elif defined (_BOARD_CMOD_CK1_)

    #include <CmodCK1-MRF24WB0MA.x>

#else

    #error Neither the WiFi Shield nor PmodWiFi is supported by this board.

#endif

// set up the WiFi INT and SPI port
#include <WiFiCommonNetworkProfile.x>

#endif // PMODWIFI_NETWORK_PROFILE_X
