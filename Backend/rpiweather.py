#!/usr/bin/python

# RPIWeather Data Gateway
# Copyright (c) 2014 A.T.Brask <atbrask@gmail.com>
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

import time, struct, binascii, sys
import logging, logging.handlers
from Queue import Queue
from threading import Thread
from influxdb import client as influxdb
from nrf24 import NRF24
import Adafruit_BMP.BMP085 as BMP

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
    def __init__(self, radio, dataqueue, magicnumber, logger):
        self.radio = radio
        self.dataqueue = dataqueue
        self.magicnumber = magicnumber
        self.logger = logger

    def printDetails(self):
        self.radio.stopListening()
        self.radio.printDetails()
        self.radio.startListening()

    def start(self):
        recv_buffer = []
        while True:
            try:
                if radio.available():
                    recv_buffer = []
                    radio.read(recv_buffer)
                    data = self.processPacket(recv_buffer)
                    self.dataqueue.put(data)
                time.sleep(0.01)
            except:
                self.logger.error("Receive error: '{0}' (Last data = {1})".format(sys.exc_info()[1].message, recv_buffer))
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

        return packet

class BMPSampler:
    def __init__(self, sensor, unitid, dataqueue, interval, altitude, logger):
        self.sensor = sensor
        self.unitid = unitid
        self.dataqueue = dataqueue
        self.interval = interval
        self.altitude = altitude
        self.logger = logger
        self.sequenceNumber = 0

    def start(self):
        while True:
            try:
                pressure = self.sensor.read_sealevel_pressure(altitude_m=self.altitude) / 100.0
                data = self.processPacket(pressure)
                self.dataqueue.put(data)
                self.sequenceNumber += 1
                time.sleep(self.interval)
            except:
                self.logger.error("BMP085/180 error: '{0}'".format(sys.exc_info()[1].message))
                time.sleep(10)


    def processPacket(self, pressure):
        # Imitate a sensor node
        return DataPacket([-1,
                           -1, 
                           self.unitid, 
                           self.sequenceNumber,
                           -1,
                           [[6, pressure, 0xc0], [0,0,0], [0,0,0], [0,0,0]],
                           -1])

class InfluxDBSender:
    def __init__(self, db, dataqueue, sensortypes, includeQuality, logger):
        self.db = db
        self.dataqueue = dataqueue
        self.sensortypes = sensortypes
        self.includeQuality = includeQuality
        self.logger = logger

    def start(self):
        data = None
        while True:
            try:
                if data is None:
                    packet = self.dataqueue.get()
                    data = self.processPacket(packet)
                self.db.write_points(data)
                # If the write fails, the following line is skipped.
                data = None
            except:
                self.logger.error("Send error '{0}' (Buffered packets: {1})".format(sys.exc_info()[1].message, self.dataqueue.qsize() + 1))
                time.sleep(10)

    def processPacket(self, packet):
        name = "Sensor-{0:04X}".format(packet.unitID)
        columns = ["time", "messageNumber"]
        points = [packet.creationTime, packet.sequenceNumber]

        if packet.batteryVoltage > -1:
            columns.append("batteryVoltage")
            points.append(packet.batteryVoltage)

        for measurement in packet.measurements:
            if measurement.sensortype > 0:
                columnName = self.sensortypes[measurement.sensortype]
                columns.append(columnName)
                points.append(measurement.value)
                if self.includeQuality:
                    columns.append(columnName + "Quality")
                    points.append(measurement.quality)

        return [{"name": name, "columns": columns, "points": [points]}]

if __name__ == "__main__":
    # Radio configuration
    spiMajor  = 0
    spiMinor  = 0
    cePin     = 22
    irqPin    = 18
    readPipe  = [0x20, 0x13, 0x09, 0x28, 0x01]
    writePipe = [0x20, 0x13, 0x09, 0x28, 0x02]
    channel   = 0x5c

    # Database configuration
    host     = "192.168.1.251"
    port     = 8086
    username = "root"
    password = "root"
    database = "RPIWeather"

    # Data configuration
    magicnumber    = 0x20130928
    includeQuality = True

    # Sensor configuration
    sensortypes = {1: "temperature",
                   2: "humidity",
                   3: "windSpeed",
                   4: "windDirection",
                   5: "rainfall",
                   6: "pressure"}

    # BMP085/180 configuration
    bmpUnitID = 0x0001       # Register as "Sensor-0001"
    bmpSampleInterval = 300  # 5 minutes
    heightAboveSeaLevel = 24 # 24 meters above sea level

    # Logging configuration
    logFileName = "/var/log/rpiweather.log"
    maxBytes    = 1048576
    backupCount = 5

    # Set up logging
    logger = logging.getLogger("RPIWeather")
    logger.setLevel(logging.DEBUG)
    handler = logging.handlers.RotatingFileHandler(logFileName, 
                                                   maxBytes=maxBytes, 
                                                   backupCount=backupCount)
    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)

    logger.info("RPIWeather Data Gateway (c) 2014 atbrask")

    # Set up queue
    dataqueue = Queue()

    # Start receiver!
    logger.info("Starting RF24 radio...")
    radio = NRF24()
    radio.begin(spiMajor, spiMinor, cePin, irqPin)
    radio.setRetries(15, 15)
    radio.setPayloadSize(32)
    radio.setChannel(channel)
    radio.setDataRate(NRF24.BR_1MBPS)
    radio.setPALevel(NRF24.PA_MAX)
    radio.setAutoAck(1)
    radio.openWritingPipe(writePipe)
    radio.openReadingPipe(1, readPipe)
    radio.startListening()

    logger.info("Starting listening thread...")
    receiver = RF24Receiver(radio, dataqueue, magicnumber, logger)
    receiverThread = Thread(target = receiver.start)
    receiverThread.daemon = True
    receiverThread.start()
    logger.info("Listening thread started!")

    # Start barometric sensor!
    try:
        logger.info("Connecting to Bosch BMP085/180 sensor...")
        sensor = BMP085.BMP085(mode = BMP085.BMP085_ULTRAHIGHRES)
        logger.info("BMP085/180 found! Starting thread...")
        bmpsampler = BMPSampler(sensor,
                                bmpUnitID,
                                dataqueue,
                                bmpSampleInterval,
                                heightAboveSeaLevel,
                                logger)
        sensorThread = Thread(target = bmpsampler.start)
        sensorThread.daemon = True
        sensorThread.start()
        logger.info("BMP085/180 thread started!")
    except:
        logger.error("Could not start BMP085/180 sensor thread! Skipping...")

    # Start sender!
    logger.info("Starting InfluxDB client...")
    db = influxdb.InfluxDBClient(host, port, username, password, database)

    logger.info("Starting sending thread...")
    sender = InfluxDBSender(db, dataqueue, sensortypes, includeQuality, logger)
    senderThread = Thread(target = sender.start)
    senderThread.daemon = True
    senderThread.start()
    logger.info("Sending thread started!")

    # Wait forever
    logger.info("RPIWeather Data Gateway start-up completed!")
    while(True):
        time.sleep(5)
