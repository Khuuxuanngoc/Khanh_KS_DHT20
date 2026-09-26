


#include "MKE_I2C_DHT20.h"


//  set MKE_I2C_DHT20_WIRE_TIME_OUT to 0 to disable.
//  note this timeout is commented in code below.
#define MKE_I2C_DHT20_WIRE_TIME_OUT         250000    //  microseconds

const uint8_t MKE_I2C_DHT20_ADDRESS = 0x38;


MKE_I2C_DHT20::MKE_I2C_DHT20()
{
  _wire        = &Wire;
  //  reset() ?
  _temperature = 0;
  _humidity    = 0;
  _humOffset   = 0;
  _tempOffset  = 0;
  _status      = MKE_I2C_DHT20_OK;
  _lastRequest = 0;
  _lastRead    = 0;
}


bool MKE_I2C_DHT20::begin(TwoWire *wire)
{
  _wire = wire;
  //  _wire->setWireTimeout(MKE_I2C_DHT20_WIRE_TIME_OUT, true);
  return isConnected();
}


bool MKE_I2C_DHT20::isConnected()
{
  _wire->beginTransmission(MKE_I2C_DHT20_ADDRESS);
  int rv = _wire->endTransmission();
  return rv == 0;
}


uint8_t MKE_I2C_DHT20::getAddress()
{
  return MKE_I2C_DHT20_ADDRESS;
}


//  See datasheet 7.4 Sensor Reading Process, point 1
//  use with care.
uint8_t MKE_I2C_DHT20::resetSensor()
{
  uint8_t count = 255;
  if ((readStatus() & 0x18) != 0x18)
  {
    count++;
    if (_resetRegister(0x1B)) count++;
    if (_resetRegister(0x1C)) count++;
    if (_resetRegister(0x1E)) count++;
    delay(10);
  }
  return count;
}


////////////////////////////////////////////////
//
//  READ THE SENSOR
//
int MKE_I2C_DHT20::read()
{
  //  do not read to fast == more than once per second.
  if (millis() - _lastRead < 1000)
  {
    return MKE_I2C_DHT20_ERROR_LASTREAD;
  }

  int status = requestData();
  if (status < 0) return status;
  //  wait for measurement ready
  uint32_t start = millis();
  while (isMeasuring())
  {
    if (millis() - start >= 1000)
    {
      return MKE_I2C_DHT20_ERROR_READ_TIMEOUT;
    }
    yield();
  }
  //  read the measurement
  status = readData();
  if (status < 0) return status;

  //  convert it to meaningful data
  return convert();
}


int MKE_I2C_DHT20::requestData()
{
  //  reset sensor if needed.
  resetSensor();

  //  GET CONNECTION
  _wire->beginTransmission(MKE_I2C_DHT20_ADDRESS);
  _wire->write(0xAC);
  _wire->write(0x33);
  _wire->write(0x00);
  int rv = _wire->endTransmission();

  _lastRequest = millis();
  return rv;
}


int MKE_I2C_DHT20::readData()
{
  //  GET DATA
  const uint8_t length = 7;
  int bytes = _wire->requestFrom(MKE_I2C_DHT20_ADDRESS, length);

  if (bytes == 0)     return MKE_I2C_DHT20_ERROR_CONNECT;
  if (bytes < length) return MKE_I2C_DHT20_MISSING_BYTES;

  bool allZero = true;
  for (int i = 0; i < bytes; i++)
  {
    _bits[i] = _wire->read();
    //  if (_bits[i] < 0x10) Serial.print(0);
    //  Serial.print(_bits[i], HEX);
    //  Serial.print(" ");
    allZero = allZero && (_bits[i] == 0);
  }
  //  Serial.println();
  if (allZero) return MKE_I2C_DHT20_ERROR_BYTES_ALL_ZERO;

  _lastRead = millis();
  return bytes;
}


