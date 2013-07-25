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

#include "RA_PCA9685.h"

RA_PCA9685::RA_PCA9685(byte address)
{
  // initialize variables
  LightsOverride = false;
  _address = address;
  _lastWrite = 0;

  for (byte a = 0; a < 16; a++)
  {
    PWMChannel[a] = 0;
  }
}

void RA_PCA9685::SendData(byte Channel, byte Data)
{
  if (Channel < 16) PWMChannel[Channel] = Data;

//  int newdata = (int) (Data * 40.95);
  int newdata = get12BitLuminanceValue(Data);
//  Serial.print("channel ");
//  Serial.print(Channel);
//  Serial.print(" value is ");
//  Serial.println(newdata);

  Wire.beginTransmission(_address);
  Wire.write(0x8 + (4 * Channel));
  Wire.write(newdata & 0xff);
  Wire.write(newdata >> 8);
  Wire.endTransmission();
}

void RA_PCA9685::SetAllChannelsPercent(byte p)
{
  for (byte a = 0; a < 16; a++)
  {
    PWMChannel[a] = p;
  }
}

void RA_PCA9685::SetChannelPercent(byte Channel, byte p)
{
  PWMChannel[Channel] = p;
}

void RA_PCA9685::SetPercentForChannels(int Channels, byte p)
{
  for (byte i = 0; i < 16; i++)
  {
    if (bitRead(Channels, i))
    {
      PWMChannel[i] = p;
    }
  }
}

void RA_PCA9685::SetSlopesForChannels(int Channels, uint8_t Start, uint8_t End, uint8_t Duration)
{
  for (int i = 0; i < 16; i++)
  {
    if (bitRead(Channels, i))
    {
      ChannelPWMSlope(i, Start, End, Duration);
    }
  }
}

void RA_PCA9685::SetSlopesForChannels(int Channels, uint8_t Start, uint8_t End, uint8_t Duration, byte MinuteOffset)
{
  for (int i = 0; i < 16; i++)
  {
    if (bitRead(Channels, i))
    {
      ChannelPWMSlope(i, Start, End, Duration, MinuteOffset);
    }
  }
}

void RA_PCA9685::SetParabolasForChannels(int Channels, uint8_t Start, uint8_t End)
{
  for (int i = 0; i < 16; i++)
  {
    if (bitRead(Channels, i))
    {
      ChannelPWMParabola(i, Start, End);
    }
  }
}

void RA_PCA9685::SetParabolasForChannels(int Channels, uint8_t Start, uint8_t End, byte MinuteOffset)
{
  for (int i = 0; i < 16; i++)
  {
    if (bitRead(Channels, i))
    {
      ChannelPWMParabola(i, Start, End, MinuteOffset);
    }
  }
}

void RA_PCA9685::Write()
{
  if (millis() - _lastWrite > 3000)
  {
    _lastWrite = millis();

    // setup PCA9685 for data receive
    // we need this to make sure it will work if connected ofter controller is booted, so we need to send it all the time.
    Wire.beginTransmission(_address);
    Wire.write(0);
    Wire.write(0xa1);
    Wire.endTransmission();

    for (byte a = 0; a < 16; a++)
    {
      SendData(a, PWMChannel[a]);
    }
  }
}

byte RA_PCA9685::GetChannelValue(byte Channel)
{
  if (PWMChannelOverride[Channel] < 100)
    return PWMChannelOverride[Channel];
  else
  return PWMChannel[Channel];
}

void RA_PCA9685::Channel0PWMSlope()
{
  ChannelPWMSlope(0, InternalMemory.PWMSlopeStart0_read(), InternalMemory.PWMSlopeEnd0_read(), InternalMemory.PWMSlopeDuration0_read());
}

void RA_PCA9685::Channel1PWMSlope()
{
  ChannelPWMSlope(1, InternalMemory.PWMSlopeStart1_read(), InternalMemory.PWMSlopeEnd1_read(), InternalMemory.PWMSlopeDuration1_read());
}

void RA_PCA9685::Channel2PWMSlope()
{
  ChannelPWMSlope(2, InternalMemory.PWMSlopeStart2_read(), InternalMemory.PWMSlopeEnd2_read(), InternalMemory.PWMSlopeDuration2_read());
}

void RA_PCA9685::Channel3PWMSlope()
{
  ChannelPWMSlope(3, InternalMemory.PWMSlopeStart3_read(), InternalMemory.PWMSlopeEnd3_read(), InternalMemory.PWMSlopeDuration3_read());
}

