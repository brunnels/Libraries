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


#include "RA_ENC28J60.h"
#include <Globals.h>
#include <RA_Wifi.h>
#include <ReefAngel.h>


byte Ethernet::buffer[500];

RA_ENC28J60::RA_ENC28J60(void)
{
  m_pushbackindex=0;
  reqtype=0;
}

void RA_ENC28J60::init()
{
  if (ether.begin(500, ethMac, ethSpiSlavePin) == 0)
  {
    Serial.println( "Failed to access Ethernet controller");
  }
#if STATIC_IP
  ether.staticSetup(myip, gwip);
#else
  if (!ether.dhcpSetup())
  {
    Serial.println("DHCP failed");
  }
#endif // STATIC_IP

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);
}

void RA_ENC28J60::ModeResponse(BufferFiller& buf, bool fOk)
{
//  uint8_t s;
//  if ( fOk )
//    s = 15;
//  else
//    s = 16;
//  PrintHeader(buf, s, 1);
//  buf.print(XML_MODE_OPEN);
//  if ( fOk )
//    buf.print(XML_OK);
//  else
//    buf.print(XML_ERR);
//  buf.print(XML_MODE_CLOSE);
//  ether.httpServerReply(buf.position());
}

void RA_ENC28J60::PrintHeader(BufferFiller& buf, int s, byte type)
{
//  buf.print(SERVER_HEADER1);
//  if (type)
//    buf.print("xml");
//  else
//    buf.print("html");
//  buf.print(SERVER_HEADER2);
//  buf.print(s, DEC);
//  buf.print("\r\n\r\n");
}

void RA_ENC28J60::pushbuffer(byte inStr)
{
  m_pushback[m_pushbackindex]=inStr;
    m_pushback[m_pushbackindex+1]=0;
    if (reqtype>0 && reqtype<128)
    {
      if (authStr[m_pushbackindex]==inStr) m_pushbackindex++; else m_pushbackindex=0;
      if (authStr[m_pushbackindex]==0) auth=true;
      //if (m_pushbackindex>0) Serial.println(m_pushbackindex,DEC);
      //if (m_pushbackindex>0) Serial.println(test,DEC);
    }
    else
    {
      m_pushbackindex++;
      if (m_pushbackindex==32) m_pushbackindex=0;
      if (reqtype>128)
      {
          if (inStr==' ')
          {
              reqtype=256-reqtype;
             if ( (reqtype == REQ_M_BYTE) || (reqtype == REQ_M_INT) || (reqtype == REQ_M_RAW || reqtype == REQ_OVERRIDE) )
              {
                // must have a comma to have second value
                // verify that the last char was a digit
                if ( isdigit(m_pushback[m_pushbackindex-2]) )
                {
                  // check for the comma to determine how we proceed
                  if ( bCommaCount )
                  {
                    bHasSecondValue = true;
                  }
                  else
                  {
                    bHasSecondValue = false;
                    weboption2 = weboption;
                  }
                }
              }
              if ( reqtype == REQ_DATE )
              {
                // last char must be a digit
            if ( isdigit(m_pushback[m_pushbackindex-2]) )
            {
              // comma count must be 2 otherwise it's an error
              // if not, set weboption to -1 to signify an error
              if ( bCommaCount != 2 )
              {
                weboption = -1;
              }
            }
            else
            {
              // last digit not a char and no commas means we need to
              // send the current date/time of the controller
              if ( bCommaCount == 0 )
              {
                weboption = -2;
              }
              else
              {
                weboption = -1;
              }
            }
              }
          }
          else if (inStr == ',')
          {
            // when we hit a comma, copy the first value (weboption) to weboption2
            // then skip over the comma and put the value to be written into weboption
            // second comma copies the value into weboption3
          bCommaCount++;
          if ( bCommaCount == 1 )
            weboption2 = weboption;
          else if ( bCommaCount == 2 )
            weboption3 = weboption;
            // reset weboption to 0
            weboption = 0;
          }
          else if (inStr == '-')
          {
            webnegoption=true;
          }
          else if(isdigit(inStr))
          {
            // process digits here
          weboption*=10;
          weboption+=inStr-'0';
          }
          // 3/14/11 - curt
          //else all other chars are discarded
          // consider further sanity checks to ensure that we don't get any malformed strings or commands
          // right now, we can embed non digits in the parameters and as long as the last char is a digit,
          // it is ok and the non digits are just skipped over
          // we may want to signal an error or break out of processing the string to indicate there is an error
          // maybe set weboption2 and weboption to -1 or 0
          // could also flush the buffer and set reqtype to a REQ_ERROR or something
          // need to give this more thought
          //
          // NOTES about too large of value being stored
          //   if you exceed the storage limit for the variable, the negative value gets stored
          //   so we should limit the value being saved from the client side
          //   otherwise we would have to do additional checks in here for the size and that would
          //   require more code
          //   Users shouldn't be manually changing values from the web without an interface that
          //   limits them, so we "should" be safe (in theory), but this may need to be revisited
          //   in the future.  - curt (3/14/11)
      }
      else
      {
              if (strncmp("GET / ", m_pushback, 6)==0) reqtype = REQ_ROOT;
              else if (strncmp("GET /wifi", m_pushback, 9)==0) reqtype = REQ_WIFI;
              else if (strncmp("GET /r", m_pushback, 6)==0) reqtype = -REQ_RELAY;
              else if (strncmp("GET /mb", m_pushback, 7)==0) { reqtype = -REQ_M_BYTE; weboption2 = -1; bHasSecondValue = false; bCommaCount = 0; }
              else if (strncmp("GET /mi", m_pushback, 7)==0) { reqtype = -REQ_M_INT; weboption2 = -1; bHasSecondValue = false; bCommaCount = 0; }
              else if (strncmp("GET /ma", m_pushback, 7)==0) reqtype = -REQ_M_ALL;
              else if (strncmp("GET /mr", m_pushback, 7)==0) { reqtype = -REQ_M_RAW; weboption2 = -1; bHasSecondValue = false; bCommaCount = 0; }
              else if (strncmp("GET /v", m_pushback, 6)==0) reqtype = -REQ_VERSION;
              else if (strncmp("GET /d", m_pushback, 6)==0) { reqtype = -REQ_DATE; weboption2 = -1; weboption3 = -1; bCommaCount = 0; }
              else if (strncmp("HTTP/1.", m_pushback, 7)==0) reqtype = -REQ_HTTP;
              else if (strncmp("GET /sr", m_pushback, 7)==0) reqtype = -REQ_R_STATUS;
              else if (strncmp("GET /sa", m_pushback, 7)==0) reqtype = -REQ_RA_STATUS;
              else if (strncmp("GET /bp", m_pushback, 7)==0) reqtype = -REQ_BTN_PRESS;
              else if (strncmp("GET /mf", m_pushback, 7)==0) reqtype = -REQ_FEEDING;
              else if (strncmp("GET /mw", m_pushback, 7)==0) reqtype = -REQ_WATER;
  //            else if (strncmp("GET /cr", m_pushback, 7)==0) reqtype = -REQ_CAL_RELOAD;
              else if (strncmp("GET /mt", m_pushback, 7)==0) reqtype = -REQ_ALARM_ATO;
              else if (strncmp("GET /mo", m_pushback, 7)==0) reqtype = -REQ_ALARM_OVERHEAT;
              else if (strncmp("GET /ml", m_pushback, 7)==0) reqtype = -REQ_ALARM_LEAK;
              else if (strncmp("GET /l0", m_pushback, 7)==0) reqtype = -REQ_LIGHTSOFF;
              else if (strncmp("GET /l1", m_pushback, 7)==0) reqtype = -REQ_LIGHTSON;
              else if (strncmp("GET /po", m_pushback, 7)==0) { reqtype = -REQ_OVERRIDE; weboption2 = -1; bHasSecondValue = false; bCommaCount = 0; }
              //else reqtype = -REQ_UNKNOWN;
      }
    }
}

