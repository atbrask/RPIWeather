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

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include "WDTSleep.h"

volatile uint8_t WDTSleep::sleepIntervals;

ISR(WDT_vect)
{
   	WDTSleep::sleepIntervals--;
}

void WDTSleep::init(void)
{
  	wdt_reset();
  	wdt_disable();
}

void WDTSleep::sleep(uint16_t seconds)
{
	// Start WDT
	cli();
  	WDTCSR = _BV(WDCE) | _BV(WDE);              // Enable WDT
  	WDTCSR = _BV(WDIE) | _BV(WDP3) | _BV(WDP0); // Set WDT timeout to 8 secs
  	sei();
	
	// Sleep...
	sleepIntervals = seconds >> 3;
	while(sleepIntervals)
	{
        set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
        sleep_enable();
        sleep_cpu ();  
        // cancel sleep as a precaution
        sleep_disable();
	}

	// Stop WDT
	wdt_disable();
}
