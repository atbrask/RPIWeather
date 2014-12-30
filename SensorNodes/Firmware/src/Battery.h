/*
Low-power battery meter using the PA0 ADC on an ATtiny84 at 1 MHz.
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

#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>

/**
 * Low-power battery meter using the PA0 ADC on an ATtiny84.
 * The code expects a 1 MHz system clock and that VCC >= battery voltage.
 *
 * For best results, use a RC low-pass filter to reduce DC/DC switching noise.
 * I used a 1 MOhm resistor and a 100 nF capacitor for this.
 *
 * Try different RC values here:
 * http://sim.okawa-denshi.jp/en/CRlowkeisan.htm
 */
class Battery
{
private:
    /**
     * The ATTiny's VCC in mV. This is our reference voltage.
     */
    uint32_t referenceVoltage;
public:
    /**
     * Constructor
     *
     * Sets PA0 up as an input and configures the ADC.
     *
     * @param vcc The ATTiny's VCC in mV.
     */
    Battery(uint32_t vcc);

    /**
     * Reads battery voltage.
     *
     * @return The current battery voltage in mV.
     */
    uint16_t read(void);
};

#endif