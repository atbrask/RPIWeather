/*
Common base class for all sensor types
Copyright (c) 2014 A.T.Brask <atbrask@gmail.com>

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

#ifndef SENSOR_H
#define SENSOR_H

#define POWERPIN PA1
#define DATAPIN PA2

#include "SensorNodeTypes.h"

/**
 * Common base class for all sensor types.
 */
class Sensor
{
protected:
    /**
     * Busy-waits for transitions on the data pin (PA2).
     *
     * @param desiredPinState The desired PA2 pin state to wait for.
     * @param maxMicroseconds How long do we want to wait?
     * @return True if the pin changed before the timeout. False otherwise.
     */
    bool waitWithTimeout(bool desiredPinState, uint16_t maxMicroseconds);

public:
    /**
     * Constructor
     *
     * Sets up PA1 as power pin and PA2 as data pin for the sensor.
     */
    Sensor(void);
    
    /**
     * Initializes the sensor reading buffer.
     *
     * @param The buffer array to be initialized
     */
    virtual void initReadings(SensorReading buffer[]);

    /**
     * Performs a single reading from the sensor.
     *
     * @param buffer The buffer to be updated if the read is successful.
     * @return Whether or not the reading succeeded.
     */
    virtual bool read(SensorReading buffer[]);
};

#endif