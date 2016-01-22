# RPIWeather Data Gateway
# Copyright (c) 2014-2016 A.T.Brask <atbrask@gmail.com>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT 
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 
# 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

import binascii
import struct
import sys
import threading
import time

import nrf24

sensortypes = {1: "temperature",
               2: "humidity",
               3: "windSpeed",
               4: "windDirection",
               5: "rainfall"}

class Measurement:
    def __init__(self, data):
        self.sensortype = data[0]
        self.quality = data[1]
        self.value = data[2] / 10.0

class DataPacket:
    def __init__(self, data):
        self.creationTime = int(time.time())
        self.magicNumber = data[0]
        self.firmwareVersion = data[1]
        self.unitID = data[2]
        self.sequenceNumber = data[3]
        self.batteryVoltage = data[4] / 1000.0
        chunks = [data[idx:idx+3] for idx in range(5, 17, 3)]
        self.measurements = [Measurement(chunk) for chunk in chunks]
        self.crc = data[17]

class RF24Receiver:
    def __init__(self, spiMajor, spiMinor, cePin, irqPin, channel, writePipe, readPipe, magicnumber, dataqueue, logger):
        self.magicnumber = magicnumber
        self.dataqueue = dataqueue
        self.logger = logger

        self.logger.info("Connecting to RF24 radio on the SPI bus...")
        self.radio = nrf24.NRF24()
        self.radio.begin(spiMajor, spiMinor, cePin, irqPin)
        self.radio.setRetries(15, 15)
        self.radio.setPayloadSize(32)
        self.radio.setChannel(channel)
        self.radio.setDataRate(nrf24.NRF24.BR_1MBPS)
        self.radio.setPALevel(nrf24.NRF24.PA_MAX)
        self.radio.setAutoAck(1)
        self.radio.openWritingPipe(writePipe)
        self.radio.openReadingPipe(1, readPipe)
        self.radio.startListening()
        self.logger.info("RF24 radio connected!")

    def start(self):
        self.logger.info("Starting RF24 listening thread...")
        thread = threading.Thread(target = self.loop)
        thread.daemon = True
        thread.start()
        self.logger.info("RF24 listening thread started!")

    def loop(self):
        recv_buffer = []
        while True:
            try:
                if self.radio.available([]):
                    recv_buffer = []
                    self.radio.read(recv_buffer)
                    data = self.processPacket(recv_buffer)
                    self.dataqueue.put(data)
                time.sleep(0.01)
            except:
                self.logger.error("RF24 error: '{0}' (Last data = {1})".format(sys.exc_info()[1].message, recv_buffer))
                time.sleep(10)

    def processPacket(self, recv_buffer):
        # Unpack 32 bytes (while verifying the length)
        rawdata = struct.unpack("<IHHHHBBhBBhBBhBBhI", bytearray(recv_buffer))
        packet = DataPacket(rawdata)

        # Verify header
        if packet.magicNumber != self.magicnumber:
            raise Exception("Wrong magic number!")

        # Verify CRC32
        crc_string = ''.join([chr(byte) for byte in recv_buffer[:28]])
        calculatedcrc = binascii.crc32(crc_string) & 0xffffffff
        if packet.crc != calculatedcrc:
            raise Exception("Bad CRC32!")

        # Create dict from received packet
        result = {
            "id" : "RF24-{0:04X}".format(packet.unitID),
            "time" : packet.creationTime,
            "messageNumber" : packet.sequenceNumber,
            "batteryVoltage" : packet.batteryVoltage
        }
        for measurement in packet.measurements:
            if measurement.sensortype == 0:
                continue
            sensor = sensortypes[measurement.sensortype]
            result[sensor] = measurement.value
            result[sensor + "Quality"] = measurement.quality

        return result