void RA_PCA9685::Channel4PWMSlope()
{
  ChannelPWMSlope(4, InternalMemory.PWMSlopeStart4_read(), InternalMemory.PWMSlopeEnd4_read(), InternalMemory.PWMSlopeDuration4_read());
}

void RA_PCA9685::Channel5PWMSlope()
{
  ChannelPWMSlope(5, InternalMemory.PWMSlopeStart5_read(), InternalMemory.PWMSlopeEnd5_read(), InternalMemory.PWMSlopeDuration5_read());
}

void RA_PCA9685::Channel6PWMSlope()
{
  ChannelPWMSlope(6, InternalMemory.PWMSlopeStart6_read(), InternalMemory.PWMSlopeEnd6_read(), InternalMemory.PWMSlopeDuration6_read());
}

void RA_PCA9685::Channel7PWMSlope()
{
  ChannelPWMSlope(7, InternalMemory.PWMSlopeStart7_read(), InternalMemory.PWMSlopeEnd7_read(), InternalMemory.PWMSlopeDuration7_read());
}

void RA_PCA9685::Channel8PWMSlope()
{
  ChannelPWMSlope(8, InternalMemory.PWMSlopeStart8_read(), InternalMemory.PWMSlopeEnd8_read(), InternalMemory.PWMSlopeDuration8_read());
}

void RA_PCA9685::Channel9PWMSlope()
{
  ChannelPWMSlope(9, InternalMemory.PWMSlopeStart9_read(), InternalMemory.PWMSlopeEnd9_read(), InternalMemory.PWMSlopeDuration9_read());
}

void RA_PCA9685::Channel10PWMSlope()
{
  ChannelPWMSlope(10, InternalMemory.PWMSlopeStart10_read(), InternalMemory.PWMSlopeEnd10_read(), InternalMemory.PWMSlopeDuration10_read());
}

void RA_PCA9685::Channel11PWMSlope()
{
  ChannelPWMSlope(11, InternalMemory.PWMSlopeStart11_read(), InternalMemory.PWMSlopeEnd11_read(), InternalMemory.PWMSlopeDuration11_read());
}

void RA_PCA9685::Channel12PWMSlope()
{
  ChannelPWMSlope(12, InternalMemory.PWMSlopeStart12_read(), InternalMemory.PWMSlopeEnd12_read(), InternalMemory.PWMSlopeDuration12_read());
}

void RA_PCA9685::Channel13PWMSlope()
{
  ChannelPWMSlope(13, InternalMemory.PWMSlopeStart13_read(), InternalMemory.PWMSlopeEnd13_read(), InternalMemory.PWMSlopeDuration13_read());
}

void RA_PCA9685::Channel14PWMSlope()
{
  ChannelPWMSlope(14, InternalMemory.PWMSlopeStart14_read(), InternalMemory.PWMSlopeEnd14_read(), InternalMemory.PWMSlopeDuration14_read());
}

void RA_PCA9685::Channel15PWMSlope()
{
  ChannelPWMSlope(15, InternalMemory.PWMSlopeStart15_read(), InternalMemory.PWMSlopeEnd15_read(), InternalMemory.PWMSlopeDuration15_read());
}

void RA_PCA9685::Channel0PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(0, InternalMemory.PWMSlopeStart0_read(), InternalMemory.PWMSlopeEnd0_read(), InternalMemory.PWMSlopeDuration0_read(), MinuteOffset);
}

void RA_PCA9685::Channel1PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(1, InternalMemory.PWMSlopeStart1_read(), InternalMemory.PWMSlopeEnd1_read(), InternalMemory.PWMSlopeDuration1_read(), MinuteOffset);
}

void RA_PCA9685::Channel2PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(2, InternalMemory.PWMSlopeStart2_read(), InternalMemory.PWMSlopeEnd2_read(), InternalMemory.PWMSlopeDuration2_read(), MinuteOffset);
}

void RA_PCA9685::Channel3PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(3, InternalMemory.PWMSlopeStart3_read(), InternalMemory.PWMSlopeEnd3_read(), InternalMemory.PWMSlopeDuration3_read(), MinuteOffset);
}

void RA_PCA9685::Channel4PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(4, InternalMemory.PWMSlopeStart4_read(), InternalMemory.PWMSlopeEnd4_read(), InternalMemory.PWMSlopeDuration4_read(), MinuteOffset);
}

