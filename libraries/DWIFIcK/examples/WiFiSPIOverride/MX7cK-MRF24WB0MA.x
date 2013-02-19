/************************************************************************/
/*																		*/
/*	MX7cK-MRF24WB0MA.h                                                  */
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

#ifndef MX7CK_MRF24WB0MA_PROFILE_H
#define MX7CK_MRF24WB0MA_PROFILE_H

    #define __Digilent_Build__

    //  JE, SPI3 or SPI1A, INT1
    #define MRF24_USING_SPI3
	#define MRF24_USING_INT1

    // set the Pmod connector JE bits
	#define WF_CS_TRIS			(TRISDbits.TRISD14) 
	#define WF_CS_IO			(LATDbits.LATD14)
	#define WF_SDI_TRIS			(TRISFbits.TRISF2)
	#define WF_SCK_TRIS			(TRISDbits.TRISD15) 
	#define WF_SDO_TRIS			(TRISFbits.TRISF8)
	#define WF_RESET_TRIS		(TRISAbits.TRISA7)
	#define WF_RESET_IO			(LATAbits.LATA7)
	#define WF_INT_TRIS			(TRISEbits.TRISE8)  // INT1
	#define WF_INT_IO			(PORTEbits.RE8)
	#define WF_HIBERNATE_TRIS	(TRISAbits.TRISA10)
	#define WF_HIBERNATE_IO		(PORTAbits.RA10)

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

#endif // MX7CK_MRF24WB0MA_PROFILE_H
