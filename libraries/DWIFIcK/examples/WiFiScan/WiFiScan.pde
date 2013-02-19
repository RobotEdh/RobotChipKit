/************************************************************************/
/*                                                                      */
/*      WiFiScan                                                        */
/*                                                                      */
/*      Scans the network for all available routers                     */
/*      and prints their parameters to the Serial Monitor               */
/*                                                                      */
/************************************************************************/
/*      Author:       Keith Vogel                                       */
/*      Copyright 2012, Digilent Inc.                                   */
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
/*                                                                      */
/*      This library is explicity targeting the chipKIT                 */
/*      software compatible product line                                */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*      2/1/2012(KeithV): Created                                       */
/*      11/13/2012(KeithV): Modified to be generic for all HW libraries */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/*              Include ONLY 1 hardware library that matches            */
/*              the network hardware you are using                      */
/*                                                                      */
/*              Refer to the hardware library header file               */
/*              for supported boards and hardware configurations        */
/*                                                                      */
/************************************************************************/
// #include <WiFiShieldOrPmodWiFi.h>                       // This is for the MRF24WBxx on a pmodWiFi or WiFiShield
#include <WiFiShieldOrPmodWiFi_G.h>                     // This is for the MRF24WGxx on a pmodWiFi or WiFiShield

/************************************************************************/
/*                    Required libraries, Do NOT comment out            */
/************************************************************************/
#include <DNETcK.h>
#include <DWIFIcK.h>

typedef enum
{
    NONE = 0,
    WAITFORSCAN,
    PRINTAPINFO,
    ERROR,
    STOP,
    DONE
} STATE;

STATE state = WAITFORSCAN;

unsigned tStart = 0;
unsigned tWait = 5000;
unsigned long t = 0;

DNETcK::STATUS status = DNETcK::None;
int  cNetworks = 0;
int iNetwork = 0;

void setup() {

    Serial.begin(9600);
    Serial.println("Start of Sketch");
    Serial.println("WiFiScan 1.0");
    Serial.println("Digilent, Copyright 2012");
    Serial.println("");

    // Set my default wait time to nothing
    DNETcK::setDefaultBlockTime(DNETcK::msImmediate); 

    // start a scan
    DWIFIcK::beginScan();
}

void loop() 
{
    switch(state)
    {
        case WAITFORSCAN:
            if(DWIFIcK::isScanDone(&cNetworks, &status))
            {
                Serial.println("Scan Done");
                state = PRINTAPINFO;
            }
            else if(DNETcK::isStatusAnError(status))
            {
                Serial.print("Scan Failed");
                state = ERROR;
            }
            break;

        case PRINTAPINFO:
            if(iNetwork < cNetworks)
            {
                DWIFIcK::SCANINFO scanInfo;
                int j = 0;

                if(DWIFIcK::getScanInfo(iNetwork, &scanInfo))
                {
                    Serial.println("");
                    Serial.print("Scan info for index: ");
                    Serial.println(iNetwork, DEC);

                    Serial.print("SSID: ");
                    Serial.println(scanInfo.szSsid);

                    Serial.print("Secuity type: ");
                    Serial.println(scanInfo.securityType, DEC);

                    Serial.print("Channel: ");
                    Serial.println(scanInfo.channel, DEC);    

                    Serial.print("Signal Strength: ");
                    Serial.println(scanInfo.signalStrength, DEC);    

                    Serial.print("Count of supported bit rates: ");
                    Serial.println(scanInfo.cBasicRates, DEC);    

                    for( j= 0; j< scanInfo.cBasicRates; j++)
                    {
                        Serial.print("\tSupported Rate: ");
                        Serial.print(scanInfo.basicRates[j], DEC); 
                        Serial.println(" bps");
                    }

                    Serial.print("SSID MAC: ");
                    for(j=0; j<sizeof(scanInfo.ssidMAC); j++)
                    {
                        if(scanInfo.ssidMAC[j] < 16)
                        {
                            Serial.print(0, HEX);
                        }
                        Serial.print(scanInfo.ssidMAC[j], HEX);
                    }
                    Serial.println("");

                    Serial.print("Beacon Period: ");
                    Serial.println(scanInfo.beconPeriod, DEC);    

                    Serial.print("dtimPeriod: ");
                    Serial.println(scanInfo.dtimPeriod, DEC);    

                    Serial.print("atimWindow: ");
                    Serial.println(scanInfo.atimWindow, DEC); 
                }
                else
                {
                    Serial.print("Unable to get scan info for iNetwork: ");
                    Serial.println(iNetwork, DEC);
                }

                iNetwork++;
            }
            else
            {
                state = STOP;
            }
            break;

        case ERROR:
            Serial.print("Status value: ");
            Serial.println(status, DEC);
            state = STOP;
            break;

        case STOP:
            Serial.println("End of Sketch");
            state = DONE;
            break;

        case DONE:
        default:
            break;

    }

    DNETcK::periodicTasks();
}


