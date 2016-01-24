/*
Base class for the various weather station protocols
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

#include "BaseDecoder.h"

BaseDecoder::BaseDecoder(bool msbFirst, bool swapNibbles)
: msbFirst(msbFirst), swapNibbles(swapNibbles)
{
    reset(); 
}

void BaseDecoder::addBit(bool value)
{
    totalBitCount++;
    if (msbFirst)
        buffer[byteIndex] |= (value << (7-bitIndex));
    else
        buffer[byteIndex] |= (value << bitIndex);
    bitIndex++;

    if (bitIndex == 8)
    {
        if (swapNibbles)
            buffer[byteIndex] = (buffer[byteIndex] << 4) | (buffer[byteIndex] >> 4);
        bitIndex = 0;
        byteIndex++;
        if (byteIndex == BUFFER_LENGTH)
            reset();
    }
}

bool BaseDecoder::decode(uint16_t duration, bool pinstate)
{
    // Dummy implementation
    return false;
}

const char* BaseDecoder::getName()
{
    return "?";
}

void BaseDecoder::reset()
{
    for(byteIndex = 0; byteIndex < BUFFER_LENGTH; byteIndex++)
        buffer[byteIndex] = 0;

    totalBitCount = 0;
    bitIndex = 0;
    byteIndex = 0;
}

uint8_t BaseDecoder::getData(uint8_t* copybuffer)
{
    memcpy(copybuffer, buffer, byteIndex);
    return byteIndex;
}
