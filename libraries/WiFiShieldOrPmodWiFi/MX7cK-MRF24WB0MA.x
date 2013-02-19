/************************************************************************/
/*																		*/
/*	MX7cK-MRF24WB0MA.x                                                  */
/*																		*/
/*	MRF24WB0MA WiFi interrupt and SPI configuration file 				*/
/*	Specific to the MX7cK                                               */
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
/*																		*/
/************************************************************************/

#ifndef MX7CK_MRF24WB0MA_X
#define MX7CK_MRF24WB0MA_X

    // we need to slow it down for the MX1/2
    #define __Digilent_Build__

    //  JF, SPI4 or SPI3A, INT2
    #define MRF24_USING_SPI4
	#define MRF24_USING_INT2

    // set the Pmod connector JF bits
	#define WF_CS_TRIS			(TRISFbits.TRISF12) // Make sure JP-1 Jumper is correct
	#define WF_CS_IO			(LATFbits.LATF12)
	#define WF_SDI_TRIS			(TRISFbits.TRISF4)
	#define WF_SCK_TRIS			(TRISFbits.TRISF13) // Make sure JP-2 Jumper is correct
	#define WF_SDO_TRIS			(TRISFbits.TRISF5)
	#define WF_RESET_TRIS		(TRISAbits.TRISA1)
	#define WF_RESET_IO			(LATAbits.LATA1)
	#define WF_INT_TRIS			(TRISEbits.TRISE9)  // INT2
	#define WF_INT_IO			(PORTEbits.RE9)
	#define WF_HIBERNATE_TRIS	(TRISAbits.TRISA5)
	#define WF_HIBERNATE_IO		(PORTAbits.RA5)

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

#endif // MX7CK_MRF24WB0MA_X
