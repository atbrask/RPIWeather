/*
Standard CRC32 calculation for AVR MCUs
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

#include <avr/pgmspace.h>
#include "CRC32.h"

const uint32_t crc_table[16] PROGMEM =
{
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

uint32_t CRC32::calculate(uint8_t buffer[], uint8_t len)
{
    uint32_t crc = ~0L;
    for (uint8_t i = 0; i < len; i++)
    {
        crc ^= buffer[i];
        crc = (crc >> 4) ^ pgm_read_dword_near(crc_table + (crc & 0xf));
        crc = (crc >> 4) ^ pgm_read_dword_near(crc_table + (crc & 0xf));
    }
    return ~crc;
}
