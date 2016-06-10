/*
Library for using a WS-2300-16 rain gauge with an ATtiny84.
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
#ifndef WS_2300_16_H
#define WS_2300_16_H

#include "Sensor.h"

extern "C" void PCINT0_vect(void) __attribute__ ((signal));

/**
 * Library for using a WS-2300-16 rain gauge with an ATtiny84. It works by
 * counting the tips of the seesaw within the rain gauge. On each tip, PA2 is
 * pulled low by a reed relay. When PA2 isn't pulled low by the rain gauge,
 * it's pulled high by PA1 through a 10k pull-up resistor. It turns out that
 * we get the most reliable results by counting the low->high pin change event
 * that occurs after the tip. Each tip represents approx 1/2 mm of rain.
 */
class WS_2300_16: public Sensor
{
private:
    /**
     * The sensor tip count.
     */
    static volatile uint16_t tips;

public:
    /**
     * We need to include the PCINT0 ISR call as a friend in order to
     * record the pin change event within the WS_2300_16 object.
     */
    friend void PCINT0_vect(void);

    /**
     * Constructor
     *
     * Sets up the pin change interrupt to listen for tips of the seesaw within
     * the rain gauge. The device is set up as being permanently powered.
     *
     * Sets up PA1 as power pin and PA2 as data pin for the sensor.
     */
    WS_2300_16(void);

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