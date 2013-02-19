/******************************************************************************

 MRF24WB0M Driver Customization
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24WB0M WiFi controller
  -Reference: MRF24WB0M Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:		WF_Config.h
 Dependencies:	TCP/IP Stack header files
 Processor:		PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 Compiler:		Microchip C32 v1.10b or higher
				Microchip C30 v3.22 or higher
				Microchip C18 v3.34 or higher
 Company:		Microchip Technology, Inc.

 Software License Agreement

 Copyright (C) 2002-2010 Microchip Technology Inc.  All rights reserved.

 Microchip licenses to you the right to use, modify, copy, and distribute:
 (i)  the Software when embedded on a Microchip microcontroller or digital 
      signal controller product ("Device") which is integrated into 
      Licensee's product; or
 (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
      ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device used in 
	  conjunction with a Microchip ethernet controller for the sole purpose 
	  of interfacing with the ethernet controller.

 You should refer to the license agreement accompanying this Software for 
 additional information regarding your rights and obligations.

 THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 NON-INFRINGEMENT. IN NO EVENT SHALL MICROCHIP BE LIABLE FOR ANY INCIDENTAL,
 SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST
 OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS BY
 THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS
 FOR INDEMNITY OR CONTRIBUTION, OR OTHER SIMILAR COSTS, WHETHER ASSERTED ON
 THE BASIS OF CONTRACT, TORT (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR
 OTHERWISE.


 Author				Date		Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 KH                 27 Jan 2010 Created for MRF24WB0M
******************************************************************************/


#ifndef __WF_CONFIG_H_
#define __WF_CONFIG_H_

/*
*********************************************************************************************************
*                                           DEFINES                               
*********************************************************************************************************
*/

/*----------------------------------------------------------------------------*/
/* This block of defines allows for code and data reduction by removing       */
/* WiFi driver code and or data that is not needed by the application.        */
/* Comment out those function blocks that are not needed.                     */
/*----------------------------------------------------------------------------*/
#define WF_USE_SCAN_FUNCTIONS
#define WF_USE_TX_POWER_CONTROL_FUNCTIONS
#define WF_USE_POWER_SAVE_FUNCTIONS
#define WF_USE_MULTICAST_FUNCTIONS
#define WF_USE_INDIVIDUAL_SET_GETS
#define WF_USE_GROUP_SET_GETS
//#define WF_USE_DATA_TX_RX_FUNCTIONS
//#define USE_GRATUITOUS_ARP



/*= WF_ASSERT MACRO ==================================================================*/
/* Customize how the WiFi driver assert macro (WF_ASSERT) should operate.             */
/*  To DISABLE the WF_ASSERT macro: Comment out '#define WF_DEBUG'                    */
/*  To ENABLE the WF_ASSERT macro:  Unomment out '#define WF_DEBUG'                   */
/*====================================================================================*/
#define WF_DEBUG 

/*= WF_CONSOLE =======================================================================*/
/* Customize whether the WiFi Driver supports a serial console application            */
/*  To DISABLE the serial console application: Comment out '#define WF_CONSOLE'       */
/*  To ENABLE the serial console application: Uncomment out '#define WF_CONSOLE'      */
/*====================================================================================*/
//#define WF_CONSOLE              /* needed for console demo */
//#define WF_CONSOLE_IFCFGUTIL    /* needed for console demo */


/*--------------------------------------------*/
/* Default settings for Connection Management */
/*--------------------------------------------*/
#define MY_DEFAULT_SSID_NAME                "chipKIT"


#define MY_DEFAULT_NETWORK_TYPE             WF_INFRASTRUCTURE   /* WF_INFRASTRUCTURE or WF_ADHOC     */

#define MY_DEFAULT_SCAN_TYPE                WF_PASSIVE_SCAN      /* WF_ACTIVE_SCAN or WF_PASSIVE_SCAN */

#define MY_DEFAULT_CHANNEL_LIST             {}            /* use {} to scan all channels       */

#define MY_DEFAULT_LIST_RETRY_COUNT         (5u)

#define MY_DEFAULT_EVENT_NOTIFICATION_LIST  (WF_NOTIFY_CONNECTION_ATTEMPT_SUCCESSFUL  |         \
                                             WF_NOTIFY_CONNECTION_ATTEMPT_FAILED      |         \
                                             WF_NOTIFY_CONNECTION_TEMPORARILY_LOST    |         \
                                             WF_NOTIFY_CONNECTION_PERMANENTLY_LOST    |         \
                                             WF_NOTIFY_CONNECTION_REESTABLISHED)

