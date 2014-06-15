#include "I2C_SC16IS750.h"
#include <Wire.h>

I2C_SC16IS750::I2C_SC16IS750(uint8_t address) {
  _device = address;
  _uartInit = false;
  _baudRate = BAUD_RATE_DEFAULT;
}

void I2C_SC16IS750::begin(unsigned long baudrate) {
  Wire.begin();
  _baudRate = baudrate;
  initUart();
}

void I2C_SC16IS750::writeRegister(byte registerAddress, byte data) {
  select();
  Wire.write(registerAddress);
  Wire.write(data);
  deselect();
}

byte I2C_SC16IS750::readRegister(byte registerAddress) {
  select();
  Wire.write(registerAddress);
  if(deselect() != 0) {
    return -1;
  }

  select();
  Wire.requestFrom(_device, 1);    // request 1 uint8_t from device
  while(!Wire.available());

  uint8_t result = Wire.read();
  deselect();
  return result;
}

void I2C_SC16IS750::select() {
  Wire.beginTransmission(_device);
}

byte I2C_SC16IS750::deselect() {
  return Wire.endTransmission();
}

// SC16IS750 register values
struct SC16IS750_UART_cfg {
  char DataFormat;
  char Flow;
};

struct SC16IS750_UART_cfg SC16IS750_Uart_config = {
  0x03,
  EFR_ENABLE_RTS | EFR_ENABLE_ENHANCED_FUNCTIONS
};

void I2C_SC16IS750::initUart() {
  // Initialise and test SC16IS750
  configureUart();
  if(uartConnected()) {
    _uartInit = true;
    Serial.println("uart connected");
  }
}

void I2C_SC16IS750::setBaudRate() {
  unsigned long divisor = BAUD_RATE_DIVISOR(_baudRate);
  writeRegister(LCR, LCR_ENABLE_DIVISOR_LATCH); // "Program baudrate"
  writeRegister(DLL, lowByte(divisor));
  writeRegister(DLM, highByte(divisor));
}

void I2C_SC16IS750::configureUart() {

  writeRegister(IOCTRL, 1 << 3); //software reset
  setBaudRate();
  writeRegister(LCR, 0xBF); // access EFR register
  writeRegister(EFR, SC16IS750_Uart_config.Flow); // enable enhanced registers
  writeRegister(LCR, SC16IS750_Uart_config.DataFormat); // 8 data bit, 1 stop bit, no parity
  writeRegister(FCR, 0x06); // reset TXFIFO, reset RXFIFO, non FIFO mode
  writeRegister(FCR, 0x01); // enable FIFO mode
}

boolean I2C_SC16IS750::present() {
  Wire.begin();
  select();
  return (deselect() == 0) ? true : false;
}

/**
 * Check that UART is connected and operational.
 * Perform read/write test to check if UART is working
 */
boolean I2C_SC16IS750::uartConnected() {
  const char TEST_CHARACTER = 'H';
  writeRegister(SPR, TEST_CHARACTER);
  return (readRegister(SPR) == TEST_CHARACTER);
}

/**
 * Get the number of bytes (characters) available for reading.
 *
 * This is data that's already arrived and stored in the receive
 * buffer (which holds 64 bytes).
 */
int I2C_SC16IS750::available() {
  return (readRegister(RXLVL));
}

int I2C_SC16IS750::read() {
  if (!available()) {
    return -1;
  }

  return readRegister(RHR);
}

#if ARDUINO >= 100
size_t I2C_SC16IS750::write(byte value) {
#else
void I2C_SC16IS750::write(byte value) {
#endif
  if(readRegister(TXLVL) >= 0) {
    while (readRegister(TXLVL) == 0) {
      // Wait for space in TX buffer
    };
    writeRegister(THR, value);
#if ARDUINO >= 100
    return 0;
  }
  else {
    return -1;
#endif
  }
}


#if ARDUINO >= 100
size_t I2C_SC16IS750::write(const char *str) {
#else
void I2C_SC16IS750::write(const char *str) {
#endif
  if(readRegister(TXLVL) >= 0) {
    write((const uint8_t *) str, strlen(str));
    while (readRegister(TXLVL) < 64) {
      // Wait for empty TX buffer (slow)
      // (But apparently still not slow enough to ensure delivery.)
    };
#if ARDUINO >= 100
    return 0;
  }
  else {
    return -1;
#endif
  }
}

void I2C_SC16IS750::flush() {
  /*
   * Flush characters from SC16IS750 receive buffer.
   */

  // Note: This may not be the most appropriate flush approach.
  //       It might be better to just flush the UART's buffer
  //       rather than the buffer of the connected device
  //       which is essentially what this does.
  while(available() > 0) {
    read();
  }
}

void I2C_SC16IS750::ioSetDirection(byte bits) {
  writeRegister(IODIR, bits);
}

boolean I2C_SC16IS750::ioSetState(byte bits) {
  writeRegister(IOSTATE, bits);
  return true;
}

byte I2C_SC16IS750::ioGetState() {
  return readRegister(IOSTATE);
}