void RA_PCA9685::Channel5PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(5, InternalMemory.PWMSlopeStart5_read(), InternalMemory.PWMSlopeEnd5_read(), InternalMemory.PWMSlopeDuration5_read(), MinuteOffset);
}

void RA_PCA9685::Channel6PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(6, InternalMemory.PWMSlopeStart6_read(), InternalMemory.PWMSlopeEnd6_read(), InternalMemory.PWMSlopeDuration6_read(), MinuteOffset);
}

void RA_PCA9685::Channel7PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(7, InternalMemory.PWMSlopeStart7_read(), InternalMemory.PWMSlopeEnd7_read(), InternalMemory.PWMSlopeDuration7_read(), MinuteOffset);
}

void RA_PCA9685::Channel8PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(8, InternalMemory.PWMSlopeStart8_read(), InternalMemory.PWMSlopeEnd8_read(), InternalMemory.PWMSlopeDuration8_read(), MinuteOffset);
}

void RA_PCA9685::Channel9PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(9, InternalMemory.PWMSlopeStart9_read(), InternalMemory.PWMSlopeEnd9_read(), InternalMemory.PWMSlopeDuration9_read(), MinuteOffset);
}

void RA_PCA9685::Channel10PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(10, InternalMemory.PWMSlopeStart10_read(), InternalMemory.PWMSlopeEnd10_read(), InternalMemory.PWMSlopeDuration10_read(), MinuteOffset);
}

void RA_PCA9685::Channel11PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(11, InternalMemory.PWMSlopeStart11_read(), InternalMemory.PWMSlopeEnd11_read(), InternalMemory.PWMSlopeDuration11_read(), MinuteOffset);
}

void RA_PCA9685::Channel12PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(12, InternalMemory.PWMSlopeStart12_read(), InternalMemory.PWMSlopeEnd12_read(), InternalMemory.PWMSlopeDuration12_read(), MinuteOffset);
}

void RA_PCA9685::Channel13PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(13, InternalMemory.PWMSlopeStart13_read(), InternalMemory.PWMSlopeEnd13_read(), InternalMemory.PWMSlopeDuration13_read(), MinuteOffset);
}

void RA_PCA9685::Channel14PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(14, InternalMemory.PWMSlopeStart14_read(), InternalMemory.PWMSlopeEnd14_read(), InternalMemory.PWMSlopeDuration14_read(), MinuteOffset);
}

void RA_PCA9685::Channel15PWMSlope(byte MinuteOffset)
{
  ChannelPWMSlope(15, InternalMemory.PWMSlopeStart15_read(), InternalMemory.PWMSlopeEnd15_read(), InternalMemory.PWMSlopeDuration15_read(), MinuteOffset);
}

void RA_PCA9685::ChannelPWMSlope(byte Channel, byte Start, byte End, byte Duration)
{
  SetChannel(Channel, PWMSlope(
    InternalMemory.StdLightsOnHour_read(),
    InternalMemory.StdLightsOnMinute_read(),
    InternalMemory.StdLightsOffHour_read(),
    InternalMemory.StdLightsOffMinute_read(),
    Start,
    End,
    Duration,
    PWMChannel[Channel]
    ));
}

void RA_PCA9685::ChannelPWMSlope(byte Channel, byte Start, byte End, byte Duration, byte MinuteOffset)
{
  int onTime = NumMins(InternalMemory.StdLightsOnHour_read(), InternalMemory.StdLightsOnMinute_read()) - MinuteOffset;
  int offTime = NumMins(InternalMemory.StdLightsOffHour_read(), InternalMemory.StdLightsOffMinute_read()) + MinuteOffset;
  SetChannel(Channel, PWMSlope(
    onTime / 60,
    onTime % 60,
    offTime / 60,
    offTime % 60,
    Start,
    End,
    Duration,
    PWMChannel[Channel]
    ));
}

void RA_PCA9685::Channel0PWMParabola()
{
  ChannelPWMParabola(0, InternalMemory.PWMSlopeStart0_read(), InternalMemory.PWMSlopeEnd0_read());
}

void RA_PCA9685::Channel1PWMParabola()
{
  ChannelPWMParabola(1, InternalMemory.PWMSlopeStart1_read(), InternalMemory.PWMSlopeEnd1_read());
}

void RA_PCA9685::Channel2PWMParabola()
{
  ChannelPWMParabola(2, InternalMemory.PWMSlopeStart2_read(), InternalMemory.PWMSlopeEnd2_read());
}

