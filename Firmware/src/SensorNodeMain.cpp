/*
Firmware to "atbrask's Sensor Nodes v1.0"
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

#include "RF24/RF24.h"

#include "WDTSleep.h"
#include "Battery.h"
#include "CRC32.h"

#include "DHT22.h"
#include "TX23.h"
#include "WS_2300_16.h"

////////////////////
//                //
//  Global stuff  //
//                //
////////////////////

// We support several sensors, but only one at a time, defined in the Makefile.
SENSORTYPE sensor;
RF24 radio(PA3, PA7);
Battery battery(VCC);
SendBuffer buffer;

////////////
//        //
//  Main  //
//        //
////////////

int main()
{
	// Reset WDT sleep timer
	WDTSleep::init();

    // Set up sensor reading array
    sensor.initReadings(buffer.Readings);

	// Set up nRF24L01 radio module
	radio.begin();
	radio.setPALevel(RF24_PA_MAX);
	radio.setChannel(CHANNEL);
	radio.setRetries(15, 15);
	radio.setDataRate(RF24_1MBPS);
	radio.openWritingPipe(TXPIPE);
	radio.openReadingPipe(1, RXPIPE);
	radio.setAutoAck(true);

    // Loop forever...
    while (true)
	{
        // Read sensor and update send buffer
        sensor.read(buffer.Readings);
        buffer.SequenceNumber++;
        buffer.BatteryVoltage = battery.read();
        buffer.Checksum = CRC32::calculate((uint8_t*)&buffer, 28);

        // Send packet
        radio.write(&buffer, 32);
        radio.powerDown();

        // Zzzz...
        WDTSleep::sleep(264);
	}
}
