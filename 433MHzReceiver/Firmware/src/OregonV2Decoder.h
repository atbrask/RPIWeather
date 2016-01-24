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

#ifndef OREGONV2DECODER_H
#define OREGONV2DECODER_H

#include "BaseDecoder.h"

class OregonV2Decoder : public BaseDecoder
{
private:
    enum { NOSIGNAL, PREAMBLE, HIGH, LOW, WAIT};
    enum { NONE, ZERO, ONE };
    uint8_t signalstate = NOSIGNAL;
    uint8_t prelength = 0;
    uint8_t bitstate = NONE;

    bool addHalfBit(bool value);
public:
    OregonV2Decoder();
    bool decode(uint16_t duration, bool pinstate);
    const char* getName();
};

#endif