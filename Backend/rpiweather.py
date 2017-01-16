#!/usr/bin/python

# RPIWeather Data Gateway
# Copyright (c) 2014-2017 A.T.Brask <atbrask@gmail.com>
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

import time
import logging
import logging.handlers
import Queue

from RPIWeather import InfluxDBSender
from RPIWeather import OOK433MHzReceiver
from RPIWeather import BMPSampler
from RPIWeather import RF24Receiver

if __name__ == "__main__":

    #################
    # CONFIGURATION #
    #################

    # RF24 receiver settings
    spiMajor    = 0
    spiMinor    = 0
    cePin       = 25 #GPIO25 = pin 22
    irqPin      = 24 #GPIO24 = pin 18
    readPipe    = [0x01, 0x28, 0x09, 0x13, 0x20]
    writePipe   = [0x02, 0x28, 0x09, 0x13, 0x20]
    channel     = 0x5c
    magicnumber = 0x20130928

    # Database settings
    host     = "192.168.1.251"
    port     = 8086
    username = "gateway"
    password = "thomas"
    database = "RPIWeather"
    table    = "sensordata"

    # 433 MHz receiver settings
    serialPort = '/dev/ttyAMA0'

    # BMP085/180 pressure sensor settings
    bmpSampleInterval   = 300 # seconds
    heightAboveSeaLevel = 24  # meters

    # Logging configuration
    logFileName = "/var/log/rpiweather.log"
    maxBytes    = 1048576
    backupCount = 5

    # Mapping from IDs to names
    namemap = {"BMP":       "Sensor-0001",

               "RF24-0101": "Sensor-0101",
               "RF24-0102": "Sensor-0102",
               "RF24-0103": "Sensor-0103",
               "RF24-0104": "Sensor-0104",
               "RF24-0105": "Sensor-0105",
               "RF24-0106": "Sensor-0106",
               "RF24-0107": "Sensor-0107",
               "RF24-0108": "Sensor-0108",
               "RF24-0109": "Sensor-0109",
               "RF24-010A": "Sensor-010A",
               "RF24-010B": "Sensor-010B",

               "RF24-FFFF": "Sensor-FFFF", # reserved for debugging

               "OS1-08-2":  "Sensor-0201",
               "TX3-10":    "Sensor-0202",
               "CHN-10":    "Sensor-0203",
               "OS2-22-2":  "Sensor-0204"}

    #########
    # START #
    #########

    # Start logging
    logger = logging.getLogger("RPIWeather")
    logger.setLevel(logging.DEBUG)
    handler = logging.handlers.RotatingFileHandler(logFileName, maxBytes=maxBytes, backupCount=backupCount)
    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)

    logger.info("RPIWeather Data Gateway (c) 2014-2017 atbrask")

    # Start data queue
    dataqueue = Queue.Queue()

    # Start data receivers
    RF24Receiver.RF24Receiver(spiMajor, spiMinor, cePin, irqPin, channel, writePipe, readPipe, magicnumber, dataqueue, logger).start()
    OOK433MHzReceiver.OOK433MHzReceiver(serialPort, dataqueue, logger).start()
    BMPSampler.BMPSampler(bmpSampleInterval, heightAboveSeaLevel, dataqueue, logger).start()

    # Start data sender
    InfluxDBSender.InfluxDBSender(host, port, username, password, database, table, namemap, dataqueue, logger).start()

    # Wait forever
    logger.info("RPIWeather Data Gateway start-up completed!")
    while(True):
        time.sleep(5)
