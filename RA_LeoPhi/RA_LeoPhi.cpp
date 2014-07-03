#include "RA_LeoPhi.h"

RA_LeoPhi::RA_LeoPhi() {

}

int RA_LeoPhi::Read() {
  int pH = 0;
  Wire.requestFrom(I2CLEOPHI, 2);
  if (Wire.available()) {
    pH = Wire.read() << 8 | Wire.read();
  }
  return pH;
}

int RA_LeoPhi::SetPh4() {
  sendCmd(I2C_CMD_SET_PH4);
  return receive(2);
}

int RA_LeoPhi::SetPh7() {
  sendCmd(I2C_CMD_SET_PH7);
  return receive(3);
}

int RA_LeoPhi::SetPH10() {
  sendCmd(I2C_CMD_SET_PH10);
  return receive(4);
}

boolean RA_LeoPhi::Default() {
  sendCmd(I2C_CMD_SET_DEFAULT);
  return receive(5);
}

void RA_LeoPhi::sendCmd(uint8_t cmd) {
  Wire.beginTransmission(I2CLEOPHI);
  Wire.write((uint8_t) cmd);
  Wire.endTransmission();
}

int RA_LeoPhi::receive(int size) {
  int receivedValue = -1;

  uint8_t available = Wire.requestFrom(I2CLEOPHI, size);

  if (available == 2) {
    receivedValue = Wire.read() << 8 | Wire.read();
  }

  Wire.endTransmission();
  return receivedValue;
}