void RA_PCA9685::Channel3PWMParabola()
{
  ChannelPWMParabola(3, InternalMemory.PWMSlopeStart3_read(), InternalMemory.PWMSlopeEnd3_read());
}

void RA_PCA9685::Channel4PWMParabola()
{
  ChannelPWMParabola(4, InternalMemory.PWMSlopeStart4_read(), InternalMemory.PWMSlopeEnd4_read());
}

void RA_PCA9685::Channel5PWMParabola()
{
  ChannelPWMParabola(5, InternalMemory.PWMSlopeStart5_read(), InternalMemory.PWMSlopeEnd5_read());
}

void RA_PCA9685::Channel6PWMParabola()
{
  ChannelPWMParabola(6, InternalMemory.PWMSlopeStart6_read(), InternalMemory.PWMSlopeEnd6_read());
}

void RA_PCA9685::Channel7PWMParabola()
{
  ChannelPWMParabola(7, InternalMemory.PWMSlopeStart7_read(), InternalMemory.PWMSlopeEnd7_read());
}

void RA_PCA9685::Channel8PWMParabola()
{
  ChannelPWMParabola(8, InternalMemory.PWMSlopeStart8_read(), InternalMemory.PWMSlopeEnd8_read());
}

void RA_PCA9685::Channel9PWMParabola()
{
  ChannelPWMParabola(9, InternalMemory.PWMSlopeStart9_read(), InternalMemory.PWMSlopeEnd9_read());
}

void RA_PCA9685::Channel10PWMParabola()
{
  ChannelPWMParabola(10, InternalMemory.PWMSlopeStart10_read(), InternalMemory.PWMSlopeEnd10_read());
}

void RA_PCA9685::Channel11PWMParabola()
{
  ChannelPWMParabola(11, InternalMemory.PWMSlopeStart11_read(), InternalMemory.PWMSlopeEnd11_read());
}

void RA_PCA9685::Channel12PWMParabola()
{
  ChannelPWMParabola(12, InternalMemory.PWMSlopeStart12_read(), InternalMemory.PWMSlopeEnd12_read());
}

void RA_PCA9685::Channel13PWMParabola()
{
  ChannelPWMParabola(13, InternalMemory.PWMSlopeStart13_read(), InternalMemory.PWMSlopeEnd13_read());
}

void RA_PCA9685::Channel14PWMParabola()
{
  ChannelPWMParabola(14, InternalMemory.PWMSlopeStart14_read(), InternalMemory.PWMSlopeEnd14_read());
}

void RA_PCA9685::Channel15PWMParabola()
{
  ChannelPWMParabola(15, InternalMemory.PWMSlopeStart15_read(), InternalMemory.PWMSlopeEnd15_read());
}

void RA_PCA9685::Channel0PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(0, InternalMemory.PWMSlopeStart0_read(), InternalMemory.PWMSlopeEnd0_read(), MinuteOffset);
}

void RA_PCA9685::Channel1PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(1, InternalMemory.PWMSlopeStart1_read(), InternalMemory.PWMSlopeEnd1_read(), MinuteOffset);
}

void RA_PCA9685::Channel2PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(2, InternalMemory.PWMSlopeStart2_read(), InternalMemory.PWMSlopeEnd2_read(), MinuteOffset);
}

void RA_PCA9685::Channel3PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(3, InternalMemory.PWMSlopeStart3_read(), InternalMemory.PWMSlopeEnd3_read(), MinuteOffset);
}

void RA_PCA9685::Channel4PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(4, InternalMemory.PWMSlopeStart4_read(), InternalMemory.PWMSlopeEnd4_read(), MinuteOffset);
}

void RA_PCA9685::Channel5PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(5, InternalMemory.PWMSlopeStart5_read(), InternalMemory.PWMSlopeEnd5_read(), MinuteOffset);
}

void RA_PCA9685::Channel6PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(6, InternalMemory.PWMSlopeStart6_read(), InternalMemory.PWMSlopeEnd6_read(), MinuteOffset);
}

void RA_PCA9685::Channel7PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(7, InternalMemory.PWMSlopeStart7_read(), InternalMemory.PWMSlopeEnd7_read(), MinuteOffset);
}

void RA_PCA9685::Channel8PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(8, InternalMemory.PWMSlopeStart8_read(), InternalMemory.PWMSlopeEnd8_read(), MinuteOffset);
}

void RA_PCA9685::Channel9PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(9, InternalMemory.PWMSlopeStart9_read(), InternalMemory.PWMSlopeEnd9_read(), MinuteOffset);
}

