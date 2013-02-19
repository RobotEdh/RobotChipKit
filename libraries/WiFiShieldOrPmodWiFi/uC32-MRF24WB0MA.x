/************************************************************************/
/*																		*/
/*	uC32-MRF24WB0MA.x                                                  */
/*																		*/
/*	MRF24WB0MA WiFi interrupt and SPI configuration file 				*/
/*	Specific to the Uno32 WiFiShield                                    */
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
/*	1/25/2012(KeithV): Created											*/
/*	5/2/2012(KeithV): Updated to support a WiFiShield					*/
/*																		*/
/************************************************************************/

#ifndef UC32_MRF24WB0MA_X
#define UC32_MRF24WB0MA_X

    // Digilent defined values for the MLA build
    #define __Digilent_Build__

    // Use connector JC on the Pmod Shield, INT 1, SPI2
    #define MRF24_USING_SPI2
	#define MRF24_USING_INT1

	#define WF_CS_TRIS			(TRISGbits.TRISG9)
	#define WF_CS_IO			(LATGbits.LATG9)
	#define WF_SDI_TRIS			(TRISGbits.TRISG7)
	#define WF_SCK_TRIS			(TRISGbits.TRISG6)
	#define WF_SDO_TRIS			(TRISGbits.TRISG8)
	#define WF_RESET_TRIS		(TRISDbits.TRISD6)
	#define WF_RESET_IO			(LATDbits.LATD6)
	#define WF_INT_TRIS			(TRISDbits.TRISD8)  // INT1
	#define WF_INT_IO			(PORTDbits.RD8)
	#define WF_HIBERNATE_TRIS	(TRISDbits.TRISD5)
	#define WF_HIBERNATE_IO		(PORTDbits.RD5)

    static inline void __attribute__((always_inline)) DNETcKInitNetworkHardware(void)
    {
        WF_HIBERNATE_IO     = 0;			
        WF_HIBERNATE_TRIS   = 0;

        WF_RESET_IO         = 0;		
        WF_RESET_TRIS       = 0;

        // Enable the WiFi
        WF_CS_IO            = 1;
        WF_CS_TRIS          = 0;

        WF_INT_TRIS         = 1;
    }

#endif // UC32_MRF24WB0MA_X
