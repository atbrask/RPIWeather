/*
Simple low-power WDT-based sleep library for ATtiny84.
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

#ifndef WDTSLEEP_H
#define WDTSLEEP_H

#include <stdint.h>

extern "C" void WDT_vect(void) __attribute__ ((signal));

/**
 * Simple low-power WDT-based sleep library for ATTiny84. Due to the nature of
 * the WDT on the AVR platform, it sleeps in intervals of eight seconds each.
 * Furthermore, the WDT is temperature-dependent, so it's also very imprecise.
 *
 * I got the inspiration from some forum posts here:
 * http://www.gammon.com.au/forum/?id=11497
 */
class WDTSleep
{
private:
	/**
	 * The amount of eight second intervals left till the sleep is over.
	 */
	static volatile uint8_t sleepIntervals;
public:
	/**
     * We need to include the WDT ISR call as a friend in order to react to the
     * timeout event in the WDTSleep class.
     */
	friend void WDT_vect(void);

	/**
	 * Resets and disables the WDT. This is meant to be called immediately upon
	 * power-up to act as a safety mechanism against reset-loops.
	 */
	static void init(void);

	/**
	 * The MCU is put to sleep for the specified time duration. The number of
	 * seconds is rounded down to nearest integer multiple of 8.
	 *
	 * @param seconds The duration we want the MCU to sleep for.
	 */
	static void sleep(uint16_t seconds);
};

#endif