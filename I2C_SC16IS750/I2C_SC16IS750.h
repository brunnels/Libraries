#ifndef __I2C_SC16IS750_H_
#define __I2C_SC16IS750_H_

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <Wire.h>
#include "Stream.h"

// SC16IS750 Register definitions
// TODO: Don't bit shift these here, do it in the read/write register routines
#define THR        0x00 << 3
#define RHR        0x00 << 3
#define IER        0x01 << 3
#define FCR        0x02 << 3
#define IIR        0x02 << 3
#define LCR        0x03 << 3
#define MCR        0x04 << 3
#define LSR        0x05 << 3
#define MSR        0x06 << 3
#define SPR        0x07 << 3
#define TXLVL      0x08 << 3
#define RXLVL      0x09 << 3
#define DLAB       0x80 << 3
#define IODIR      0x0A << 3
#define IOSTATE    0x0B << 3
#define IOINTMSK   0x0C << 3
#define IOCTRL     0x0E << 3
#define EFCR       0x0F << 3

#define DLL        0x00 << 3
#define DLM        0x01 << 3
#define EFR        0x02 << 3
#define XON1       0x04 << 3
#define XON2       0x05 << 3
#define XOFF1      0x06 << 3
#define XOFF2      0x07 << 3

#define BAUD_RATE_DEFAULT 9600

// See datasheet section 7.8 for configuring the
// "Programmable baud rate generator"
#define PRESCALER 1 // Default prescaler after reset
#define BAUD_RATE_DIVISOR(baud) ((XTAL_FREQUENCY/PRESCALER)/(baud*16UL))

// See section 8.10 of the datasheet for definitions
// of bits in the Enhanced Features Register (EFR)
#define EFR_ENABLE_CTS 1 << 7
#define EFR_ENABLE_RTS 1 << 6
#define EFR_ENABLE_ENHANCED_FUNCTIONS 1 << 4

// See section 8.4 of the datasheet for definitions
// of bits in the Line Control Register (LCR)
#define LCR_ENABLE_DIVISOR_LATCH 1 << 7

#define XTAL_FREQUENCY 14745600UL // On-board crystal (New mid-2010 Version)

class I2C_SC16IS750 : public Stream {
  public:
    I2C_SC16IS750(uint8_t address = 0x48);
    void begin(unsigned long baudrate = BAUD_RATE_DEFAULT);
    int available();
    int read();
    void ioSetDirection(byte bits);
    boolean ioSetState(byte bits);
    byte ioGetState();
#if ARDUINO >= 100
    size_t write(byte value);
    size_t write(const char *str);
#else
    void write(byte value);
    void write(const char *str);
#endif
    using Print::write;
    void flush();

//required for Stream
    int peek() {return 0;};
    boolean present();
    boolean uartConnected();

  protected:
    byte deselect();
    void select();
    void writeRegister(byte registerAddress, byte data);
    byte readRegister(byte registerAddress);

    void initUart();
    void configureUart();
    void setBaudRate();

    unsigned long _baudRate;

  private:
    int8_t _device;
    boolean _uartInit;

};

#endif /* __I2C_SC16IS750_H_ */
