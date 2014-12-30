/*
Library for using the popular DHT22 sensor with an ATtiny84 at 1 MHz clock.
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

#include "DHT22.h"

void DHT22::initReadings(SensorReading buffer[])
{
    buffer[0].Type = TemperatureSensor;
    buffer[0].Quality = QualityBadCommFailure;
    buffer[1].Type = HumiditySensor;
    buffer[1].Quality = QualityBadCommFailure;
}

bool DHT22::readPowered(SensorReading buffer[])
{
    // Set data pin as output and pull high. Wait for 250 ms.
    DDRA |= _BV(DATAPIN);
    PORTA |= _BV(DATAPIN);
    _delay_ms(250);
  
    // Pull data pin low for 20 ms.
    PORTA &= ~_BV(DATAPIN);
    _delay_ms(20);

    // Disable interrupts
    cli();
  
    // Set data pin as input and pull high
    DDRA &= ~_BV(DATAPIN);
    PORTA |= _BV(DATAPIN);

    // Read transitions
    uint8_t bit = 0;
    uint8_t data[5] = { 0, 0, 0, 0, 0 };
    uint8_t laststate = _BV(DATAPIN);
    for (uint8_t i = 0; i < 85; i++)
    {
        // Measure the length of each transition
        uint8_t counter = 0;
        while ((PINA & _BV(DATAPIN)) == laststate && ++counter != 255)
            _delay_us(1);

        // Did we encounter a timeout?
        if (counter == 255)
            break;

        laststate = PINA & _BV(DATAPIN);

        // Ignore first 3 transitions and then look at every other transition
        if ((i > 3) && (i % 2 == 0))
        {
            data[bit >> 3] <<= 1;
            if (counter > 1)
                data[bit >> 3] |= 1;
            bit++;
        }
    }

    // Enable interrupts
    sei();

    // Check result and update measurement state
    if (bit == 40 &&
        data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xff))
    {
        int16_t temperature = ((data[2] & 0x7f) << 8) | data[3];
        int16_t humidity = ((data[0] & 0x7f) << 8) | data[1];

        // The DHT22 uses a weird format for negative numbers (single sign bit)
        if (data[2] & 0x80)
            temperature = -temperature;

        // Update buffer object
        buffer[0].Quality = QualityGoodNonSpecific;
        buffer[0].Value = temperature;

        buffer[1].Quality = QualityGoodNonSpecific;
        buffer[1].Value = humidity;
        
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
