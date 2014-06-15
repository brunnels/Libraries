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

/**
 * RA_SC16IS750 emulates RA Relay to control a BayTech power distribution switch and utilize input and output pins
 */

#include "RA_SC16IS750.h"
#include <Relay.h>

RA_SC16IS750::RA_SC16IS750():
#ifdef DISPLAY_TANK

_i2cSC16IS750_2(I2C_SC16IS750_2),
_bt2(_i2cSC16IS750_2),
#endif
_i2cSC16IS750_1(I2C_SC16IS750_1),
_bt1(_i2cSC16IS750_1)
{
  _channelData = 0;
  _initialized_1 = false;
#ifdef DISPLAY_TANK
  IOPorts = 0;
  _initialized_2 = false;
#endif // DISPLAY_TANK
}

boolean RA_SC16IS750::_init1() {
  if(_initialized_1) {
    _initialized_1 = true;
  }
  else if(!_initialized_1 && _i2cSC16IS750_1.present()) {
    _i2cSC16IS750_1.begin(9600);
    _i2cSC16IS750_1.ioSetDirection(0xff);
    _initialized_1 = true;
#ifdef BT_SERIAL_DEBUG
    Serial.print("BT1 Initialized");
#endif //  BT_SERIAL_DEBUG
  }
  else {
    _initialized_1 = false;
  }
  return _initialized_1;
}

#ifdef DISPLAY_TANK
boolean RA_SC16IS750::_init2() {
  if(_initialized_2) {
    _initialized_2 = true;
  }
  else if(!_initialized_2 && _i2cSC16IS750_2.present()) {
    _i2cSC16IS750_2.begin(9600);
    _i2cSC16IS750_2.ioSetDirection(0x00);
    _initialized_2 = true;
#ifdef BT_SERIAL_DEBUG
    Serial.print("BT2 Initialized");
#endif //  BT_SERIAL_DEBUG
  }
  else {
    _initialized_2 = false;
  }
  return _initialized_2;
}
#endif // DISPLAY_TANK

void RA_SC16IS750::Write()
{
#ifndef RA_TOUCHDISPLAY

  byte TempRelay = RelayData;
  byte present = 0;
#ifndef DISPLAY_TANK
  TempRelay &= RelayMaskOff;
  TempRelay |= RelayMaskOn;
  Wire.beginTransmission(I2CExpander1);
  Wire.write(~TempRelay);   // MSB
  present = Wire.endTransmission();
#ifdef SaveRelaysPresent
  RelayPresent = (present == 0);
#endif  // SaveRelaysPresent
#endif  //  DISPLAY_TANK

#ifdef RelayExp
  for ( byte EID = 0; EID < MAX_RELAY_EXPANSION_MODULES; EID++ ) {
    TempRelay = RelayDataE[EID];
    TempRelay &= RelayMaskOffE[EID];
    TempRelay |= RelayMaskOnE[EID];

    if(EID == 0) {
      present = _init1();
      if(present && _bt1.available()) {
        present = _bt1.setOutletData(TempRelay);
      }
    }

    else if(EID == 1) {
      present = _init1();
      if(present) {
        present = SetOutputChannels(TempRelay);
      }
    }
#ifdef DISPLAY_TANK
    else if(EID == 2) {
      present = _init2();
      if(present && _bt2.available()) {
        present = _bt2.setOutletData(TempRelay);
      }
    }
#endif //  DISPLAY_TANK
    else {
      Wire.beginTransmission(I2CExpModule+EID);
      Wire.write(~TempRelay);  // MSB
      present = Wire.endTransmission();
    }
#ifdef SaveRelaysPresent
    RelayPresentE[EID] = (present == 0);
#endif  // SaveRelaysPresent
  }
#endif  // RelayExp
#endif  // RA_TOUCHDISPLAY
}

#ifdef DISPLAY_TANK
byte RA_SC16IS750::GetInputChannels() {
  IOPorts = _i2cSC16IS750_2.ioGetState();
  return IOPorts;
}

boolean RA_SC16IS750::GetInputChannel(byte channel) {
  GetInputChannels();
  return bitRead(IOPorts, channel);
}
#endif // DISPLAY_TANK

boolean RA_SC16IS750::SetOutputChannels(byte channelData) {
#ifndef DISPLAY_TANK
  byte tempChannelData;
  // some remapping to fix my screwed up wiring job with the relays
  bitWrite(tempChannelData, 6, bitRead(channelData, 0));
  bitWrite(tempChannelData, 4, bitRead(channelData, 1));
  bitWrite(tempChannelData, 0, bitRead(channelData, 2));
  bitWrite(tempChannelData, 2, bitRead(channelData, 3));
  bitWrite(tempChannelData, 7, bitRead(channelData, 4));
  bitWrite(tempChannelData, 5, bitRead(channelData, 5));
  bitWrite(tempChannelData, 3, bitRead(channelData, 6));
  bitWrite(tempChannelData, 1, bitRead(channelData, 7));

  if(_channelData != tempChannelData) {
    if(_i2cSC16IS750_1.ioSetState(~tempChannelData)) {
      _channelData = tempChannelData;
    }
#else
  if(_channelData != channelData) {
    if(_i2cSC16IS750_1.ioSetState(~channelData)) {
      _channelData = channelData;
    }
#endif // DISPLAY_TANK
    else {
      return false;
    }
  }
  return true;
}
