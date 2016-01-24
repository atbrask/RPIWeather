/*
Decoder class for the "Oregon Scientific V2" weather station protocol
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

#include "OregonV2Decoder.h"

OregonV2Decoder::OregonV2Decoder() : BaseDecoder(false, true) {}

bool OregonV2Decoder::addHalfBit(bool value)
{
    switch(bitstate)
    {
        case NONE:
        bitstate = value ? ONE : ZERO;
        return true;
        case ZERO:
        bitstate = NONE;
        if (value)
        {
            addBit(true);
            return true;
        }
        else
            return false;
        case ONE:
        bitstate = NONE;
        if (!value)
        {
            addBit(false);
            return true;
        }
        else
            return false;
    }
    return false;
}

bool OregonV2Decoder::decode(uint16_t duration, bool pinstate)
{
    if (duration < 200 || duration > 1400)
    {
        signalstate = NOSIGNAL;
        bitstate = NONE;
        if (totalBitCount > 71)
            return true;
        else
        {
            reset();
            return false;
        }
    }

    bool isLong = duration > 800;

    switch (signalstate)
    {
        case NOSIGNAL: // Look for preamble
        prelength = 0;
        if (isLong)
        {
            signalstate = PREAMBLE;
            prelength = 1;
        }
        else
            reset();
        return false;
        case PREAMBLE:
        if (isLong)
            prelength++;
        else if (prelength <= 32 && !pinstate)
            signalstate = WAIT;
        else
        {
            signalstate = NOSIGNAL;
            reset();
        }
        return false;
        case HIGH:
        if (addHalfBit(true))
            signalstate = isLong ? LOW : WAIT;
        else
        {
            signalstate = NOSIGNAL;
            reset();
        }
        return false;
        case LOW:
        if (addHalfBit(false))
            signalstate = isLong ? HIGH : WAIT;
        else
        {
            signalstate = NOSIGNAL;
            reset();
        }
        return false;
        case WAIT:
        if (isLong)
        {
            signalstate = NOSIGNAL;
            bitstate = NONE;
            if (totalBitCount > 71)
                return true;
            else
            {
                reset();
                return false;
            }
        }
        else
        {
            signalstate = pinstate ? HIGH : LOW;
            return false;
        }
    }
    return false;
}

const char* OregonV2Decoder::getName()
{
    return "OS2";
}