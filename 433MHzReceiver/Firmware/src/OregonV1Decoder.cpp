/*
Decoder class for the "Oregon Scientific V1" weather station protocol
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

#include "OregonV1Decoder.h"

OregonV1Decoder::OregonV1Decoder() : BaseDecoder(false, true) { }

// Pre: 12 x (1750 us high + 1100 us low)
//      4250 us low
//      5600 us high
//      5200 us low (followed by high-low)
// -or- 6850 us low (incl start of low-high)

// 1250 us short low
// 2850 us long low
// 1600 us short high
// 3050 us long high

bool OregonV1Decoder::decode(uint16_t duration, bool pinstate)
{
    bool isLong = duration > 2000;

    switch(signalstate)
    {
        case NOSIGNAL: // Look for preamble
        prelength = 0;
        if (duration >= 1400 && duration <= 2000 && pinstate)
        {
            signalstate = PREAMBLE;
            prelength = 1;
        }
        else
            reset();
        return false;

        case PREAMBLE: // Look for long low
        if (duration >= 1000 && duration <= 1600 && !pinstate)
            prelength++;
        else if (duration >= 1400 && duration <= 2000 && pinstate)
            prelength++;
        else if (duration >= 4000 && duration <= 4600 && !pinstate && prelength > 10)
            signalstate = SYNCLOW;
        else
        {
            signalstate = NOSIGNAL;
            reset();
        }
        return false;

        case SYNCLOW: // Look for long high
        if (duration >= 5300 && duration <= 6000 && pinstate)
            signalstate = SYNCHIGH;
        else
        {
            signalstate = NOSIGNAL;
            reset();
        }
        return false;

        case SYNCHIGH: // Look for long low
        if (duration >= 5000 && duration <= 6000 && !pinstate)
            signalstate = WAIT;
        else if (duration > 6000 && duration <= 7500 && !pinstate)
            signalstate = LOW;
        else
        {
            signalstate = NOSIGNAL;
            reset();
        }
        return false;

        case HIGH:
        addBit(true);
        signalstate = isLong ? LOW : WAIT;
        break;

        case LOW:
        addBit(false);
        signalstate = isLong ? HIGH : WAIT;
        break;

        case WAIT:
        if (isLong)
        {
            signalstate = NOSIGNAL;
            reset();
            return false;
        }
        signalstate = pinstate ? HIGH : LOW;
        break;
    }

    if (totalBitCount == 32)
    {
        signalstate = NOSIGNAL;
        return true;
    }
    else
        return false;
}

const char* OregonV1Decoder::getName()
{
    return "OS1";
}