void RA_PCA9685::Channel10PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(10, InternalMemory.PWMSlopeStart10_read(), InternalMemory.PWMSlopeEnd10_read(), MinuteOffset);
}

void RA_PCA9685::Channel11PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(11, InternalMemory.PWMSlopeStart11_read(), InternalMemory.PWMSlopeEnd11_read(), MinuteOffset);
}

void RA_PCA9685::Channel12PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(12, InternalMemory.PWMSlopeStart12_read(), InternalMemory.PWMSlopeEnd12_read(), MinuteOffset);
}

void RA_PCA9685::Channel13PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(13, InternalMemory.PWMSlopeStart13_read(), InternalMemory.PWMSlopeEnd13_read(), MinuteOffset);
}

void RA_PCA9685::Channel14PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(14, InternalMemory.PWMSlopeStart14_read(), InternalMemory.PWMSlopeEnd14_read(), MinuteOffset);
}

void RA_PCA9685::Channel15PWMParabola(byte MinuteOffset)
{
  ChannelPWMParabola(15, InternalMemory.PWMSlopeStart15_read(), InternalMemory.PWMSlopeEnd15_read(), MinuteOffset);
}

void RA_PCA9685::ChannelPWMParabola(byte Channel, byte Start, byte End)
{
  SetChannel(Channel, PWMParabola(
    InternalMemory.StdLightsOnHour_read(),
    InternalMemory.StdLightsOnMinute_read(),
    InternalMemory.StdLightsOffHour_read(),
    InternalMemory.StdLightsOffMinute_read(),
    Start,
    End,
    PWMChannel[Channel]
    ));
}

void RA_PCA9685::ChannelPWMParabola(byte Channel, byte Start, byte End, byte MinuteOffset)
{
  int onTime = NumMins(InternalMemory.StdLightsOnHour_read(), InternalMemory.StdLightsOnMinute_read()) - MinuteOffset;
  int offTime = NumMins(InternalMemory.StdLightsOffHour_read(), InternalMemory.StdLightsOffMinute_read()) + MinuteOffset;
  SetChannel(Channel, PWMParabola(
    onTime / 60,
    onTime % 60,
    offTime / 60,
    offTime % 60,
    Start,
    End,
    PWMChannel[Channel]
    ));
}

#ifdef SUNLOCATION
void RA_PCA9685::SunLocationChannel0PWMSlope()
{
  SunLocationChannelPWMSlope(0, InternalMemory.PWMSlopeStart0_read(), InternalMemory.PWMSlopeEnd0_read(), InternalMemory.PWMSlopeDuration0_read());
}

void RA_PCA9685::SunLocationChannel1PWMSlope()
{
  SunLocationChannelPWMSlope(1, InternalMemory.PWMSlopeStart1_read(), InternalMemory.PWMSlopeEnd1_read(), InternalMemory.PWMSlopeDuration1_read());
}

void RA_PCA9685::SunLocationChannel2PWMSlope()
{
  SunLocationChannelPWMSlope(2, InternalMemory.PWMSlopeStart2_read(), InternalMemory.PWMSlopeEnd2_read(), InternalMemory.PWMSlopeDuration2_read());
}

void RA_PCA9685::SunLocationChannel3PWMSlope()
{
  SunLocationChannelPWMSlope(3, InternalMemory.PWMSlopeStart3_read(), InternalMemory.PWMSlopeEnd3_read(), InternalMemory.PWMSlopeDuration3_read());
}

void RA_PCA9685::SunLocationChannel4PWMSlope()
{
  SunLocationChannelPWMSlope(4, InternalMemory.PWMSlopeStart4_read(), InternalMemory.PWMSlopeEnd4_read(), InternalMemory.PWMSlopeDuration4_read());
}

void RA_PCA9685::SunLocationChannel5PWMSlope()
{
  SunLocationChannelPWMSlope(5, InternalMemory.PWMSlopeStart5_read(), InternalMemory.PWMSlopeEnd5_read(), InternalMemory.PWMSlopeDuration5_read());
}

void RA_PCA9685::SunLocationChannel6PWMSlope()
{
  SunLocationChannelPWMSlope(6, InternalMemory.PWMSlopeStart6_read(), InternalMemory.PWMSlopeEnd6_read(), InternalMemory.PWMSlopeDuration6_read());
}

