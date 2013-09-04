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

#ifndef __RA_WIFI_H__
#define __RA_WIFI_H__

#include <Globals.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <RA_NET.h>

class RA_Wifi: public RA_NET
{
  public:
    RA_Wifi(HardwareSerial &wifiSerial);
    void ProcessSerial();
    void ReceiveData();
    inline size_t write(uint8_t c) { return _wifiSerial->write((uint8_t)c); }
    inline size_t write(unsigned long n) { return _wifiSerial->write((uint8_t)n); }
    inline size_t write(long n) { return _wifiSerial->write((uint8_t)n); }
    inline size_t write(unsigned int n) { return _wifiSerial->write((uint8_t)n); }
    inline size_t write(int n) { return _wifiSerial->write((uint8_t)n); }

  protected:
    HardwareSerial* _wifiSerial;
    void _init();
};

#endif  // __RA_WIFI_H__
