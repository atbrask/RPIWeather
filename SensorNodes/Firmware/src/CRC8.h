/*
Standard CRC8 calculation for AVR MCUs
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

#ifndef CRC8_H
#define CRC8_H

#include <stdint.h>

/**
 * Standard CRC8 calculation for AVR MCUs
 */
class CRC8
{
private:
	static uint8_t polynomial;
	static uint8_t checksum;

public:
	/**
	 * Initializes a CRC8 checksum calculation.
	 *
	 * @param polynomial The polynomial used for this particular CRC8.
	 * @param initialValue In some cases an initial value is needed.
	 */
	static void init(uint8_t polynomial, uint8_t initialValue);

	/**
	 * Updates the checksum by adding one byte of data.
	 *
	 * @param value The byte to be added to the checksum calculation.
	 * @return The CRC8 checksum after adding the byte.
	 */
  static uint8_t update(uint8_t value);
};

#endif