#define MY_DEFAULT_PS_POLL                   WF_DISABLED         /* WF_DISABLED or WF_ENABLED */

#define MY_DEFAULT_WIFI_SECURITY_MODE        WF_SECURITY_WPA2_WITH_KEY

//#define USE_MRF24W_HOST_BUFFER

//#define STACK_USE_EZ_CONFIG
//#define EZ_CONFIG_SCAN
//#define EZ_CONFIG_STALL
//#define EZ_CONFIG_STORE




/*****************************************************************************/
/*****************************************************************************/
/*                             WIFI SECURITY COMPILE-TIME DEFAULTS           */
/*****************************************************************************/
/*****************************************************************************/
// Security modes available on WiFi network:
//   WF_SECURITY_OPEN                      : No security
//   WF_SECURITY_WEP_40                    : WEP Encryption using 40 bit keys
//   WF_SECURITY_WEP_104                   : WEP Encryption using 104 bit keys
//   WF_SECURITY_WPA_WITH_KEY              : WPA-PSK Personal where binary key is given to MRF24WB0M 
//   WF_SECURITY_WPA_WITH_PASS_PHRASE      : WPA-PSK Personal where passphrase is given to MRF24WB0M and it calculates the binary key
//   WF_SECURITY_WPA2_WITH_KEY             : WPA2-PSK Personal where binary key is given to MRF24WB0M 
//   WF_SECURITY_WPA2_WITH_PASS_PHRASE     : WPA2-PSK Personal where passphrase is given to MRF24WB0M and it calculates the binary key
//   WF_SECURITY_WPA_AUTO_WITH_KEY         : WPA-PSK Personal or WPA2-PSK Personal where binary key is given and MRF24WB0M will 
//                                             connect at highest level AP supports (WPA or WPA2)                                                
//   WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE : WPA-PSK Personal or WPA2-PSK Personal where passphrase is given to MRF24WB0M and it 
//                                             calculates the binary key and connects at highest level AP supports (WPA or WPA2)

// Default pass phrase used for WF_SECURITY_WPA_WITH_PASS_PHRASE and 
// WF_SECURITY_WPA2_WITH_PASS_PHRASE security modes
#define MY_DEFAULT_PSK_PHRASE               "Digilent"


// If using security mode of WF_SECURITY_WPA_WITH_KEY or WF_SECURITY_WPA2_WITH_KEY, then this section 
// must be set to  match the key for MY_DEFAULT_SSID_NAME and MY_DEFAULT_PSK_PHRASE
// combination.  The values below are derived from the SSID "MicrochipDemoAP" and the pass phrase
// "Microchip 802.11 Secret PSK Password".
// The tool at http://www.wireshark.org/tools/wpa-psk.html can be used to generate this field. 
#define MY_DEFAULT_PSK "\
\x27\x2C\x89\xCC\xE9\x56\x31\x1E\
\x3B\xAD\x79\xF7\x1D\xC4\xB9\x05\
\x7A\x34\x4C\x3E\xB5\xFA\x38\xC2\
\x0F\x0A\xB0\x90\xDC\x62\xAD\x58"


//-----------------------------------------------------------------------------------
// Default WEP keys used in WF_SECURITY_WEP_40  and WF_SECURITY_WEP_104 security mode
//-----------------------------------------------------------------------------------
#define MY_DEFAULT_WEP_PHRASE           "Digilent"

// string 4 40-bit WEP keys -- corresponding to passphraseof "WEP Phrase"
#define MY_DEFAULT_WEP_KEYS_40 "\
\xBE\xC9\x58\x06\x97\
\xC1\x04\x49\xFD\x4E\
\x43\x18\x2B\x33\x88\
\xB0\x73\x69\xF4\x78"

// string containing 4 104-bit WEP keys -- corresponding to passphraseof "WEP Phrase"
#define MY_DEFAULT_WEP_KEYS_104 "\
\x3E\xCD\x30\xB2\x55\x2D\x3C\x50\x52\x71\xE8\x83\x91\
\x77\x4A\x69\x45\xA4\x3D\x66\x63\xFE\x5B\x1D\xB9\xFD\
\x82\x29\x87\x4C\x9B\xDC\x6D\xDF\x87\xD1\xCF\x17\x41\
\xCC\xD7\x62\xDE\x92\xAD\xBA\x3B\x62\x2F\x7F\xBE\xFB"


/* Valid Key Index: 0, 1, 2, 3  */
#define MY_DEFAULT_WEP_KEY_INDEX        (0u)


#endif /* __WF_CONFIG_H_ */


