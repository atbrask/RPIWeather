/*
Library for using a WS-2300-16 rain gauge with an ATtiny84 at 1 MHz clock.
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
#include <avr/interrupt.h>
#include "WS_2300_16.h"

volatile uint16_t WS_2300_16::tips;

WS_2300_16::WS_2300_16() : Sensor()
{
    // Set power pin to output and power up
    DDRA |= _BV(POWERPIN);
    PORTA |= _BV(POWERPIN);

    // Set data pin to input and pull down
    DDRA &= ~_BV(DATAPIN);
    PORTA &= ~_BV(DATAPIN);

    tips = 0;
    PCMSK0 |= (1<<PCINT2);   // pin change mask: listen to PA2 
    GIMSK |= (1<<PCIE0);     // enable PCINT interrupt 
}

ISR(PCINT0_vect)
{
    if (PINA & _BV(DATAPIN))
        WS_2300_16::tips++;
} 

void WS_2300_16::initReadings(SensorReading buffer[])
{
    buffer[0].Type = RainGaugeSensor;
    buffer[0].Quality = QualityBadCommFailure;
}

void WS_2300_16::read(SensorReading buffer[])
{
    buffer[0].Value = tips * 5; // Each tip is approx 1/2 mm
    buffer[0].Quality = QualityGoodNonSpecific;
}