int MKE_I2C_DHT20::convert()
{
  //  CONVERT AND STORE
  _status      = _bits[0];
  uint32_t raw = _bits[1];
  raw <<= 8;
  raw += _bits[2];
  raw <<= 4;
  raw += (_bits[3] >> 4);
  _humidity = raw * 9.5367431640625e-5;   // ==> / 1048576.0 * 100%;

  raw = (_bits[3] & 0x0F);
  raw <<= 8;
  raw += _bits[4];
  raw <<= 8;
  raw += _bits[5];
  _temperature = raw * 1.9073486328125e-4 - 50;  //  ==> / 1048576.0 * 200 - 50;

  //  TEST CHECKSUM
  uint8_t _crc = _crc8(_bits, 6);
  //  Serial.print(_crc, HEX);
  //  Serial.print("\t");
  //  Serial.println(_bits[6], HEX);
  if (_crc != _bits[6]) return MKE_I2C_DHT20_ERROR_CHECKSUM;

  return MKE_I2C_DHT20_OK;
}


////////////////////////////////////////////////
//
//  TEMPERATURE & HUMIDITY & OFFSET
//
float MKE_I2C_DHT20::getHumidity()
{
  return _humidity + _humOffset;
};


float MKE_I2C_DHT20::getTemperature()
{
  return _temperature + _tempOffset;
};


void MKE_I2C_DHT20::setHumOffset(float offset)
{
  _humOffset = offset;
};


void MKE_I2C_DHT20::setTempOffset(float offset)
{
  _tempOffset = offset;
};


float MKE_I2C_DHT20::getHumOffset()
{
  return _humOffset;
};


float MKE_I2C_DHT20::getTempOffset()
{
  return _tempOffset;
};


////////////////////////////////////////////////
//
//  STATUS
//
uint8_t MKE_I2C_DHT20::readStatus()
{
  _wire->requestFrom(MKE_I2C_DHT20_ADDRESS, (uint8_t)1);
  delay(1);  //  needed to stabilize timing
  return (uint8_t) _wire->read();
}


bool MKE_I2C_DHT20::isCalibrated()
{
  return (readStatus() & 0x08) == 0x08;
}


bool MKE_I2C_DHT20::isMeasuring()
{
  return (readStatus() & 0x80) == 0x80;
}


bool MKE_I2C_DHT20::isIdle()
{
  return (readStatus() & 0x80) == 0x00;
}


int MKE_I2C_DHT20::internalStatus()
{
  return _status;
};


////////////////////////////////////////////////
//
//  TIMING
//
uint32_t MKE_I2C_DHT20::lastRead()
{
  return _lastRead;
};


uint32_t MKE_I2C_DHT20::lastRequest()
{
  return _lastRequest;
};


////////////////////////////////////////////////
//
//  PRIVATE
//
uint8_t MKE_I2C_DHT20::_crc8(uint8_t *ptr, uint8_t len)
{
  uint8_t crc = 0xFF;
  while(len--)
  {
    crc ^= *ptr++;
    for (uint8_t i = 0; i < 8; i++)
    {
      if (crc & 0x80)
      {
        crc <<= 1;
        crc ^= 0x31;
      }
      else
      {
        crc <<= 1;
      }
    }
  }
  return crc;
}


//  Code based on demo code sent by www.aosong.com
//  no further documentation.
//  0x1B returned 18, 0, 4
//  0x1C returned 18, 65, 0
//  0x1E returned 18, 8, 0
//    18 seems to be status register
//    other values unknown.
bool MKE_I2C_DHT20::_resetRegister(uint8_t reg)
{
  uint8_t value[3];
  _wire->beginTransmission(MKE_I2C_DHT20_ADDRESS);
  _wire->write(reg);
  _wire->write(0x00);
  _wire->write(0x00);
  if (_wire->endTransmission() != 0) return false;
  delay(5);

  int bytes = _wire->requestFrom(MKE_I2C_DHT20_ADDRESS, (uint8_t)3);
  for (int i = 0; i < bytes; i++)
  {
    value[i] = _wire->read();
    //  Serial.println(value[i], HEX);
  }
  delay(10);

  _wire->beginTransmission(MKE_I2C_DHT20_ADDRESS);
  _wire->write(0xB0 | reg);
  _wire->write(value[1]);
  _wire->write(value[2]);
  if (_wire->endTransmission() != 0) return false;
  delay(5);
  return true;
}


// -- END OF FILE --

