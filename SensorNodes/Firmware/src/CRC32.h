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

#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>

/**
 * Standard CRC32 calculation for AVR MCUs
 */
class CRC32
{
public:
	/**
	 * Calculates a CRC32 checksum for the given buffer.
	 *
	 * @param buffer[] The data we want a CRC32 checksum for.
	 * @param len The length of the given buffer.
	 * @return The CRC32 checksum.
	 */
    static uint32_t calculate(uint8_t buffer[], uint8_t len);
};

#endif