/************************************************************************/
/*                                                                      */
/*    WiFiShieldOrPmodWiFi.h                                            */
/*                                                                      */
/*    PmodWiFi library include file to be used with DNETck and DWIFIcK  */
/*                                                                      */
/*    Supports:                                                         */
/*                                                                      */
/*  Cmod CK1 with pmodeWiFi on connector J1                             */
/*  Uno32 JP5&7 on Master with WiFi Shield                              */
/*  Uno32 JP5&7 on Master with PmodShield and PmodWiFi on connector JC  */
/*  uC32 JP5&7 on Master with WiFi Shield                               */
/*  uC32 JP5&7 on Master with PmodShield and PmodWiFi on connector JC   */
/*  Max32 JP3&4 on Master with WiFi Shield                              */
/*  Max32 JP3&4 on Master with PmodShield and PmodWiFi on connector JC  */
/*  MX3cK JP6&8 on Master with PmodWiFi on connector JE                 */
/*  MX3cK-512 JP6&8 on Master with PmodWiFi on connector JE             */
/*  MX4ck JP3 on INT3 with PmodWiFi on connector JB                     */
/*  MX7ck with PmodWiFi on connector JF                                 */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2011, Digilent Inc.                                     */
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
/*  Revision History:                                                   */
/*                                                                      */
/*    4/26/2012(KeithV): Created                                        */
/*                                                                      */
/************************************************************************/
#ifndef PMODWIFI_H
#define PMODWIFI_H

#if defined(DNETcK_Network_Hardware)
    #error Multiple network hardwares are included, select only one.
#elif defined(_DNETCK_H)
    #error DNETcK.h must be included after PmodWiFi.h.
#elif defined( _DWIFICK_H)
    #error DWIFIcK.h must be included after PmodWiFi.h.
#endif

#define DNETcK_Network_Hardware
#define DWIFIcK_PmodWiFi

#endif // PMODWIFI_H