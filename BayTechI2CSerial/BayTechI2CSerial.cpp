/**
 * BayTechSerial controls the BayTech power switches and gather stats via serial connection
 */

#include "BayTechI2CSerial.h"

BayTechI2CSerial::BayTechI2CSerial(I2C_SC16IS750 &i2cSC16IS750) {
  _returnLength = 0;
  _readCount = 0;
  _outletData = -1;
  _tempOutletData = -1;
  _sentOutletData = -1;
  _tempOnOutletData = -1;
  _tempOffOutletData = -1;
  _receiveComplete = true;
  _outletsPending = false;
  _lastUpdate = 0;
  _timeout = 0;
  _i2cSC16IS750 = &i2cSC16IS750;
}

boolean BayTechI2CSerial::available(void) {
  if (!_receiveComplete && millis() - _timeout > 5000) {
    _i2cSC16IS750->flush();
    _receiveComplete = true;
    _timeout = millis();
    return false;
  }

  while (_i2cSC16IS750->available() > 0) {
    byte c;
    c = _i2cSC16IS750->read();
#ifdef BT_SERIAL_DEBUG
    if(c != '\r') {
      Serial.print((char)c);
    }
#endif //  BT_SERIAL_DEBUG
    _readCount++;
  }

//  if(!_receiveComplete){
//    Serial.print(_readCount);
//    Serial.print(" >= ");
//    Serial.println(_returnLength);
//  }

  if (!_receiveComplete && _readCount >= _returnLength) {
    _readCount = 0;
    _returnLength = 0;
    _receiveComplete = true;
    _lastUpdate = millis();
    _timeout = millis();

    if (_outletsPending && _sentOutletData != _outletData) {
      _outletData = _sentOutletData;
      _outletsPending = false;
    }
  }

  else if (_receiveComplete) {
    _timeout = millis();
    //force a write and update every 15 seconds
    if (millis() - _lastUpdate > 15000) {
      _i2cSC16IS750->present();
      _outletData = ~_tempOutletData;
      _lastUpdate = millis();
    }

    if (_tempOutletData >= 0 && _tempOutletData != _outletData) {
      if (_tempOutletData == 0) {
        _returnLength = 12;
        _receiveComplete = false;
        _outletsPending = true;
        _i2cSC16IS750->println("off");
        _sentOutletData = _tempOutletData;
      }
      else if (_tempOutletData == 0xff) {
        _returnLength = 11;
        _receiveComplete = false;
        _outletsPending = true;
        _i2cSC16IS750->println("on");
        _sentOutletData = _tempOutletData;
      }
      else {
        String onPorts = "on ";
        _tempOnOutletData = _outletData;
        String offPorts = "off ";
        _tempOffOutletData = _outletData;

        for (int i = 0; i < 8; i++) {
          if (bitRead(_tempOutletData, i) && !outletStatus(i)) {
            onPorts.concat(i + 1);
            onPorts.concat(",");
            _tempOnOutletData |= 1 << i;
          }
          else if (!bitRead(_tempOutletData, i) && outletStatus(i)) {
            offPorts.concat(i + 1);
            offPorts.concat(",");
            bitClear(_tempOffOutletData, i);
          }
        }

        if (offPorts.length() > 4) {
          // remove trailing comma
          offPorts = offPorts.substring(0, offPorts.length() - 1);
          _returnLength = offPorts.length() + 9;
          _receiveComplete = false;
          _outletsPending = true;
          _i2cSC16IS750->println(offPorts);
          _sentOutletData = _tempOffOutletData;
        }

        else if (onPorts.length() > 3) {
          // remove trailing comma
          onPorts = onPorts.substring(0, onPorts.length() - 1);
          _returnLength = onPorts.length() + 9;
          _receiveComplete = false;
          _outletsPending = true;
          _i2cSC16IS750->println(onPorts);
          _sentOutletData = _tempOnOutletData;
        }
      }
      _lastUpdate = millis();
    }
  }
  return _receiveComplete;
}

boolean BayTechI2CSerial::outletStatus(byte outlet) {
  return bitRead(_outletData, outlet);
}

byte BayTechI2CSerial::getOutletData() {
  return _outletData;
}

boolean BayTechI2CSerial::setOutletData(byte outletData) {
  _tempOutletData = outletData;
  return 1;
}
