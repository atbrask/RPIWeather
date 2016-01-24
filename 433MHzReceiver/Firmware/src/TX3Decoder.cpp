/*
Decoder class for the "La Crosse TX3" weather station protocol
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

#include "TX3Decoder.h"

TX3Decoder::TX3Decoder() : BaseDecoder(true, false) { }

bool TX3Decoder::decode(uint16_t duration, bool pinstate) 
{
    if (pinstate)
    {
            // 500us high = 1
        if (duration >= 400 && duration <= 600)
        {
            addBit(true);
            return checkBits();
        }
            // 1300us high = 0
        else if (duration >= 1200 && duration <= 1400)
        {
            addBit(false);
            return checkBits();
        }
        else
        {
            reset();
            return false;
        }
    }
    else
    {
            // 950us low = bit delimiter
        if (duration < 850 || duration > 1050)
            reset();
        return false;
    }    
}

bool TX3Decoder::checkBits()
{
    if (totalBitCount == 44)
    {
        for (uint8_t i = 0; i < 4; i++)
            addBit(false);
        return true;
    }
    else
        return false;
}

const char* TX3Decoder::getName()
{
    return "TX3";
}