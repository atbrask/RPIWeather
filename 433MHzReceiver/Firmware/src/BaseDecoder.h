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

#ifndef BASEDECODER_H
#define BASEDECODER_H

#define BUFFER_LENGTH 25
#include <string.h>
#include <stdint.h>

class BaseDecoder
{
private:
    bool msbFirst, swapNibbles;
protected:
    uint8_t totalBitCount, bitIndex, byteIndex, buffer[BUFFER_LENGTH];
    void addBit(bool value);
public:
    BaseDecoder(bool msbFirst, bool swapNibbles);
    virtual bool decode(uint16_t duration, bool pinstate);
    virtual const char* getName();
    void reset();
    uint8_t getData(uint8_t* copybuffer);
};

#endif