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

#include "CRC8.h"

uint8_t CRC8::polynomial;
uint8_t CRC8::checksum;

void CRC8::init(uint8_t polynomial, uint8_t initialValue)
{
	CRC8::polynomial = polynomial;
	CRC8::checksum = initialValue;
}

uint8_t CRC8::update(uint8_t value)
{
	CRC8::checksum ^= value;
	for (uint8_t i = 0; i < 8; i++)
	{
		if (CRC8::checksum & 0x80)
			CRC8::checksum = (CRC8::checksum << 1) ^ CRC8::polynomial;
		else
			CRC8::checksum <<= 1;
	}
	return checksum;
}