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

#ifndef __RA_PCA9685_H__
#define __RA_PCA9685_H__

#include <Globals.h>
#include <InternalEEPROM.h>
#include <Wire.h>

class RA_PCA9685
{
  public:
    RA_PCA9685(byte address);
    boolean LightsOverride;

    byte PWMChannel[16];
    byte PWMChannelOverride[16];
    void inline SetChannel(byte Channel, byte Value)
    {
      if (Channel < 16) PWMChannel[Channel] = Value;
    }
    ;
    void inline SetChannelOverride(byte Channel, byte Value)
    {
      if (Channel < 16) PWMChannelOverride[Channel] = Value;
    }
    ;
    void SetAllChannelsPercent(byte p);
    void SetChannelPercent(byte Channel, byte p);
    void SetPercentForChannels(int Channels, byte p);
    void SetSlopesForChannels(int Channels, uint8_t Start, uint8_t End, uint8_t Duration);
    void SetSlopesForChannels(int Channels, uint8_t Start, uint8_t End, uint8_t Duration, byte MinuteOffset);
    void SetParabolasForChannels(int Channels, uint8_t Start, uint8_t End);
    void SetParabolasForChannels(int Channels, uint8_t Start, uint8_t End, byte MinuteOffset);

    byte GetChannelValue(byte Channel);
    void SendData(byte Channel, byte Data);
    void Write();

    void Channel0PWMSlope();
    void Channel1PWMSlope();
    void Channel2PWMSlope();
    void Channel3PWMSlope();
    void Channel4PWMSlope();
    void Channel5PWMSlope();
    void Channel6PWMSlope();
    void Channel7PWMSlope();
    void Channel8PWMSlope();
    void Channel9PWMSlope();
    void Channel10PWMSlope();
    void Channel11PWMSlope();
    void Channel12PWMSlope();
    void Channel13PWMSlope();
    void Channel14PWMSlope();
    void Channel15PWMSlope();

    void Channel0PWMSlope(byte MinuteOffset);
    void Channel1PWMSlope(byte MinuteOffset);
    void Channel2PWMSlope(byte MinuteOffset);
    void Channel3PWMSlope(byte MinuteOffset);
    void Channel4PWMSlope(byte MinuteOffset);
    void Channel5PWMSlope(byte MinuteOffset);
    void Channel6PWMSlope(byte MinuteOffset);
    void Channel7PWMSlope(byte MinuteOffset);
    void Channel8PWMSlope(byte MinuteOffset);
    void Channel9PWMSlope(byte MinuteOffset);
    void Channel10PWMSlope(byte MinuteOffset);
    void Channel11PWMSlope(byte MinuteOffset);
    void Channel12PWMSlope(byte MinuteOffset);
    void Channel13PWMSlope(byte MinuteOffset);
    void Channel14PWMSlope(byte MinuteOffset);
    void Channel15PWMSlope(byte MinuteOffset);

    void ChannelPWMSlope(byte Channel, byte Start, byte End, byte Duration);
    void ChannelPWMSlope(byte Channel, byte Start, byte End, byte Duration, byte MinuteOffset);

    void Channel0PWMParabola();
    void Channel1PWMParabola();
    void Channel2PWMParabola();
    void Channel3PWMParabola();
    void Channel4PWMParabola();
    void Channel5PWMParabola();
    void Channel6PWMParabola();
    void Channel7PWMParabola();
    void Channel8PWMParabola();
    void Channel9PWMParabola();
    void Channel10PWMParabola();
    void Channel11PWMParabola();
    void Channel12PWMParabola();
    void Channel13PWMParabola();
    void Channel14PWMParabola();
    void Channel15PWMParabola();

    void Channel0PWMParabola(byte MinuteOffset);
    void Channel1PWMParabola(byte MinuteOffset);
    void Channel2PWMParabola(byte MinuteOffset);
    void Channel3PWMParabola(byte MinuteOffset);
    void Channel4PWMParabola(byte MinuteOffset);
    void Channel5PWMParabola(byte MinuteOffset);
    void Channel6PWMParabola(byte MinuteOffset);
    void Channel7PWMParabola(byte MinuteOffset);
    void Channel8PWMParabola(byte MinuteOffset);
    void Channel9PWMParabola(byte MinuteOffset);
    void Channel10PWMParabola(byte MinuteOffset);
    void Channel11PWMParabola(byte MinuteOffset);
    void Channel12PWMParabola(byte MinuteOffset);
    void Channel13PWMParabola(byte MinuteOffset);
    void Channel14PWMParabola(byte MinuteOffset);
    void Channel15PWMParabola(byte MinuteOffset);

