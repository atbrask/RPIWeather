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

#include "WDTSleep.h"

#include "PowerManagedSensor.h"

PowerManagedSensor::PowerManagedSensor(void)
{
    // Set power pin to output and power down the device
    DDRA |= _BV(POWERPIN);
    powerDown();
}

void PowerManagedSensor::powerUp(void)
{
    // Set data pin to input and pull up
    DDRA &= ~_BV(DATAPIN);
    PORTA |= _BV(DATAPIN);
    
    // Set power pin high
    PORTA |= _BV(POWERPIN);
}

void PowerManagedSensor::powerDown(void)
{
    // Set power pin low
    PORTA &= ~_BV(POWERPIN);

    // Set data pin to input and pull down
    DDRA &= ~_BV(DATAPIN);
    PORTA &= ~_BV(DATAPIN);
}

void PowerManagedSensor::read(SensorReading buffer[])
{
    powerUp();
    WDTSleep::sleep(8);
    readPowered(buffer);
    powerDown();
}
