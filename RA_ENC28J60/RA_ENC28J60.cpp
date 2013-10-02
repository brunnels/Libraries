/*
 * Copyright 2013 Reef Angel / Benjamin Runnels
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <Globals.h>
#ifdef ETH_ENC28J60
#include "RA_ENC28J60.h"
#include <SPI.h>
#include <RA_Wifi.h>
#include <UIPEthernet.h>
#include <UIPServer.h>
#include <UIPClient.h>
#include <avr/wdt.h>

RA_ENC28J60::RA_ENC28J60()
{
  PortalConnection=false;
  PortalTimeOut=millis();
}

void RA_ENC28J60::Init()
{
  UIPEthernet.begin(NetMac, NetIP, NetDNS, NetGateway, NetSubnet); // Start Ethernet with DHCP polling enabled
  NetServer.begin();
//  Serial.print("My IP address: ");
//  for (byte thisByte = 0; thisByte < 4; thisByte++) {
//    // print the value of each byte of the IP address:
//    Serial.print(EthernetDHCP.ipAddress()[thisByte], DEC);
//    Serial.print(".");
//  }
//  Serial.println();

}

void RA_ENC28J60::Update()
{
//  EthernetDHCP.poll();
  if (NetClient.available() && PortalConnection)
  {
    while(NetClient.available())
    {
      wdt_reset();
      char c = NetClient.read();
    }
  }

  // if the server's disconnected, stop the client
  if (!NetClient.connected() && PortalConnection)
  {
    PortalConnection=false;
    NetClient.stop();
  }

  // if request timed out, stop the client
  if (NetClient.connected() && PortalConnection && millis()-PortalTimeOut>PORTAL_TIMEOUT)
  {
    PortalConnection=false;
    NetClient.stop();
  }
}

void RA_ENC28J60::ReceiveData()
{
  NetClient=NetServer.available();
  if (NetClient)
  {
    while (NetClient.connected())
    {
#if defined WDT || defined WDT_FORCE
      wdt_reset();
#endif  // defined WDT || defined WDT_FORCE
      if ( NetClient.available() > 0 ) ProcessEthernet();
    }
  }
//  if (RelayClient.available())
//  {
//    while (RelayClient.connected())
//    {
//#if defined WDT || defined WDT_FORCE
//      wdt_reset();
//#endif  // defined WDT || defined WDT_FORCE
//      if (RelayClient.available()>54) // length of the return header of the HTTP upgrade
//        RelayClient.find("\r\n\r\n"); // Discard header
//      else
//        RelayClient.read(); // Most likely HTTP/1.1 204, so we read one byte to cause timeout
//      ProcessRelayEthernet();
//    }
//  }
}

void RA_ENC28J60::ProcessEthernet()
{
  bIncoming=true;
  timeout=millis();
  while (bIncoming)
  {
    if (millis()-timeout>100)
    {
      bIncoming=false;
      NetClient.stop();
    }
    if (NetClient.available()>0)
    {
      PushBuffer(NetClient.read());
      timeout=millis();
#if defined WDT || defined WDT_FORCE
      wdt_reset();
#endif  // defined WDT || defined WDT_FORCE
      if (reqtype>0 && reqtype<128)
      {
        bIncoming=false;
        while(NetClient.available())
        {
#if defined WDT || defined WDT_FORCE
          wdt_reset();
#endif  // defined WDT || defined WDT_FORCE
          NetClient.read();
        }
      }
    }
  }
#if defined WDT || defined WDT_FORCE
  wdt_reset();
#endif  // defined WDT || defined WDT_FORCE
  ProcessHTTP();

  NetClient.flush();
  NetClient.stop();
  m_pushbackindex=0;
}
#endif  // ETH_ENC28J60