    void ChannelPWMParabola(byte Channel, byte Start, byte End);
    void ChannelPWMParabola(byte Channel, byte Start, byte End, byte MinuteOffset);

#ifdef SUNLOCATION
    void SunLocationChannel0PWMSlope();
    void SunLocationChannel1PWMSlope();
    void SunLocationChannel2PWMSlope();
    void SunLocationChannel3PWMSlope();
    void SunLocationChannel4PWMSlope();
    void SunLocationChannel5PWMSlope();
    void SunLocationChannel6PWMSlope();
    void SunLocationChannel7PWMSlope();
    void SunLocationChannel8PWMSlope();
    void SunLocationChannel9PWMSlope();
    void SunLocationChannel10PWMSlope();
    void SunLocationChannel11PWMSlope();
    void SunLocationChannel12PWMSlope();
    void SunLocationChannel13PWMSlope();
    void SunLocationChannel14PWMSlope();
    void SunLocationChannel15PWMSlope();

    void SunLocationChannel0PWMSlope(byte MinuteOffset);
    void SunLocationChannel1PWMSlope(byte MinuteOffset);
    void SunLocationChannel2PWMSlope(byte MinuteOffset);
    void SunLocationChannel3PWMSlope(byte MinuteOffset);
    void SunLocationChannel4PWMSlope(byte MinuteOffset);
    void SunLocationChannel5PWMSlope(byte MinuteOffset);
    void SunLocationChannel6PWMSlope(byte MinuteOffset);
    void SunLocationChannel7PWMSlope(byte MinuteOffset);
    void SunLocationChannel8PWMSlope(byte MinuteOffset);
    void SunLocationChannel9PWMSlope(byte MinuteOffset);
    void SunLocationChannel10PWMSlope(byte MinuteOffset);
    void SunLocationChannel11PWMSlope(byte MinuteOffset);
    void SunLocationChannel12PWMSlope(byte MinuteOffset);
    void SunLocationChannel13PWMSlope(byte MinuteOffset);
    void SunLocationChannel14PWMSlope(byte MinuteOffset);
    void SunLocationChannel15PWMSlope(byte MinuteOffset);

    void SunLocationChannelPWMSlope(byte Channel, byte Start, byte End, byte Duration);
    void SunLocationChannelPWMSlope(byte Channel, byte Start, byte End, byte Duration, byte MinuteOffset);

    void SunLocationChannel0PWMParabola();
    void SunLocationChannel1PWMParabola();
    void SunLocationChannel2PWMParabola();
    void SunLocationChannel3PWMParabola();
    void SunLocationChannel4PWMParabola();
    void SunLocationChannel5PWMParabola();
    void SunLocationChannel6PWMParabola();
    void SunLocationChannel7PWMParabola();
    void SunLocationChannel8PWMParabola();
    void SunLocationChannel9PWMParabola();
    void SunLocationChannel10PWMParabola();
    void SunLocationChannel11PWMParabola();
    void SunLocationChannel12PWMParabola();
    void SunLocationChannel13PWMParabola();
    void SunLocationChannel14PWMParabola();
    void SunLocationChannel15PWMParabola();

    void SunLocationChannel0PWMParabola(byte MinuteOffset);
    void SunLocationChannel1PWMParabola(byte MinuteOffset);
    void SunLocationChannel2PWMParabola(byte MinuteOffset);
    void SunLocationChannel3PWMParabola(byte MinuteOffset);
    void SunLocationChannel4PWMParabola(byte MinuteOffset);
    void SunLocationChannel5PWMParabola(byte MinuteOffset);
    void SunLocationChannel6PWMParabola(byte MinuteOffset);
    void SunLocationChannel7PWMParabola(byte MinuteOffset);
    void SunLocationChannel8PWMParabola(byte MinuteOffset);
    void SunLocationChannel9PWMParabola(byte MinuteOffset);
    void SunLocationChannel10PWMParabola(byte MinuteOffset);
    void SunLocationChannel11PWMParabola(byte MinuteOffset);
    void SunLocationChannel12PWMParabola(byte MinuteOffset);
    void SunLocationChannel13PWMParabola(byte MinuteOffset);
    void SunLocationChannel14PWMParabola(byte MinuteOffset);
    void SunLocationChannel15PWMParabola(byte MinuteOffset);

    void SunLocationChannelPWMParabola(byte Channel, byte Start, byte End);
    void SunLocationChannelPWMParabola(byte Channel, byte Start, byte End, byte MinuteOffset);
#endif  // SUNLOCATION

  private:
    byte _address;
    unsigned long _lastWrite;
};

#endif  // __RA_PCA9685_H__
