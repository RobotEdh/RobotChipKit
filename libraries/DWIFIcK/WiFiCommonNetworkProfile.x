/************************************************************************/
/*																		*/
/*	WiFiCommonNetworkProfile.x                                          */
/*																		*/
/*	Network Hardware vector file for the MRF24WB0MA PmodWiFi            */
/*																		*/
/*	This defines the WiFi INT and SPI settings   						*/
/*	This file is good for both the MRF24WB and G modules				*/
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
/*	11/9/2012(KeithV): Created											*/
/*																		*/
/************************************************************************/

#ifndef PMODWIFI_INT_SPI_NETWORK_PROFILE_X
#define PMODWIFI_INT_SPI_NETWORK_PROFILE_X

// puts three item together in one macro name a##b##c
#define CAT2_3_THINGS(a,b) a##b
#define CAT1_3_THINGS(a,b,c) CAT2_3_THINGS(a##b,c)
#define CAT_3_THINGS(a,b,c) CAT1_3_THINGS(a,b,c)

#define MAKE3_BITS_INT_REF(a,b) a.b
#define MAKE2_BITS_INT_REF(x,y,z) MAKE3_BITS_INT_REF(x##bits,y##z)
#define MAKE1_BITS_INT_REF(a,b,c,d) MAKE2_BITS_INT_REF(a##b,INT##c,d)
#define MAKE_BITS_INT_REF(a,b,c,d) MAKE1_BITS_INT_REF(a,b,c,d)

// What MRF24 interrupt are we using?
#if defined(MRF24_USING_INT0)
    #error INT0 is not supported as an MRF interrupt
#elif defined(MRF24_USING_INT1)
    #define MRF24_INT_NBR   1
#elif defined(MRF24_USING_INT2)
    #define MRF24_INT_NBR   2
#elif defined(MRF24_USING_INT3)
    #define MRF24_INT_NBR   3
#elif defined(MRF24_USING_INT4)
    #define MRF24_INT_NBR   4
#else
    #error unsupported MRF24 interrupt
#endif
#define MRF24_INT_IRQ   CAT_3_THINGS(_EXTERNAL_,MRF24_INT_NBR,_IRQ)         // _EXTERNAL_x_IRQ
#define MRF24_INT_VEC   CAT_3_THINGS(_EXTERNAL_,MRF24_INT_NBR,_VECTOR)      // _EXTERNAL_x_VECTOR

// calculate the IE and IF register number
#if (MRF24_INT_IRQ < 32)
    #define  MRF24_INT_IE_IF_REG_NBR 0
#elif (MRF24_INT_IRQ < 64)
    #define  MRF24_INT_IE_IF_REG_NBR 1
#elif (MRF24_INT_IRQ < 96)
    #define  MRF24_INT_IE_IF_REG_NBR 2
#else
    #error unsupported external INT IE/IF register
#endif

// calculate the IPC register number
#if (MRF24_INT_VEC < 4)
    #define  MRF24_INT_IPC_REG_NBR 0
#elif (MRF24_INT_VEC < 8)
    #define  MRF24_INT_IPC_REG_NBR 1
#elif (MRF24_INT_VEC < 12)
    #define  MRF24_INT_IPC_REG_NBR 2
#elif (MRF24_INT_VEC < 16)
    #define  MRF24_INT_IPC_REG_NBR 3
#elif (MRF24_INT_VEC < 20)
    #define  MRF24_INT_IPC_REG_NBR 4
#elif (MRF24_INT_VEC < 24)
    #define  MRF24_INT_IPC_REG_NBR 5
#elif (MRF24_INT_VEC < 28)
    #define  MRF24_INT_IPC_REG_NBR 6
#elif (MRF24_INT_VEC < 32)
    #define  MRF24_INT_IPC_REG_NBR 7
#elif (MRF24_INT_VEC < 36)
    #define  MRF24_INT_IPC_REG_NBR 8
#elif (MRF24_INT_VEC < 40)
    #define  MRF24_INT_IPC_REG_NBR 9
#elif (MRF24_INT_VEC < 44)
    #define  MRF24_INT_IPC_REG_NBR 10
#elif (MRF24_INT_VEC < 48)
    #define  MRF24_INT_IPC_REG_NBR 11
#elif (MRF24_INT_VEC < 52)
    #define  MRF24_INT_IPC_REG_NBR 12
#elif (MRF24_INT_VEC < 56)
    #define  MRF24_INT_IPC_REG_NBR 13
