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

#include <Globals.h>
#include <EtherCard.h>
#if defined WDT || defined WDT_FORCE
#include <avr/wdt.h>
#endif  // defined WDT || defined WDT_FORCE

class RA_ENC28J60
{
  public:
    RA_ENC28J60();
    void init();
    void SendXMLData(BufferFiller& buf, bool fAtoLog = false);
    void processHTTP();
    void FillHeader(int s, byte type);
    void pushbuffer(byte data);
    void ModeResponse(BufferFiller& buf, bool fOk);
    void PrintHeader(BufferFiller& buf, int s, byte type);
    void WebResponse (BufferFiller& buf, const prog_char *response, long strsize);
    BufferFiller buf;
};

#endif  // __RA_ENC28J60_H__
