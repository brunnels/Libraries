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
 * RA_SC16IS750 emulates RA Relay to control a BayTech power distribution switch and gathers stats via serial connection
 */

#ifndef __RA_SC16IS750__
#define __RA_SC16IS750__

#include <Globals.h>
#include <BayTechI2CSerial.h>
#include <I2C_SC16IS750.h>
#include <Relay.h>

class RA_SC16IS750 : public RelayClass
{
  public:
    RA_SC16IS750(void);
    void Write(void);
#ifdef DISPLAY_TANK
    byte IOPorts;
    byte GetInputChannels();
    boolean GetInputChannel(byte channel);
#endif
    boolean SetOutputChannels(byte channelData);

  private:
    byte _channelData;
    boolean _initialized_1;
    I2C_SC16IS750 _i2cSC16IS750_1;
    BayTechI2CSerial _bt1;
    boolean _init1();
#ifdef DISPLAY_TANK
    boolean _initialized_2;
    I2C_SC16IS750 _i2cSC16IS750_2;
    BayTechI2CSerial _bt2;
    boolean _init2();
#endif
};

#endif // __RA_SC16IS750_