#elif (MRF24_INT_VEC < 60)
    #define  MRF24_INT_IPC_REG_NBR 14
#elif (MRF24_INT_VEC < 64)
    #define  MRF24_INT_IPC_REG_NBR 15
#else
    #error unsupported IPC register
#endif

	#define WF_INT_EDGE			MAKE_BITS_INT_REF(INT,CON,MRF24_INT_NBR,EP)                             // INTCONbits.INTiEP
	#define WF_INT_IE			MAKE_BITS_INT_REF(IEC,MRF24_INT_IE_IF_REG_NBR,MRF24_INT_NBR,IE)         // IECxbits.INTiIE
	#define WF_INT_IF			MAKE_BITS_INT_REF(IFS,MRF24_INT_IE_IF_REG_NBR,MRF24_INT_NBR,IF)         // IFSxbits.INTiIF
	#define WF_INT_BIT			(1 << (MRF24_INT_IRQ % 32))	                                            // bit position for the IE/IF
	#define WF_INT_IPCSET		CAT_3_THINGS(IPC,MRF24_INT_IPC_REG_NBR,SET)                             // IPCySET
	#define WF_INT_IPCCLR		CAT_3_THINGS(IPC,MRF24_INT_IPC_REG_NBR,CLR)                             // IPCyCLR

    // common SPI interrupt stuff; careful only good for INT 1,2,3,4
    #define WF_INT_IE_CLEAR		CAT_3_THINGS(IEC,MRF24_INT_IE_IF_REG_NBR,CLR)                           // IECxCLR
	#define WF_INT_IF_CLEAR		CAT_3_THINGS(IFS,MRF24_INT_IE_IF_REG_NBR,CLR)                           // IFSxCLR
	#define WF_INT_IE_SET		CAT_3_THINGS(IEC,MRF24_INT_IE_IF_REG_NBR,SET)                           // IECxSET
	#define WF_INT_IF_SET		CAT_3_THINGS(IFS,MRF24_INT_IE_IF_REG_NBR,SET)                           // IFSxSET

	#define WF_INT_IPC_MASK		(0x1F << (8 * (MRF24_INT_VEC % 4)))
	#define WF_INT_IPC_VALUE    (0x0C << (8 * (MRF24_INT_VEC % 4)))                                     // Priority 3

#if defined(MRF24_USING_SPI1)
    #define MRF24_SPI 1
#elif defined(MRF24_USING_SPI2)
    #define MRF24_SPI 2
#elif defined(MRF24_USING_SPI3)
    #define MRF24_SPI 3
#elif defined(MRF24_USING_SPI4)
    #define MRF24_SPI 4
#else
    #error unsupported SPI port
#endif

    #define _SPI_IRQ            CAT_3_THINGS(_SPI,MRF24_SPI,_ERR_IRQ)   // _SPIx_ERR_IRQ 
	#define WF_SSPBUF			CAT_3_THINGS(SPI,MRF24_SPI,BUF)         // SPIxBUF
	#define WF_SPISTAT			CAT_3_THINGS(SPI,MRF24_SPI,STAT)        // SPIxSTAT 
	#define WF_SPISTATbits		CAT_3_THINGS(SPI,MRF24_SPI,STATbits)    // SPIxSTATbits 
	#define WF_SPICON1			CAT_3_THINGS(SPI,MRF24_SPI,CON)         // SPIxCON
	#define WF_SPICON1bits		CAT_3_THINGS(SPI,MRF24_SPI,CONbits)     // SPIxCONbits
	#define WF_SPI_BRG			CAT_3_THINGS(SPI,MRF24_SPI,BRG)         // SPIxBRG

#if (_SPI_IRQ < 32)
	#define WF_SPI_IE_CLEAR		IEC0CLR
	#define WF_SPI_IF_CLEAR		IFS01CLR
#elif (_SPI_IRQ < 64) 
	#define WF_SPI_IE_CLEAR		IEC1CLR
	#define WF_SPI_IF_CLEAR		IFS1CLR
#elif (_SPI_IRQ < 96) 
	#define WF_SPI_IE_CLEAR		IEC2CLR
	#define WF_SPI_IF_CLEAR		IFS2CLR
#else
    #error unsupported SPI IRQ
#endif

	#define WF_SPI_INT_BITS		(0x7 << (_SPI_IRQ % 32))  

#endif // PMODWIFI_INT_SPI_NETWORK_PROFILE_X