void RA_PCA9685::SunLocationChannel7PWMSlope()
{
  SunLocationChannelPWMSlope(7, InternalMemory.PWMSlopeStart7_read(), InternalMemory.PWMSlopeEnd7_read(), InternalMemory.PWMSlopeDuration7_read());
}

void RA_PCA9685::SunLocationChannel8PWMSlope()
{
  SunLocationChannelPWMSlope(8, InternalMemory.PWMSlopeStart8_read(), InternalMemory.PWMSlopeEnd8_read(), InternalMemory.PWMSlopeDuration8_read());
}

void RA_PCA9685::SunLocationChannel9PWMSlope()
{
  SunLocationChannelPWMSlope(9, InternalMemory.PWMSlopeStart9_read(), InternalMemory.PWMSlopeEnd9_read(), InternalMemory.PWMSlopeDuration9_read());
}

void RA_PCA9685::SunLocationChannel10PWMSlope()
{
  SunLocationChannelPWMSlope(10, InternalMemory.PWMSlopeStart10_read(), InternalMemory.PWMSlopeEnd10_read(), InternalMemory.PWMSlopeDuration10_read());
}

void RA_PCA9685::SunLocationChannel11PWMSlope()
{
  SunLocationChannelPWMSlope(11, InternalMemory.PWMSlopeStart11_read(), InternalMemory.PWMSlopeEnd11_read(), InternalMemory.PWMSlopeDuration11_read());
}

void RA_PCA9685::SunLocationChannel12PWMSlope()
{
  SunLocationChannelPWMSlope(12, InternalMemory.PWMSlopeStart12_read(), InternalMemory.PWMSlopeEnd12_read(), InternalMemory.PWMSlopeDuration12_read());
}

void RA_PCA9685::SunLocationChannel13PWMSlope()
{
  SunLocationChannelPWMSlope(13, InternalMemory.PWMSlopeStart13_read(), InternalMemory.PWMSlopeEnd13_read(), InternalMemory.PWMSlopeDuration13_read());
}

void RA_PCA9685::SunLocationChannel14PWMSlope()
{
  SunLocationChannelPWMSlope(14, InternalMemory.PWMSlopeStart14_read(), InternalMemory.PWMSlopeEnd14_read(), InternalMemory.PWMSlopeDuration14_read());
}

void RA_PCA9685::SunLocationChannel15PWMSlope()
{
  SunLocationChannelPWMSlope(15, InternalMemory.PWMSlopeStart15_read(), InternalMemory.PWMSlopeEnd15_read(), InternalMemory.PWMSlopeDuration15_read());
}

