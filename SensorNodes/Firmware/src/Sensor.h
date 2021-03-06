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
 * Common base interface for all sensor types.
 */
class Sensor
{
public:
    /**
     * Initializes the sensor reading buffer.
     *
     * @param The buffer array to be initialized
     */
    virtual void initReadings(SensorReading buffer[]) { };

    /**
     * Performs a single reading from the sensor.
     *
     * @param buffer The buffer to be updated if the read is successful.
     */
    virtual void read(SensorReading buffer[]) { };
};

#endif