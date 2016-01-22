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

import sys
import threading
import time

import Adafruit_BMP.BMP085 as BMP

class BMPSampler:
    def __init__(self, interval, altitude, dataqueue, logger):
        self.interval = interval
        self.altitude = altitude
        self.dataqueue = dataqueue
        self.logger = logger
        self.logger.info("Connecting to Bosch BMP085/180 sensor on I2C bus...")
        self.sensor = BMP.BMP085(mode = BMP.BMP085_ULTRAHIGHRES)
        self.logger.info("BMP085/180 connected!")

    def start(self):
        self.logger.info("Starting BMP085/180 thread...")
        thread = threading.Thread(target = self.loop)
        thread.daemon = True
        thread.start()
        self.logger.info("BMP085/180 thread started!")

    def loop(self):
        while True:
            try:
                pressure = self.sensor.read_sealevel_pressure(altitude_m=self.altitude)
                data = self.processPacket(pressure)
                self.dataqueue.put(data)
                time.sleep(self.interval)
            except:
                self.logger.error("BMP085/180 error: '{0}'".format(sys.exc_info()[1].message))
                time.sleep(10)


    def processPacket(self, pressure):
        return {
            "id" : "BMP",
            "time" : int(time.time()),
            "pressure" : pressure / 100.0
        }
