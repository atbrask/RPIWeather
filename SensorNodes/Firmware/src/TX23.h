/*
Library for using a TX23 anemometer with an ATtiny84 at 1 MHz clock.
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

#ifndef TX23_H
#define TX23_H

#include "PowerManagedSensor.h"

/**
 * Library for using a La Crosse TX23 anemometer with an ATtiny84 at 1 MHz
 * clock. Other setups may require tweaks.
 *
 * The code is based on information found here:
 * http://www.rd-1000.com/chpm78/lacrosse/Lacrosse_TX23_protocol.html
 * http://www.john.geek.nz/2012/08/la-crosse-tx23u-anemometer-communication-protocol/
 */
class TX23: public PowerManagedSensor 
{
private:
    /**
     * Busy-waits for transitions on the data pin (PA2).
     *
     * @param desiredPinState The desired PA2 pin state to wait for.
     * @param maxMicroseconds How long do we want to wait?
     * @return True if the pin changed before the timeout. False otherwise.
     */
    bool waitWithTimeout(bool desiredPinState, uint16_t maxMicroseconds);
protected:
    /**
     * Performs a single reading from the sensor.
     *
     * @param buffer The buffer to be updated if the read is successful.
     */
    void readPowered(SensorReading buffer[]);
public:
    /**
     * Initializes the sensor reading buffer.
     *
     * @param The buffer array to be initialized
     */
    void initReadings(SensorReading buffer[]);
};

#endif