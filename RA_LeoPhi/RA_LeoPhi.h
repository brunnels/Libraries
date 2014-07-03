#ifndef __RA_LEO_PHI_H__
#define __RA_LEO_PHI_H__
#include <Globals.h>
#include <Wire.h>

#define I2CLEOPHI 0x5a

enum {
  I2C_CMD_GET_PH = 1,
  I2C_CMD_SET_PH4 = 2,
  I2C_CMD_SET_PH7 = 3,
  I2C_CMD_SET_PH10 = 4,
  I2C_CMD_SET_DEFAULT = 5
};

class RA_LeoPhi {
  public:
    RA_LeoPhi();
    int Read();
    int SetPh4();
    int SetPh7();
    int SetPH10();
    boolean Default();
  private:
    void sendCmd(uint8_t cmd);
    int receive(int size);
};

#endif  // __RA_LEO_PHI_H__
