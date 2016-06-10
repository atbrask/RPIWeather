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

#ifndef HTU21D_H
#define HTU21D_H

#include "Sensor.h"

#define HTU21D_I2CADDRESS   0x80
#define HTU21D_READTEMP     0xE3
#define HTU21D_READHUM      0xE5
#define HTU21D_WRITEUSERREG 0xE6
#define HTU21D_READUSERREG  0xE7
#define HTU21D_SOFTRESET    0xFE

class HTU21D : public Sensor
{
private:
	/**
	 * Performs a soft reset of the HTU21D sensor.
	 */
  void softReset(void);

  /**
   * Performs a temperature reading
   *
   * @return The current temperature in 1/100 degrees Celcius
   */
  int16_t readTemperature(void);

  /**
   * Performs a humidity reading
   *
   * @return The current relative humidity in 1/100%
   */
  int16_t readHumidity(void);
public:
	/**
	 * Constructor.
	 *
	 * Sets up the bit-banged i2c connection and resets the sensor.
	 */
	 HTU21D(void);

  /**
   * Initializes the sensor reading buffer.
   *
   * @param The buffer array to be initialized
   */
  void initReadings(SensorReading buffer[]);

  /**
   * Performs a single reading from the sensor.
   *
   * @param buffer The buffer to be updated if the read is successful.
   */
  void read(SensorReading buffer[]);
};

#endif