void RA_PCA9685::SunLocationChannel0PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(0, InternalMemory.PWMSlopeStart0_read(), InternalMemory.PWMSlopeEnd0_read(), InternalMemory.PWMSlopeDuration0_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel1PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(1, InternalMemory.PWMSlopeStart1_read(), InternalMemory.PWMSlopeEnd1_read(), InternalMemory.PWMSlopeDuration1_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel2PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(2, InternalMemory.PWMSlopeStart2_read(), InternalMemory.PWMSlopeEnd2_read(), InternalMemory.PWMSlopeDuration2_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel3PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(3, InternalMemory.PWMSlopeStart3_read(), InternalMemory.PWMSlopeEnd3_read(), InternalMemory.PWMSlopeDuration3_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel4PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(4, InternalMemory.PWMSlopeStart4_read(), InternalMemory.PWMSlopeEnd4_read(), InternalMemory.PWMSlopeDuration4_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel5PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(5, InternalMemory.PWMSlopeStart5_read(), InternalMemory.PWMSlopeEnd5_read(), InternalMemory.PWMSlopeDuration5_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel6PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(6, InternalMemory.PWMSlopeStart6_read(), InternalMemory.PWMSlopeEnd6_read(), InternalMemory.PWMSlopeDuration6_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel7PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(7, InternalMemory.PWMSlopeStart7_read(), InternalMemory.PWMSlopeEnd7_read(), InternalMemory.PWMSlopeDuration7_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel8PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(8, InternalMemory.PWMSlopeStart8_read(), InternalMemory.PWMSlopeEnd8_read(), InternalMemory.PWMSlopeDuration8_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel9PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(9, InternalMemory.PWMSlopeStart9_read(), InternalMemory.PWMSlopeEnd9_read(), InternalMemory.PWMSlopeDuration9_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel10PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(10, InternalMemory.PWMSlopeStart10_read(), InternalMemory.PWMSlopeEnd10_read(), InternalMemory.PWMSlopeDuration10_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel11PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(11, InternalMemory.PWMSlopeStart11_read(), InternalMemory.PWMSlopeEnd11_read(), InternalMemory.PWMSlopeDuration11_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel12PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(12, InternalMemory.PWMSlopeStart12_read(), InternalMemory.PWMSlopeEnd12_read(), InternalMemory.PWMSlopeDuration12_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel13PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(13, InternalMemory.PWMSlopeStart13_read(), InternalMemory.PWMSlopeEnd13_read(), InternalMemory.PWMSlopeDuration13_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel14PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(14, InternalMemory.PWMSlopeStart14_read(), InternalMemory.PWMSlopeEnd14_read(), InternalMemory.PWMSlopeDuration14_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel15PWMSlope(byte MinuteOffset)
{
  SunLocationChannelPWMSlope(15, InternalMemory.PWMSlopeStart15_read(), InternalMemory.PWMSlopeEnd15_read(), InternalMemory.PWMSlopeDuration15_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannelPWMSlope(byte Channel, byte Start, byte End, byte Duration)
{
  SetChannel(Channel, PWMSlope(
    InternalMemory.SunriseHour_read(),
    InternalMemory.SunriseMinute_read(),
    InternalMemory.SunsetHour_read(),
    InternalMemory.SunsetMinute_read(),
    Start,
    End,
    Duration,
    PWMChannel[Channel]
    ));
}

void RA_PCA9685::SunLocationChannelPWMSlope(byte Channel, byte Start, byte End, byte Duration, byte MinuteOffset)
{
  int onTime = NumMins(InternalMemory.SunriseHour_read(), InternalMemory.SunriseMinute_read()) - MinuteOffset;
  int offTime = NumMins(InternalMemory.SunsetHour_read(), InternalMemory.SunsetMinute_read()) + MinuteOffset;
  SetChannel(Channel, PWMSlope(
    onTime / 60,
    onTime % 60,
    offTime / 60,
    offTime % 60,
    Start,
    End,
    Duration,
    PWMChannel[Channel]
    ));
}

void RA_PCA9685::SunLocationChannel0PWMParabola()
{
  SunLocationChannelPWMParabola(0, InternalMemory.PWMSlopeStart0_read(), InternalMemory.PWMSlopeEnd0_read());
}

void RA_PCA9685::SunLocationChannel1PWMParabola()
{
  SunLocationChannelPWMParabola(1, InternalMemory.PWMSlopeStart1_read(), InternalMemory.PWMSlopeEnd1_read());
}

void RA_PCA9685::SunLocationChannel2PWMParabola()
{
  SunLocationChannelPWMParabola(2, InternalMemory.PWMSlopeStart2_read(), InternalMemory.PWMSlopeEnd2_read());
}

void RA_PCA9685::SunLocationChannel3PWMParabola()
{
  SunLocationChannelPWMParabola(3, InternalMemory.PWMSlopeStart3_read(), InternalMemory.PWMSlopeEnd3_read());
}

void RA_PCA9685::SunLocationChannel4PWMParabola()
{
  SunLocationChannelPWMParabola(4, InternalMemory.PWMSlopeStart4_read(), InternalMemory.PWMSlopeEnd4_read());
}

void RA_PCA9685::SunLocationChannel5PWMParabola()
{
  SunLocationChannelPWMParabola(5, InternalMemory.PWMSlopeStart5_read(), InternalMemory.PWMSlopeEnd5_read());
}

void RA_PCA9685::SunLocationChannel6PWMParabola()
{
  SunLocationChannelPWMParabola(6, InternalMemory.PWMSlopeStart6_read(), InternalMemory.PWMSlopeEnd6_read());
}

void RA_PCA9685::SunLocationChannel7PWMParabola()
{
  SunLocationChannelPWMParabola(7, InternalMemory.PWMSlopeStart7_read(), InternalMemory.PWMSlopeEnd7_read());
}

void RA_PCA9685::SunLocationChannel8PWMParabola()
{
  SunLocationChannelPWMParabola(8, InternalMemory.PWMSlopeStart8_read(), InternalMemory.PWMSlopeEnd8_read());
}

void RA_PCA9685::SunLocationChannel9PWMParabola()
{
  SunLocationChannelPWMParabola(9, InternalMemory.PWMSlopeStart9_read(), InternalMemory.PWMSlopeEnd9_read());
}

void RA_PCA9685::SunLocationChannel10PWMParabola()
{
  SunLocationChannelPWMParabola(10, InternalMemory.PWMSlopeStart10_read(), InternalMemory.PWMSlopeEnd10_read());
}

void RA_PCA9685::SunLocationChannel11PWMParabola()
{
  SunLocationChannelPWMParabola(11, InternalMemory.PWMSlopeStart11_read(), InternalMemory.PWMSlopeEnd11_read());
}

void RA_PCA9685::SunLocationChannel12PWMParabola()
{
  SunLocationChannelPWMParabola(12, InternalMemory.PWMSlopeStart12_read(), InternalMemory.PWMSlopeEnd12_read());
}

void RA_PCA9685::SunLocationChannel13PWMParabola()
{
  SunLocationChannelPWMParabola(13, InternalMemory.PWMSlopeStart13_read(), InternalMemory.PWMSlopeEnd13_read());
}

void RA_PCA9685::SunLocationChannel14PWMParabola()
{
  SunLocationChannelPWMParabola(14, InternalMemory.PWMSlopeStart14_read(), InternalMemory.PWMSlopeEnd14_read());
}

void RA_PCA9685::SunLocationChannel15PWMParabola()
{
  SunLocationChannelPWMParabola(15, InternalMemory.PWMSlopeStart15_read(), InternalMemory.PWMSlopeEnd15_read());
}

void RA_PCA9685::SunLocationChannel0PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(0, InternalMemory.PWMSlopeStart0_read(), InternalMemory.PWMSlopeEnd0_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel1PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(1, InternalMemory.PWMSlopeStart1_read(), InternalMemory.PWMSlopeEnd1_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel2PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(2, InternalMemory.PWMSlopeStart2_read(), InternalMemory.PWMSlopeEnd2_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel3PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(3, InternalMemory.PWMSlopeStart3_read(), InternalMemory.PWMSlopeEnd3_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel4PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(4, InternalMemory.PWMSlopeStart4_read(), InternalMemory.PWMSlopeEnd4_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel5PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(5, InternalMemory.PWMSlopeStart5_read(), InternalMemory.PWMSlopeEnd5_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel6PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(6, InternalMemory.PWMSlopeStart6_read(), InternalMemory.PWMSlopeEnd6_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel7PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(7, InternalMemory.PWMSlopeStart7_read(), InternalMemory.PWMSlopeEnd7_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel8PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(8, InternalMemory.PWMSlopeStart8_read(), InternalMemory.PWMSlopeEnd8_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel9PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(9, InternalMemory.PWMSlopeStart9_read(), InternalMemory.PWMSlopeEnd9_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel10PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(10, InternalMemory.PWMSlopeStart10_read(), InternalMemory.PWMSlopeEnd10_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel11PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(11, InternalMemory.PWMSlopeStart11_read(), InternalMemory.PWMSlopeEnd11_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel12PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(12, InternalMemory.PWMSlopeStart12_read(), InternalMemory.PWMSlopeEnd12_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel13PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(13, InternalMemory.PWMSlopeStart13_read(), InternalMemory.PWMSlopeEnd13_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel14PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(14, InternalMemory.PWMSlopeStart14_read(), InternalMemory.PWMSlopeEnd14_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannel15PWMParabola(byte MinuteOffset)
{
  SunLocationChannelPWMParabola(15, InternalMemory.PWMSlopeStart15_read(), InternalMemory.PWMSlopeEnd15_read(), MinuteOffset);
}

void RA_PCA9685::SunLocationChannelPWMParabola(byte Channel, byte Start, byte End)
{
  SetChannel(Channel, PWMParabola(
    InternalMemory.SunriseHour_read(),
    InternalMemory.SunriseMinute_read(),
    InternalMemory.SunsetHour_read(),
    InternalMemory.SunsetMinute_read(),
    Start,
    End,
    PWMChannel[Channel]
    ));
}

void RA_PCA9685::SunLocationChannelPWMParabola(byte Channel, byte Start, byte End, byte MinuteOffset)
{
  int onTime = NumMins(InternalMemory.SunriseHour_read(), InternalMemory.SunriseMinute_read()) - MinuteOffset;
  int offTime = NumMins(InternalMemory.SunsetHour_read(), InternalMemory.SunsetMinute_read()) + MinuteOffset;
  SetChannel(Channel, PWMParabola(
    onTime / 60,
    onTime % 60,
    offTime / 60,
    offTime % 60,
    Start,
    End,
    PWMChannel[Channel]
    ));
}
#endif  // SUNLOCATION