void RA_ENC28J60::WebResponse (BufferFiller& buf, const prog_char *response, long strsize)
{
  PrintHeader(buf, strsize,0);
  buf.print(response);
}

//void RA_ENC28J60::processHTTP()
//{
//  word pos = ether.packetLoop(ether.packetReceive());
//  if (pos) { // check if valid tcp data is received
//    buf = ether.tcpOffset();
//    char* data = (char *) Ethernet::buffer + pos;
//    buf = ether.tcpOffset();
//    buf.emit_p(PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\nE405"));
//    ether.httpServerReply(buf.position()); // send web page data
//  }
//}

void RA_ENC28J60::processHTTP()
{
  //wdt_reset();
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
//  word pos = ether.packetLoop(ether.packetReceive());
  if (pos)
  {
    buf = ether.tcpOffset();
    char* data = (char *) Ethernet::buffer + pos;
    reqtype = REQ_ROOT;
    auth = true;
//    for (byte i = 0; i < len ; i++)
//    {
//      pushbuffer(data[i]);
//    }
//    Serial.print("Request type: ");
//    Serial.println(reqtype);

    timeout = millis();
    buf = ether.tcpOffset();
    buf.emit_p(PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\nE405"));
    ether.httpServerReply(buf.position()); // send web page data
  }
//
//  if (webnegoption) weboption *= -1;
//  if (authStr[0] == 0) auth = true;
//  if (auth)
//  {
//    auth = false;
//    switch (reqtype)
//    {
//      case REQ_ROOT:
//        {
//        WebResponse(buf, SERVER_DEFAULT, sizeof(SERVER_DEFAULT) - 1);
//        break;
//      }  // REQ_ROOT
//
//      case REQ_WIFI:
//        {
//        WebResponse(buf, SERVER_RA, sizeof(SERVER_RA) - 1);
//        break;
//      }  // REQ_WIFI
//
//      case REQ_RELAY:
//        {
//        if (weboption < 10) break;
//        byte o_relay = weboption / 10;
//        byte o_type = weboption % 10;
//        if (o_type == 0)  // Turn port off
//        {
//          if (o_relay < 9)
//          {
//            bitClear(ReefAngel.Relay.RelayMaskOn, o_relay - 1);
//            bitClear(ReefAngel.Relay.RelayMaskOff, o_relay - 1);
//          }
//#ifdef RELAY_EXP
//          if ( (o_relay > 10) && (o_relay < 89) )
//          {
//            byte EID = byte(o_relay/10);
//            bitClear(ReefAngel.Relay.RelayMaskOnE[EID-1],(o_relay%10)-1);
//            bitClear(ReefAngel.Relay.RelayMaskOffE[EID-1],(o_relay%10)-1);
//          }
//#endif  // RELAY_EXP
//        }
//        else if (o_type == 1)  // Turn port on
//        {
//          if (o_relay < 9)
//          {
//            bitSet(ReefAngel.Relay.RelayMaskOn, o_relay - 1);
//            bitSet(ReefAngel.Relay.RelayMaskOff, o_relay - 1);
//          }
//#ifdef RELAY_EXP
//          if ( (o_relay > 10) && (o_relay < 89) )
//          {
//            byte EID = byte(o_relay/10);
//            bitSet(ReefAngel.Relay.RelayMaskOnE[EID-1],(o_relay%10)-1);
//            bitSet(ReefAngel.Relay.RelayMaskOffE[EID-1],(o_relay%10)-1);
//          }
//#endif  // RELAY_EXP
//        }
//        else if (o_type == 2)  // Set port back to Auto
//        {
//          if (o_relay < 9)
//          {
//            bitClear(ReefAngel.Relay.RelayMaskOn, o_relay - 1);
//            bitSet(ReefAngel.Relay.RelayMaskOff, o_relay - 1);
//          }
//#ifdef RELAY_EXP
//          if ( (o_relay > 10) && (o_relay < 89) )
//          {
//            byte EID = byte(o_relay/10);
//            bitClear(ReefAngel.Relay.RelayMaskOnE[EID-1],(o_relay%10)-1);
//            bitSet(ReefAngel.Relay.RelayMaskOffE[EID-1],(o_relay%10)-1);
//          }
//#endif  // RELAY_EXP
//        }
//#ifdef OVERRIDE_PORTS
//        // Reset relay masks for ports we want always in their programmed states.
//        ReefAngel.Relay.RelayMaskOn &= ~ReefAngel.OverridePorts;
//        ReefAngel.Relay.RelayMaskOff |= ReefAngel.OverridePorts;
//#ifdef RELAY_EXP
//        byte i;
//        for ( i = 0; i < MAX_RELAY_EXPANSION_MODULES; i++ )
//        {
//          ReefAngel.Relay.RelayMaskOnE[i] &= ~ReefAngel.OverridePortsE[i];
//          ReefAngel.Relay.RelayMaskOffE[i] |= ReefAngel.OverridePortsE[i];
//        }
//#endif  // RELAY_EXP
//#endif  // OVERRIDE_PORTS
//        ReefAngel.Relay.Write();
//        // Force update of the Portal after relay change
////        ReefAngel.Timer[PORTAL_TIMER].ForceTrigger();
//      }
//
//      case REQ_RA_STATUS:
//        case REQ_R_STATUS:
//        {
//        char temp[6];
//        int s = 165;
//        //<RA><ID></ID><T1></T1><T2></T2><T3></T3><PH></PH><R></R><RON></RON><ROFF></ROFF><ATOLOW></ATOLOW><ATOHIGH></ATOHIGH><EM></EM><EM1></EM1><REM></REM><FLAG></FLAG></RA>
//        s += strlen(ReefAngel.portalusername);
//        s += intlength(ReefAngel.Params.Temp[T1_PROBE]);
//        s += intlength(ReefAngel.Params.Temp[T2_PROBE]);
//        s += intlength(ReefAngel.Params.Temp[T3_PROBE]);
//        s += intlength(ReefAngel.Params.PH);
//        s += intlength(ReefAngel.EM);
//        s += intlength(ReefAngel.EM1);
//        s += intlength(ReefAngel.REM);
//        s += intlength(ReefAngel.Flags);
//        s += 2;  // one digit for each ATO
//        s += intlength(ReefAngel.Relay.RelayData);
//        s += intlength(ReefAngel.Relay.RelayMaskOn);
//        s += intlength(ReefAngel.Relay.RelayMaskOff);
//#ifdef DISPLAY_LED_PWM
//        s += 56;
//        //<PWMA></PWMA><PWMD></PWMD><PWMAO></PWMAO><PWMDO></PWMDO>
//        s += intlength(ReefAngel.PWM.GetDaylightValue());
//        s += intlength(ReefAngel.PWM.GetActinicValue());
//        s += intlength(ReefAngel.PWM.GetDaylightOverrideValue());
//        s += intlength(ReefAngel.PWM.GetActinicOverrideValue());
//#endif  // DISPLAY_LED_PWM
//#ifdef RELAY_EXP
//        s += 296;
//        //<R0></R0><RON0></RON0><ROFF0></ROFF0><R1></R1><RON1></RON1><ROFF1></ROFF1><R2></R2><RON2></RON2><ROFF2></ROFF2><R3></R3><RON3></RON3><ROFF3></ROFF3><R4></R4><RON4></RON4><ROFF4></ROFF4><R5></R5><RON5></RON5><ROFF5></ROFF5><R6></R6><RON6></RON6><ROFF6></ROFF6><R7></R7><RON7></RON7><ROFF7></ROFF7>
//        for ( byte EID = 0; EID < MAX_RELAY_EXPANSION_MODULES; EID++ )
//        {
//          s += intlength(ReefAngel.Relay.RelayDataE[EID]);
//          s += intlength(ReefAngel.Relay.RelayMaskOnE[EID]);
//          s += intlength(ReefAngel.Relay.RelayMaskOffE[EID]);
//        }
//#endif  // RELAY_EXP
//#ifdef PWMEXPANSION
//        s += 90;
//        //<PWME0></PWME0><PWME1></PWME1><PWME2></PWME2><PWME3></PWME3><PWME4></PWME4><PWME5></PWME5>
//        for ( byte EID = 0; EID < PWM_EXPANSION_CHANNELS; EID++ ) s += intlength(ReefAngel.PWM.ExpansionChannel[EID]);
//#endif  // PWMEXPANSION
//#ifdef RFEXPANSION
//        s += 181;
//        //<RFM></RFM><RFS></RFS><RFD></RFD><RFW></RFW><RFRB></RFRB><RFR></RFR><RFG></RFG><RFB></RFB><RFI></RFI><RFWO></RFWO><RFRBO></RFRBO><RFRO></RFRO><RFGO></RFGO><RFBO></RFBO><RFIO></RFIO>
//        for ( byte EID = 0; EID < RF_CHANNELS; EID++ ) s += intlength(ReefAngel.RF.GetChannel(EID));
//        for ( byte EID = 0; EID < RF_CHANNELS; EID++ ) s += intlength(ReefAngel.RF.GetOverrideChannel(EID));
//        s += intlength(ReefAngel.RF.Mode);
//        s += intlength(ReefAngel.RF.Speed);
//        s += intlength(ReefAngel.RF.Duration);
//#endif  // RFEXPANSION
//#ifdef AI_LED
//        s += 76;
//        //<AIW></AIW><AIB></AIB><AIRB></AIRB><AIWO></AIWO><AIBO></AIBO><AIRBO></AIRBO>
//        for ( byte EID = 0; EID < AI_CHANNELS; EID++ ) s += intlength(ReefAngel.AI.GetChannel(EID));
//        for ( byte EID = 0; EID < AI_CHANNELS; EID++ ) s += intlength(ReefAngel.AI.GetOverrideChannel(EID));
//#endif  // AI_LED
//#ifdef SALINITYEXPANSION
//        s += 11;
//        //<SAL></SAL>
//        s += intlength(ReefAngel.Params.Salinity);
//#endif  // SALINITYEXPANSION
//#ifdef ORPEXPANSION
//        s += 11;
//        //<ORP></ORP>
//        s += intlength(ReefAngel.Params.ORP);
//#endif  // ORPEXPANSION
//#ifdef PHEXPANSION
//        s += 11;
//        //<PHE></PHE>
//        s += intlength(ReefAngel.Params.PHExp);
//#endif  // PHEXPANSION
//#ifdef WATERLEVELEXPANSION
//        s += 9;
//        //<WL></WL>
//        s += intlength(ReefAngel.WaterLevel.GetLevel());
//#endif  // WATERLEVELEXPANSION
//#ifdef HUMIDITYEXPANSION
//        s += 11;
//        //<HUM></HUM>
//        s += intlength(ReefAngel.Humidity.GetLevel());
//#endif  // HUMIDITYEXPANSION
//#ifdef DCPUMPCONTROL
//        s += 33;
//        //<DCM></DCM><DCS></DCS><DCD></DCD>
//        s += intlength(ReefAngel.DCPump.Mode);
//        s += intlength(ReefAngel.DCPump.Speed);
//        s += intlength(ReefAngel.DCPump.Duration);
//#endif  // DCPUMPCONTROL
//#ifdef IOEXPANSION
//        s += 9;
//        //<IO></IO>
//        s += intlength(ReefAngel.IO.GetChannel());
//#endif  // IOEXPANSION
//#ifdef CUSTOM_VARIABLES
//        s += 72;
//        //<C0></C0><C1></C1><C2></C2><C3></C3><C4></C4><C5></C5><C6></C6><C7></C7>
//        for ( byte EID = 0; EID < 8; EID++ ) s += intlength(ReefAngel.CustomVar[EID]);
//#endif  // CUSTOM_VARIABLES
//#ifdef ENABLE_ATO_LOGGING
//        if ( reqtype == REQ_RA_STATUS )
//        {
//          // we send all the ato logging events both high & low
//          //
//          // The XML code will be like this.  This is 1 event.  There are 8 events total (4 low / 4 high).
//          // <AL#ON>DWORD</AL#ON>
//          // <AL#OFF>DWORD</AL#OFF>
//          // Each event is 40 bytes (32 bytes for text, 8 bytes for dword values total)
//          //
//          s += 320;
//        }
//#endif  // ENABLE_ATO_LOGGING
//        PrintHeader(buf, s, 1);
//#ifdef ENABLE_ATO_LOGGING
//        if ( reqtype == REQ_RA_STATUS )
//        SendXMLData(buf, true);
//        else
//        SendXMLData(buf);
//#else
//        SendXMLData(buf);
//#endif  // ENABLE_ATO_LOGGING
//        break;
//
//      }  // REQ_RELAY
//      case REQ_M_BYTE:
//        case REQ_M_INT:
//        {
//        int s;
//
//        // if memory location is > 800 it means app is trying to pull/set old memory location.
//        // we decrease 600 to start using new memory map
//        // this is a temporary solution until we get all apps to point to new memory location
//        int newweboption2 = weboption2;
//        if (weboption2 >= 800) newweboption2 -= 600;
//
//        // weboption2 is location
//        // weboption is value
//        if (bHasSecondValue && (weboption2 >= 0))
//        {
//
//          // if we have a second value, we write the value to memory
//          if (reqtype == REQ_M_BYTE)
//            InternalMemory.write(newweboption2, weboption);
//          else
//          InternalMemory.write_int(newweboption2, weboption);
//
//          // check if we have to reload any timers
//          if (weboption2 == Mem_I_FeedingTimer)
//          {
//            ReefAngel.Timer[FEEDING_TIMER].SetInterval(weboption);
//          }
//          else if (weboption2 == Mem_I_LCDTimer)
//          {
//            ReefAngel.Timer[LCD_TIMER].SetInterval(weboption);
//          }
////#ifdef DISPLAY_LED_PWM
////          else if ( weboption2 == Mem_B_LEDPWMActinic )
////          {
////            ReefAngel.PWM.SetActinic(weboption);
////            // Force update of the Portal after change
////            ReefAngel.Timer[PORTAL_TIMER].ForceTrigger();
////          }
////          else if ( weboption2 == Mem_B_LEDPWMDaylight )
////          {
////            ReefAngel.PWM.SetDaylight(weboption);
////            // Force update of the Portal after change
////            ReefAngel.Timer[PORTAL_TIMER].ForceTrigger();
////          }
////#endif  // DISPLAY_LED_PWM
//
//          s = 9;  // <M>OK</M>
//          // add in the location, twice
//          s += (intlength(weboption2) * 2);
//          PrintHeader(buf, s, 1);
//
//          buf.print(XML_M_OPEN);
//          buf.print(weboption2, DEC);
//          buf.print(XML_CLOSE_TAG);
//          buf.print(XML_OK);
//          buf.print(XML_M_CLOSE);
//          buf.print(weboption2, DEC);
//          buf.print(XML_CLOSE_TAG);
//        }
//        else if (!bHasSecondValue && (weboption2 >= 0) && (bCommaCount == 0))
//        {
//          // get the length first
//          s = 7;  // <M></M>
//          // length of the memory location, twice since it's in the open & close tag
//          s += (intlength(weboption2) * 2);
//          // length of the value from memory
//          if (reqtype == REQ_M_BYTE)
//            s += intlength(InternalMemory.read(newweboption2));
//          else
//          s += intlength(InternalMemory.read_int(newweboption2));
//
//          PrintHeader(buf, s, 1);
//
//          // no second value and no comma, so we read the value from memory
//          buf.print(XML_M_OPEN);
//          buf.print(weboption2, DEC);
//          buf.print(XML_CLOSE_TAG);
//          if (reqtype == REQ_M_BYTE)
//            buf.print(InternalMemory.read(newweboption2), DEC);
//          else
//          buf.print(InternalMemory.read_int(newweboption2), DEC);
//          buf.print(XML_M_CLOSE);
//          buf.print(weboption2, DEC);
//          buf.print(XML_CLOSE_TAG);
//        }
//        else
//        {
//          s = 10;  // <M>ERR</M>
//          PrintHeader(buf, s, 1);
//          buf.print(XML_M_OPEN);
//          buf.print(XML_CLOSE_TAG);
//          buf.print(XML_ERR);
//          buf.print(XML_M_CLOSE);
//          buf.print(XML_CLOSE_TAG);
//        }
//        ether.httpServerReply(buf.position());
//        break;
//
//      }  // REQ_M_BYTE || REQ_M_INT
//      case REQ_OVERRIDE:
//        {
//        int s;
//
//        // weboption2 is channel
//        // weboption is override value
//        if (bHasSecondValue && (weboption2 < OVERRIDE_CHANNELS))
//        {
//          // Override channel
//          // if channel is from an expansion module that is not enabled, the command will be accepted, but it will do nothing.
//#ifdef DISPLAY_LED_PWM
//          if (weboption2 == 0)
//            ReefAngel.PWM.SetDaylightOverride(weboption);
//          else if (weboption2 == 1) ReefAngel.PWM.SetActinicOverride(weboption);
//#endif // DISPLAY_LED_PWM
//#ifdef PWMEXPANSION
//          if (weboption2>=2 && weboption2<=7) ReefAngel.PWM.SetChannelOverride(weboption2-2,weboption);
//#endif // PWMEXPANSION
//#ifdef AI_LED
//          if (weboption2>=8 && weboption2<=10) ReefAngel.AI.SetChannelOverride(weboption2-8,weboption);
//#endif // AI_LED
//#ifdef RFEXPANSION
//          if (weboption2>=11 && weboption2<=16) ReefAngel.RF.SetChannelOverride(weboption2-11,weboption);
//#endif // RFEXPANSION
//          s = 9;  // <P>OK</P>
//          // add in the channel, twice
//          s += (intlength(weboption2) * 2);
//          PrintHeader(buf, s, 1);
//          buf.print(XML_P_OPEN);
//          buf.print(weboption2, DEC);
//          buf.print(XML_CLOSE_TAG);
//          buf.print(XML_OK);
//          buf.print(XML_P_CLOSE);
//          buf.print(weboption2, DEC);
//          buf.print(XML_CLOSE_TAG);
//        }
//        else
//        {
//          s = 10;  // <P>ERR</P>
//          PrintHeader(buf, s, 1);
//          buf.print(XML_P_OPEN);
//          buf.print(XML_CLOSE_TAG);
//          buf.print(XML_ERR);
//          buf.print(XML_P_CLOSE);
//          buf.print(XML_CLOSE_TAG);
//        }
//        ether.httpServerReply(buf.position());
//        break;
//
//      }  // REQ_OVERRIDE
//      case REQ_M_RAW:
//        {
//        int s = 11;  // start with the base size of the mem tags
//
//        // default to Main memory locations
//        int memStart = VarsStart;
//        int memEnd = VarsEnd;
//        if (bHasSecondValue && (weboption2 >= 0))
//        {
//          memStart = weboption2;
//          memEnd = weboption;
//
//          // Some sanity check here
//          if (memStart > memEnd)
//          {
//            weboption = -1;
//          }
//        }
//
//        if (weboption == -1)
//        {
//          s = 14;  // <MEM>ERR</MEM>
//          PrintHeader(buf, s, 1);
//          buf.print(XML_MEM_OPEN);
//          buf.print(XML_ERR);
//        }
//        else
//        {
//          s += (memEnd - memStart) * 2;
//          PrintHeader(buf, s, 1);
//          buf.print(XML_MEM_OPEN);
//
//          byte m;
//          for (int x = memStart; x < memEnd; x++)
//          {
//            m = InternalMemory.read(x);
//            if (m < 16) buf.print("0");
//            buf.print(m, HEX);
//          }  // for x
//        }
//        buf.print(XML_MEM_CLOSE);
//        ether.httpServerReply(buf.position());
//        break;
//
//      }  // REQ_M_RAW
//      case REQ_M_ALL:
//        {
//        // TODO update all memory function
//        //int s = 123;  // start with the base size of the headers plus the mem tags
//        int s = 11;  // start with the base size of the mem tags
//        //
//        // Send all the data to the client requesting it.  The values will be sent as follows:
//        // - wrapped in <MEM></MEM> XML tags
//        // - individual memory values wrapped in their location XML tags
//        // Memory 800, value 20 - <M800>20</M800>
//        //
//        // An example would be:
//        //<MEM>
//        // <M800>20</M800>
//        // <M801>0</M801>
//        // <M802>16</M802>
//        // ...
//        // </MEM>
//        //
//        uint8_t offsets[] =
//        { 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2 };
//        uint8_t num = sizeof(offsets) / sizeof(uint8_t);
//        // add in the memory location sizes, 13 bytes if the memory location is 3 digits <MXXX></MXXX>
//        s += num * 13;
//        uint16_t count = VarsStart;
//        uint8_t x;
//        for (x = 0; x < num; x++)
//        {
//          if (offsets[x] == 1)
//            s += intlength(InternalMemory.read(count));
//          else
//          s += intlength(InternalMemory.read_int(count));
//          count += offsets[x];
//        }  // for x
//
//        PrintHeader(buf, s, 1);
//        buf.print(XML_MEM_OPEN);
//        //
//        // Loop through all the memory locations starting at VarsStart
//        // Check the offset for each location to tell us how many bytes to read and how much to increment
//        // the memory locations.  Currently there are only 1 and 2 byte locations, so when we check
//        // we just check for 1 and read a byte, otherwise we read 2 bytes (an int)
//        // Then we increment the memory position by the offset.
//        // We could be thrown off if somebody accidentally put a value larger than 2 in the offset array
//        //
//        for (x = 0, count = VarsStart; x < num; x++)
//        {
//          buf.print(XML_M_OPEN);
//          buf.print(count + 600, DEC); // 600 was added as temp fix - issue #26
//          buf.print(XML_CLOSE_TAG);
//          if (offsets[x] == 1)
//            buf.print(InternalMemory.read(count), DEC);
//          else
//          buf.print(InternalMemory.read_int(count), DEC);
//          buf.print(XML_M_CLOSE);
//          buf.print(count + 600, DEC); // 600 was added as temp fix - issue #26
//          buf.print(XML_CLOSE_TAG);
//          count += offsets[x];
//        }  // for x
//        buf.print(XML_MEM_CLOSE);
//        ether.httpServerReply(buf.position());
//        break;
//
//      }  // REQ_M_ALL
//      case REQ_VERSION:
//        {
//        int s = 7;
//        s += strlen(ReefAngel_Version);
//        PrintHeader(buf, s, 1);
//        buf.print("<V>"ReefAngel_Version"</V>");
//        ether.httpServerReply(buf.position());
//        break;
//      }  // REQ_VERSION
//      case REQ_DATE:
//        {
//        uint8_t s = 10;
//        uint8_t hr, min, mon, mday;
//        if (weboption > -1)
//        {
//          hr = weboption2 / 100;
//          min = weboption2 % 100;
//          mon = weboption3 / 100;
//          mday = weboption3 % 100;
//
//          if ((hr > 23 || hr < 0) ||
//            (min > 59 || min < 0) ||
//            (mon > 12 || mon < 1) ||
//            (mday > 31 || mday < 1))
//          {
//            weboption = -1;
//          }
//          else
//          {
//            s--;
//          }
//        }
//        else if (weboption == -2)
//        {
//          // sending controller date/time
//          // 51 = rest of xml tags
//          // 12 = data being sent (already have 3 extra in s)
//          //  7  = base xml tags (open & close d)
//          s += 60;
//        }
//        PrintHeader(buf, s, 1);
//        buf.print(XML_DATE_OPEN);
//        if (weboption == -1)
//        {
//          buf.print(XML_ERR);
//        }
//        else if (weboption == -2)
//        {
//          time_t n = now();
//          buf.print("<HR>");
//          buf.print(hour(n), DEC);
//          buf.print("</HR><MIN>");
//          buf.print(minute(n), DEC);
//          buf.print("</MIN><MON>");
//          buf.print(month(n), DEC);
//          buf.print("</MON><DAY>");
//          buf.print(day(n), DEC);
//          buf.print("</DAY><YR>");
//          buf.print(year(n), DEC);
//          buf.print("</YR>");
//        }
//        else
//        {
//          buf.print(XML_OK);
//          setTime(hr, min, 0, mday, mon, weboption);
//          now();
////          RTC.set(now());
//        }
//        buf.print(XML_DATE_CLOSE);
//        ether.httpServerReply(buf.position());
//        break;
//
//      }  // REQ_DATE
//      case REQ_HTTP:
//        {
//        // When using the WebBanner and sending to reefangel.com, ra.com replies back to us that the command was successful
//        // If we process that command (which we will), we end up not recognizing it and send off an uknown request response
//        // back to the server.  Then the server will send another response back to us and we end up getting in an almost
//        // infinite loop.  We will mark it as an HTTP request and ignore it
//        break;
//      }
//      case REQ_FEEDING:
//        {
//        // Start up the feeding mode only if we are on the home screen or from Water change
//        if (ReefAngel.DisplayedMenu == DEFAULT_MENU || ReefAngel.DisplayedMenu == WATERCHANGE_MODE)
//        {
//          ReefAngel.ChangeMode = FEEDING_MODE;
//          ModeResponse(buf, true);
//        }
//        else
//        {
//          ModeResponse(buf, false);
//        }
//        break;
//      }
//      case REQ_WATER:
//        {
//        // Start up the water change mode only if we are on the home screen
//        if (ReefAngel.DisplayedMenu == DEFAULT_MENU)
//        {
//          ReefAngel.ChangeMode = WATERCHANGE_MODE;
//          ModeResponse(buf, true);
//        }
//        else
//        {
//          ModeResponse(buf, false);
//        }
//        break;
//      }
//      case REQ_BTN_PRESS:
//        {
//        // Only accept button press for feeding and water change mode
//        if (ReefAngel.DisplayedMenu == FEEDING_MODE || ReefAngel.DisplayedMenu == WATERCHANGE_MODE)
//        {
//          // Simulate a button press to stop the modes
//          ButtonPress++;
//          ModeResponse(buf, true);
//        }
//        else
//        {
//          ModeResponse(buf, false);
//        }
//        break;
//      }
////      case REQ_CAL_RELOAD:
////      {
////        // Reload calibration values from memory
////        ReefAngel.PHMin = InternalMemory.PHMin_read();
////        ReefAngel.PHMax = InternalMemory.PHMax_read();
////#ifdef SALINITYEXPANSION
////        ReefAngel.SalMax = InternalMemory.SalMax_read();
////#endif  // SALINITYEXPANSION
////#ifdef ORPEXPANSION
////        ReefAngel.ORPMin = InternalMemory.ORPMin_read();
////        ReefAngel.ORPMax = InternalMemory.ORPMax_read();
////#endif  // ORPEXPANSION
////          buf.print(XML_OK);
////          break;
////      }
//      case REQ_ALARM_ATO:
//        {
//        ReefAngel.ATOClear();
//        ModeResponse(buf, true);
//        break;
//      }
//      case REQ_ALARM_OVERHEAT:
//        {
//        ReefAngel.OverheatClear();
//        ModeResponse(buf, true);
//        break;
//      }
//      case REQ_ALARM_LEAK:
//        {
//#ifdef LEAKDETECTOREXPANSION
//        ReefAngel.LeakClear();
//#endif // LEAKDETECTOREXPANSION
//        ModeResponse(buf, true);
//        break;
//      }
//      case REQ_LIGHTSON:
//        {
//        // Turn Lights On
//        ReefAngel.LightsOn();
//        ModeResponse(buf, true);
//        break;
//      }
//      case REQ_LIGHTSOFF:
//        {
//        // Turn Lights Off
//        ReefAngel.LightsOff();
//        ModeResponse(buf, true);
//        break;
//      }
//      default:
//        case REQ_UNKNOWN:
//        {
//        //P(WebBodyMsg) = SERVER_UKNOWN;
//        //WebResponse(buf, WebBodyMsg, sizeof(WebBodyMsg) - 1);
//        break;
//      }
//    }  // switch reqtype
//  }
//  else
//  {
//    if (reqtype > 0)
//    {
//      P(WebBodyMsg) = SERVER_DENY;
//      buf.print(SERVER_DENY);
//    }
//  }

  m_pushbackindex = 0;
  reqtype = 0;
  weboption = 0;
  webnegoption = false;
}

void RA_ENC28J60::SendXMLData(BufferFiller& buf, bool fAtoLog)
{
//  // This function is used for sending the XML data on the wifi interface
//  // It prints the strings from program memory instead of RAM
//  buf.print(XML_ID);
//  buf.print(ReefAngel.portalusername);
//  buf.print(XML_T1);
//  buf.print(ReefAngel.Params.Temp[T1_PROBE]);
//  buf.print(XML_T2);
//  buf.print(ReefAngel.Params.Temp[T2_PROBE]);
//  buf.print(XML_T3);
//  buf.print(ReefAngel.Params.Temp[T3_PROBE]);
//  buf.print(XML_PH);
//  buf.print(ReefAngel.Params.PH);
//  buf.print(XML_R);
//  buf.print(ReefAngel.Relay.RelayData, DEC);
//  buf.print(XML_RON);
//  buf.print(ReefAngel.Relay.RelayMaskOn, DEC);
//  buf.print(XML_ROFF);
//  buf.print(ReefAngel.Relay.RelayMaskOff, DEC);
//  buf.print(XML_RE_CLOSE);
//  buf.print(XML_RE_OFF);
//  buf.print(XML_CLOSE_TAG);
//#ifdef RELAY_EXP
//  for ( byte EID = 0; EID < MAX_RELAY_EXPANSION_MODULES; EID++ )
//  {
//    // relay data
//    buf.print(XML_RE_OPEN);
//    buf.print(EID+1, DEC);
//    buf.print(XML_CLOSE_TAG);
//    buf.print(ReefAngel.Relay.RelayDataE[EID],DEC);
//    buf.print(XML_RE_CLOSE);
//    buf.print(EID+1, DEC);
//    buf.print(XML_CLOSE_TAG);
//    // relay on mask
//    buf.print(XML_RE_OPEN);
//    buf.print(XML_RE_ON);
//    buf.print(EID+1, DEC);
//    buf.print(XML_CLOSE_TAG);
//    buf.print(ReefAngel.Relay.RelayMaskOnE[EID],DEC);
//    buf.print(XML_RE_CLOSE);
//    buf.print(XML_RE_ON);
//    buf.print(EID+1, DEC);
//    buf.print(XML_CLOSE_TAG);
//    // relay off mask
//    buf.print(XML_RE_OPEN);
//    buf.print(XML_RE_OFF);
//    buf.print(EID+1, DEC);
//    buf.print(XML_CLOSE_TAG);
//    buf.print(ReefAngel.Relay.RelayMaskOffE[EID],DEC);
//    buf.print(XML_RE_CLOSE);
//    buf.print(XML_RE_OFF);
//    buf.print(EID+1, DEC);
//    buf.print(XML_CLOSE_TAG);
//  }
//#endif  // RELAY_EXP
//  buf.print(XML_ATOLOW);
//  buf.print(ReefAngel.LowATO.IsActive());
//  buf.print(XML_ATOHIGH);
//  buf.print(ReefAngel.HighATO.IsActive());
//  buf.print(XML_EM);
//  buf.print(ReefAngel.EM, DEC);
//  buf.print(XML_EM1);
//  buf.print(ReefAngel.EM1, DEC);
//  buf.print(XML_REM);
//  buf.print(ReefAngel.REM, DEC);
//  buf.print(XML_FLAG);
//  buf.print(ReefAngel.Flags, DEC);
//  buf.print(XML_FLAG_END);
//#ifdef DISPLAY_LED_PWM
//  buf.print(XML_PWMA);
//  buf.print(ReefAngel.PWM.GetActinicValue(), DEC);
//  buf.print(XML_PWMD);
//  buf.print(ReefAngel.PWM.GetDaylightValue(), DEC);
//  buf.print(XML_PWMAO);
//  buf.print(ReefAngel.PWM.GetActinicOverrideValue(), DEC);
//  buf.print(XML_PWMDO);
//  buf.print(ReefAngel.PWM.GetDaylightOverrideValue(), DEC);
//  buf.print(XML_PWMDO_END);
//#endif  // DISPLAY_LED_PWM
//#ifdef SALINITYEXPANSION
//  buf.print(XML_SAL);
//  buf.print(ReefAngel.Params.Salinity, DEC);
//  buf.print(XML_SAL_END);
//#endif  // SALINITYEXPANSION
//#ifdef ORPEXPANSION
//  buf.print(XML_ORP);
//  buf.print(ReefAngel.Params.ORP, DEC);
//  buf.print(XML_ORP_END);
//#endif  // ORPEXPANSION
//#ifdef PHEXPANSION
//  buf.print(XML_PHEXP);
//  buf.print(ReefAngel.Params.PHExp, DEC);
//  buf.print(XML_PHEXP_END);
//#endif  // PHEXPANSION
//#ifdef WATERLEVELEXPANSION
//  buf.print(XML_WL);
//  buf.print(ReefAngel.WaterLevel.GetLevel(), DEC);
//  buf.print(XML_WL_END);
//#endif  // WATERLEVELEXPANSION
//#ifdef HUMIDITYEXPANSION
//  buf.print(XML_HUM);
//  buf.print(ReefAngel.Humidity.GetLevel(), DEC);
//  buf.print(XML_HUM_END);
//#endif  // HUMIDITYEXPANSION
//#ifdef DCPUMPCONTROL
//  buf.print(XML_DCM);
//  buf.print(ReefAngel.DCPump.Mode, DEC);
//  buf.print(XML_DCM_END);
//  buf.print(ReefAngel.DCPump.Speed, DEC);
//  buf.print(XML_DCS_END);
//  buf.print(ReefAngel.DCPump.Duration, DEC);
//  buf.print(XML_DCD_END);
//#endif  // DCPUMPCONTROL
//#ifdef IOEXPANSION
//  buf.print(XML_IO);
//  buf.print(ReefAngel.IO.GetChannel(), DEC);
//  buf.print(XML_IO_END);
//#endif  // IOEXPANSION
//#ifdef CUSTOM_VARIABLES
//  for ( byte EID = 0; EID < 8; EID++ )
//  {
//    buf.print(XML_C);
//    buf.print(EID, DEC);
//    buf.print(XML_CLOSE_TAG);
//    buf.print(ReefAngel.CustomVar[EID], DEC);
//    buf.print(XML_C_END);
//    buf.print(EID, DEC);
//    buf.print(XML_CLOSE_TAG);
//  }
//#endif  // CUSTOM_VARIABLES
//#ifdef PWMEXPANSION
//  for ( byte EID = 0; EID < PWM_EXPANSION_CHANNELS; EID++ )
//  {
//    buf.print(XML_PWME);
//    buf.print(EID, DEC);
//    buf.print(XML_CLOSE_TAG);
//    buf.print(ReefAngel.PWM.GetChannelValue(EID), DEC);
//    buf.print(XML_PWME_END);
//    buf.print(EID, DEC);
//    buf.print(XML_CLOSE_TAG);
//  }
//#endif  // PWMEXPANSION
//#ifdef AI_LED
//  buf.print(XML_AIW);
//  buf.print(ReefAngel.AI.GetChannel(0), DEC);
//  buf.print(XML_AIW_END);
//  buf.print(ReefAngel.AI.GetChannel(1), DEC);
//  buf.print(XML_AIB_END);
//  buf.print(ReefAngel.AI.GetChannel(2), DEC);
//  buf.print(XML_AIRB_END);
//  buf.print(ReefAngel.AI.GetOverrideChannel(0), DEC);
//  buf.print(XML_AIWO_END);
//  buf.print(ReefAngel.AI.GetOverrideChannel(1), DEC);
//  buf.print(XML_AIBO_END);
//  buf.print(ReefAngel.AI.GetOverrideChannel(2), DEC);
//  buf.print(XML_AIRBO_END);
//#endif  // AI_LED
//#ifdef RFEXPANSION
//  buf.print(XML_RFM);
//  buf.print(ReefAngel.RF.Mode, DEC);
//  buf.print(XML_RFM_END);
//  buf.print(ReefAngel.RF.Speed, DEC);
//  buf.print(XML_RFS_END);
//  buf.print(ReefAngel.RF.Duration, DEC);
//  buf.print(XML_RFD_END);
//  buf.print(ReefAngel.RF.GetChannel(0), DEC);
//  buf.print(XML_RFW_END);
//  buf.print(ReefAngel.RF.GetChannel(1), DEC);
//  buf.print(XML_RFRB_END);
//  buf.print(ReefAngel.RF.GetChannel(2), DEC);
//  buf.print(XML_RFR_END);
//  buf.print(ReefAngel.RF.GetChannel(3), DEC);
//  buf.print(XML_RFG_END);
//  buf.print(ReefAngel.RF.GetChannel(4), DEC);
//  buf.print(XML_RFB_END);
//  buf.print(ReefAngel.RF.GetChannel(5), DEC);
//  buf.print(XML_RFI_END);
//  buf.print(ReefAngel.RF.GetOverrideChannel(0), DEC);
//  buf.print(XML_RFWO_END);
//  buf.print(ReefAngel.RF.GetOverrideChannel(1), DEC);
//  buf.print(XML_RFRBO_END);
//  buf.print(ReefAngel.RF.GetOverrideChannel(2), DEC);
//  buf.print(XML_RFRO_END);
//  buf.print(ReefAngel.RF.GetOverrideChannel(3), DEC);
//  buf.print(XML_RFGO_END);
//  buf.print(ReefAngel.RF.GetOverrideChannel(4), DEC);
//  buf.print(XML_RFBO_END);
//  buf.print(ReefAngel.RF.GetOverrideChannel(5), DEC);
//  buf.print(XML_RFIO_END);
//#endif  // RFEXPANSION
//#ifdef ENABLE_ATO_LOGGING
//  if ( fAtoLog )
//  {
//    int loc;
//    for ( byte b = 0; b < MAX_ATO_LOG_EVENTS; b++ )
//    {
//      // print ato low event
//      // low start time
//      loc = (b * ATOEventSize) + ATOEventStart;
//      buf.print(XML_ATOLOW_LOG_OPEN);
//      buf.print(b,DEC);
//      buf.print(XML_RE_ON);
//      buf.print(XML_CLOSE_TAG);
//      buf.print(InternalMemory.read_dword(loc), DEC);
//      buf.print(XML_ATOLOW_LOG_CLOSE);
//      buf.print(b,DEC);
//      buf.print(XML_RE_ON);
//      buf.print(XML_CLOSE_TAG);
//      // zero out memory after sent
//      InternalMemory.write_dword(loc, 0);
//      // low stop time
//      loc += ATOEventOffStart;
//      buf.print(XML_ATOLOW_LOG_OPEN);
//      buf.print(b,DEC);
//      buf.print(XML_RE_OFF);
//      buf.print(XML_CLOSE_TAG);
//      buf.print(InternalMemory.read_dword(loc), DEC);
//      buf.print(XML_ATOLOW_LOG_CLOSE);
//      buf.print(b,DEC);
//      buf.print(XML_RE_OFF);
//      buf.print(XML_CLOSE_TAG);
//      // zero out memory after sent
//      InternalMemory.write_dword(loc, 0);
//      // print ato high event
//      // high start time
//      loc = (b * ATOEventSize) + ATOEventStart + (ATOEventSize * MAX_ATO_LOG_EVENTS);
//      buf.print(XML_ATOHIGH_LOG_OPEN);
//      buf.print(b,DEC);
//      buf.print(XML_RE_ON);
//      buf.print(XML_CLOSE_TAG);
//      buf.print(InternalMemory.read_dword(loc), DEC);
//      buf.print(XML_ATOHIGH_LOG_CLOSE);
//      buf.print(b,DEC);
//      buf.print(XML_RE_ON);
//      buf.print(XML_CLOSE_TAG);
//      // zero out memory after sent
//      InternalMemory.write_dword(loc, 0);
//      // high stop time
//      loc += ATOEventOffStart;
//      buf.print(XML_ATOHIGH_LOG_OPEN);
//      buf.print(b,DEC);
//      buf.print(XML_RE_OFF);
//      buf.print(XML_CLOSE_TAG);
//      buf.print(InternalMemory.read_dword(loc), DEC);
//      buf.print(XML_ATOHIGH_LOG_CLOSE);
//      buf.print(b,DEC);
//      buf.print(XML_RE_OFF);
//      buf.print(XML_CLOSE_TAG);
//      // zero out memory after sent
//      InternalMemory.write_dword(loc, 0);
//    }
//    AtoEventCount = 0;
//  }
//#endif  // ENABLE_ATO_LOGGING
//  buf.print(XML_END);
//  ether.httpServerReply(buf.position());
}
