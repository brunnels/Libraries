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

#include "RA_Wifi.h"

RA_Wifi::RA_Wifi(HardwareSerial &wifiSerial) : RA_NET()
{
  _wifiSerial = &wifiSerial;
  _init();
}

void RA_Wifi::_init()
{
  RA_NET::_init();
  _wifiSerial->begin(57600);
  //println("init");
}

void RA_Wifi::ProcessSerial()
{
  bIncoming=true;
  timeout=millis();
  while (bIncoming)
  {
    if (millis()-timeout>100)
    {
      bIncoming=false;
      //for (int a=0;a<32;a++) pushbuffer(0);
    }
    if (_wifiSerial->available() > 0)
    {
      //PushBuffer(_wifiSerial->read());
      _wifiSerial->print(_wifiSerial->read());
      timeout=millis();
#if defined WDT || defined WDT_FORCE
      wdt_reset();
#endif  // defined WDT || defined WDT_FORCE
    }
  }
  //ProcessHTTP();

  _wifiSerial->flush();
  m_pushbackindex=0;
}

void RA_Wifi::ReceiveData()
{
  if ( _wifiSerial->available() > 0 ) ProcessSerial();
}
