/*
Library for using a HTU21D sensor with an ATtiny84 at 1 MHz clock.
Copyright (c) 2016 A.T.Brask <atbrask@gmail.com>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT 
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "HTU21D.h"
#include "i2cmaster.h"
#include "CRC8.h"

HTU21D::HTU21D()
{
  i2c_init();
  softReset();
}

void HTU21D::softReset(void)
{
  i2c_start_wait(HTU21D_I2CADDRESS | I2C_WRITE);
  i2c_write(HTU21D_SOFTRESET);
  i2c_stop();
}

int16_t HTU21D::readTemperature(void) 
{  
  // Request temperature reading
  i2c_start_wait(HTU21D_I2CADDRESS | I2C_WRITE);
  i2c_write(HTU21D_READTEMP);
  i2c_rep_start(HTU21D_I2CADDRESS | I2C_READ);
  uint8_t highbyte = i2c_readAck();
  uint8_t lowbyte = i2c_readAck();
  uint8_t crc = i2c_readNak();
  i2c_stop();

  // Validate CRC8
  CRC8::init(0x31, 0);
  CRC8::update(highbyte);
  if (CRC8::update(lowbyte) != crc)
    return INT16_MIN;

  // Convert to temperature using magic numbers from datasheet
  uint32_t temp = ((uint16_t)highbyte << 8) | lowbyte;
  temp = ((temp * 17572) >> 16) - 4685;

  return (int16_t)temp;
}

int16_t HTU21D::readHumidity(void) 
{
  // Request humidity reading
  i2c_start_wait(HTU21D_I2CADDRESS | I2C_WRITE);
  i2c_write(HTU21D_READHUM);
  i2c_rep_start(HTU21D_I2CADDRESS | I2C_READ);
  uint8_t highbyte = i2c_readAck();
  uint8_t lowbyte = i2c_readAck();
  uint8_t crc = i2c_readNak();
  i2c_stop();

  // Validate CRC8
  CRC8::init(0x31, 0);
  CRC8::update(highbyte);
  if (CRC8::update(lowbyte) != crc)
    return INT16_MIN;

  // Convert to humidity using magic numbers from datasheet
  uint32_t hum = ((uint16_t)highbyte << 8) | lowbyte;
  hum = ((hum * 12500) >> 16) - 600;

  return hum;
}

void HTU21D::initReadings(SensorReading buffer[])
{
    buffer[0].Type = TemperatureSensor;
    buffer[0].Quality = QualityBadCommFailure;
    buffer[1].Type = HumiditySensor;
    buffer[1].Quality = QualityBadCommFailure;
}

void HTU21D::read(SensorReading buffer[])
{
  // Update temperature
  int16_t temperature = readTemperature();
  if (temperature > INT16_MIN)
  {
    buffer[0].Quality = QualityGoodNonSpecific;
    buffer[0].Value = temperature / 10;
  }
  else if (buffer[0].Quality == QualityGoodNonSpecific)
    buffer[0].Quality = QualityBadLastKnownValue;

  // Update humidity
  int16_t humidity = readHumidity();
  if (humidity > INT16_MIN)
  {
    buffer[1].Quality = QualityGoodNonSpecific;
    buffer[1].Value = humidity / 10;
  }
  else if (buffer[1].Quality == QualityGoodNonSpecific)
    buffer[1].Quality = QualityBadLastKnownValue;

}