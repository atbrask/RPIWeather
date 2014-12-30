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

#include "Battery.h"
#include <avr/io.h>

Battery::Battery(uint32_t vcc)
 : referenceVoltage(vcc)
{
    DDRA &= ~_BV(PA0); // Set PA0 as input
    ADMUX &= 0xc0;     // Set lower six bits low => ADC0 on PA0
    ADCSRA = 0;        // Turn off ADC
}

uint16_t Battery::read(void)
{
    ADCSRA = _BV(ADEN) |                   // ADC Enable
             _BV(ADPS1) | _BV(ADPS0) |     // Set ADC prescaler to 8
                                           // (128 kHz @ 1 MHz system clock)
             _BV(ADSC);                    // ADC Start Conversion
    while(ADCSRA & _BV(ADSC));             // Wait for ADC reading to complete
    ADCSRA = 0;                            // Turn off ADC
    return (ADC * referenceVoltage) >> 10; // Return normalized result
}
