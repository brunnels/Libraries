#ifndef __BAY_TECH_I2C_SERIAL__
#define __BAY_TECH_I2C_SERIAL__

#include <Globals.h>
#include <Wire.h>
#include <I2C_SC16IS750.h>

class BayTechI2CSerial {
  public:
    BayTechI2CSerial(I2C_SC16IS750 &i2cSC16IS750);
    boolean available(void);
    byte getOutletData(void);
    boolean setOutletData(byte outletData);

  private:
    void _init();
    I2C_SC16IS750* _i2cSC16IS750;
    boolean outletStatus(byte outlet);
    byte _returnLength;
    boolean _receiveComplete;
    unsigned long _lastUpdate;
    unsigned long _timeout;
    byte _outletData;
    byte _tempOutletData;
    byte _tempOnOutletData;
    byte _tempOffOutletData;
    byte _sentOutletData;
    boolean _outletsPending;
    byte _readCount;
};

#endif // __BAY_TECH_I2C_SERIAL__
