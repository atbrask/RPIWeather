/*
Base class for all sensor types that benefit from power management
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

#ifndef POWERMANAGEDSENSOR_H
#define POWERMANAGEDSENSOR_H

#include "Sensor.h"

/**
 * Common base class for all sensor types. It includes some methods for simple
 * power management.
 */
class PowerManagedSensor: public Sensor
{
private:
    /**
     * Turns on the sensor.
     */
    void powerUp(void);

    /**
     * Turns off the sensor.
     */
    void powerDown(void);

protected:
    /**
     * Performs a single reading from the sensor after it has been powered up.
     *
     * @param buffer The buffer to be updated if the read is successful.
     */
    virtual void readPowered(SensorReading buffer[]) { }

public:
    /**
     * Constructor
     *
     * Sets up PA1 as power pin and PA2 as data pin for the sensor.
     * The sensor is initially not powered up.
     */
    PowerManagedSensor(void);

    /**
     * Performs a single reading from the sensor including power management.
     *
     * @param buffer The buffer to be updated if the read is successful.
     */
    void read(SensorReading buffer[]);
};

#endif