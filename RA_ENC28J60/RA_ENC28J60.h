/*
 * Copyright 2010 Reef Angel / Roberto Imai
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

/*
 * Updated by:  Curt Binder
 * Updates Released under Apache License, Version 2.0
 */

#ifndef __RA_ENC28J60_H__
#define __RA_ENC28J60_H__

#ifdef RA_ENC28J60
//#include <SPI.h>
#include <RA_Wifi.h>
#include <UIPEthernet.h>
#include <UIPServer.h>
#include <UIPClient.h>
#include <avr/wdt.h>

static UIPServer NetServer(2000);
static byte NetMac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static IPAddress NetIP(10,0,0,38);
static IPAddress NetGateway(10,0,0,254);
static IPAddress NetSubnet(255,255,255,0);
static IPAddress NetDNS(10,0,0,254);
const IPAddress PortalServer(198,171,134,6);
static UIPClient NetClient;

#define PORTAL_TIMEOUT  10000

class RA_ENC28J60  : public RA_Wifi
{
  public:
    RA_ENC28J60();
    void Init();
    void ReceiveData();
    void ProcessEthernet();
    void Update();
    boolean PortalConnection;
    unsigned long PortalTimeOut;


  protected:
    size_t write(uint8_t c);
    size_t write(unsigned long n);
    size_t write(long n);
    size_t write(unsigned int n);
    size_t write(int n);


};
#endif  // RA_ENC28J60

#endif  // __RA_ENC28J60_H__
