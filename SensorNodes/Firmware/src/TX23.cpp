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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "TX23.h"

void TX23::initReadings(SensorReading buffer[])
{
    buffer[0].Type = WindSpeedSensor;
    buffer[0].Quality = QualityBadCommFailure;
    buffer[1].Type = WindDirectionSensor;
    buffer[1].Quality = QualityBadCommFailure;
}

bool TX23::readPowered(SensorReading buffer[])
{
    // Set data pin as output and pull high. Wait for 100 ms.
    DDRA |= _BV(DATAPIN);
    PORTA |= _BV(DATAPIN);
    _delay_ms(100);
  
    // Pull data pin low for 500 ms.
    PORTA &= ~_BV(DATAPIN);
    _delay_ms(500);

    // Disable interrupts
    cli();
  
    // Set data pin as input and pull high
    DDRA &= ~_BV(DATAPIN);
    PORTA |= _BV(DATAPIN);

    // Wait for the header bits...
    if (!waitWithTimeout(true,  30000) || // Wait for 0->1
        !waitWithTimeout(false, 30000) || // Wait for 1->0
        !waitWithTimeout(true,  30000))   // Wait for 0->1 (start of header)
    {
        // If this happens, the sensor has malfunctioned or is disconnected
        if (buffer[0].Quality == QualityGoodNonSpecific)
            buffer[0].Quality = QualityBadLastKnownValue;

        if (buffer[1].Quality == QualityGoodNonSpecific)
            buffer[1].Quality = QualityBadLastKnownValue;

        return false;
    }

    // Read 41 bits (LSB first)
    // The offset of 3 bits is to make the first byte header-only (it's 5 bits)
    uint8_t data[6] = { 0, 0, 0, 0, 0, 0 };
    for (uint8_t i = 3; i < 44; i++)
    {
        data[i >> 3] |= (PINA & _BV(DATAPIN)) ? _BV(i % 8) : 0;
        _delay_us(1185); // Good values: 1171...1199
    }

    // Enable interrupts
    sei();

    // Extract data and calculate checksum
    uint8_t header = data[0] >> 3;
    uint8_t windDirection = data[1] & 0xf;
    uint16_t windSpeed = ((data[1] >> 4) & 0xf) | (data[2] << 4);
    uint8_t checksum = data[3] & 0xf;
    uint8_t invertedWindDirection = (data[3] >> 4) & 0xf;
    uint16_t invertedWindSpeed = data[4] | ((data[5] & 0xf) << 8);

    uint8_t calculatedChecksum = (windDirection + 
                                  ((windSpeed >> 8) & 0xf) + 
                                  ((windSpeed >> 4) & 0xf) + 
                                  (windSpeed & 0xf)) & 0xf;

    // We need to verify a few things here:
    // 1) Is the header correct? (i.e. 11011)
    // 2) Is the checksum correct?
    // 3) Are the two data sets the inverted mirror of another?
    if (header == 0b11011 &&
        checksum == calculatedChecksum &&
        (windSpeed ^ invertedWindSpeed) == 0b111111111111 &&
        (windDirection ^ invertedWindDirection) == 0b1111)
    {
        // If it all checks out, the read was successful!
        // Note: The wind vane has 4 bits of resolution:
        //       360 degrees / 16 directions = 22.5 degrees/direction

        // Update buffer object
        buffer[0].Quality = QualityGoodNonSpecific;
        buffer[0].Value = windSpeed;
        buffer[1].Quality = QualityGoodNonSpecific;
        buffer[1].Value = windDirection * 225;
        return true;
    }
    else
    {
        if (buffer[0].Quality == QualityGoodNonSpecific)
            buffer[0].Quality = QualityBadLastKnownValue;

        if (buffer[1].Quality == QualityGoodNonSpecific)
            buffer[1].Quality = QualityBadLastKnownValue;
        
        return false;
    }
}
