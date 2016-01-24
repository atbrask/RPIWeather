/*
Firmware to "433MHz Receiver v1.0"
Copyright (c) 2016 A.T.Brask <atbrask@gmail.com>

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
#include <stdint.h>

#include "BasicSerial3.h"
#include "TX3Decoder.h"
#include "ChinaDecoder.h"
#include "OregonV1Decoder.h"
#include "OregonV2Decoder.h"
#include "CRC32.h"

// GLOBAL VARIABLES

TX3Decoder tx3;
ChinaDecoder china;
OregonV1Decoder orscV1;
OregonV2Decoder orscV2;

volatile uint16_t pulse;
volatile uint16_t micros;
volatile bool pinstate;

// SERIAL OUTPUT

void writeString(const char* str)
{
   while (*str)
    TxByte (*str++);
}

void writeHexNibble(uint8_t nibble)
{
    nibble &= 0xf;
    if (nibble < 10)
        TxByte(nibble+48); // 0...9
    else
        TxByte(nibble+55); // A...F
}

void writeHexByte(uint8_t byte)
{
    writeHexNibble(byte >> 4);
    writeHexNibble(byte & 0xf);
}

void writeSerial(BaseDecoder& decoder) 
{
    uint8_t data[25];
    uint8_t length = decoder.getData(data);
    uint32_t crc = CRC32::calculate((uint8_t*)&data, length);

    // Write decoder name
    writeString(decoder.getName());
    TxByte(' ');

    // Write received data
    for (uint8_t i = 0; i < length; ++i)
        writeHexByte(data[i]);
    TxByte(' ');

    // Write CRC32
    writeHexByte((uint8_t)((crc >> 24) & 0xff));
    writeHexByte((uint8_t)((crc >> 16) & 0xff));
    writeHexByte((uint8_t)((crc >> 8) & 0xff));
    writeHexByte((uint8_t)(crc & 0xff));

    TxByte('\n');
    
    decoder.reset();
}

// RADIO INPUT

void initRadioListener()
{
    // Set up listener interrupt on PA2
    DDRA &= ~_BV(PA2);     // Set ADC1 (pin 12) as input
    PORTA |= _BV(PA2);     // Enable pull-up
    PCMSK0 |= _BV(PCINT2); // Pin change mask: listen to PA2 
    GIMSK |= _BV(PCIE0);   // Enable PCINT interrupt 
}

ISR(PCINT0_vect)
{
    static uint16_t last;
    pulse = micros - last;
    last += pulse;
    pinstate = !(PINA & _BV(PA2));
}

// TIMER

void initTimer()
{
    // Set up timer 0
    TCCR0A = _BV(WGM01);   // CTC mode
    TCCR0B = _BV(CS01);    // Div8
    OCR0A = 49;            // 50us compare value
    TIMSK0 |= _BV(OCIE0A); // Enable timer interrupt
}

ISR (TIM0_COMPA_vect)
{
    micros+=50;
}

// MAIN

int main()
{
    writeString("[433MHzReceiver]\n");

    initRadioListener();
    initTimer();

    while(true)
    {
        cli();
        uint16_t p = pulse;
        pulse = 0;
        bool s = pinstate;
        sei();
    
        if (p != 0) 
        {
            if (tx3.decode(p, s))
                writeSerial(tx3);
            if (china.decode(p, s))
                writeSerial(china);
            if (orscV1.decode(p, s))
                writeSerial(orscV1);  
            if (orscV2.decode(p, s))
                writeSerial(orscV2);  
        }
    }
}
