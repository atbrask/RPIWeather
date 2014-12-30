/*
Base class for all power managed 1-wire sensors
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

#include <avr/io.h>
#include <util/delay.h>

#include "Sensor.h"

Sensor::Sensor(void)
{
    // Set power pin to output and power up
    DDRA |= _BV(POWERPIN);
    PORTA |= _BV(POWERPIN);

    // Set data pin to input and pull down
    DDRA &= ~_BV(DATAPIN);
    PORTA &= ~_BV(DATAPIN);
}

bool Sensor::waitWithTimeout(bool desiredPinState, uint16_t maxMicroseconds)
{
    while ((bool)(PINA & _BV(DATAPIN)) != desiredPinState)
    {
        _delay_us(1);
        if (--maxMicroseconds == 0)
            return false;
    }
    return true;
}

void Sensor::initReadings(SensorReading buffer[])
{
    return;
}

bool Sensor::read(SensorReading buffer[])
{
    return false;
}
