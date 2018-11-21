# RPIWeather Data Gateway
# Copyright (c) 2014-2018 A.T.Brask <atbrask@gmail.com>
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

from influxdb import InfluxDBClient

class InfluxDBSender:
    def __init__(self, host, port, ssl, verify_ssl, username, password, database, table, namemap, dataqueue, logger):
        self.db = InfluxDBClient(host, port, username, password, database, ssl, verify_ssl)
        self.table = table
        self.namemap = namemap # id -> name
        self.dataqueue = dataqueue
        self.logger = logger
        self.history = dict() # id -> timestamp
        self.unmapped = dict() # id -> timestamp

    def start(self):
        self.logger.info("Starting InfluxDB client thread...")
        thread = threading.Thread(target = self.loop)
        thread.daemon = True
        thread.start()
        self.logger.info("Sending thread started!")

    def loop(self):
        data = None
        while True:
            try:
                if data is None:
                    packet = self.dataqueue.get()
                    data = self.processPacket(packet)
                
                if data is not None:
                    self.db.write_points(data)
                    # If the write fails, the following line is skipped.
                    data = None
            except:
                self.logger.error("Send error '{0}' (Buffered packets: {1})".format(sys.exc_info()[1].message, self.dataqueue.qsize() + 1))
                time.sleep(10)

    def processPacket(self, packet):
        # Some sensors might repeat a message a couple of times. At most once per 5 seconds is a reasonable limit.
        if packet['id'] in self.history and packet['time'] - self.history[packet['id']] < 5:
            return None
        self.history[packet['id']] = packet['time']

        # Manage list of unknown sensors
        currenttime = int(time.time())
        for sensor in [k for k, v in self.unmapped.iteritems() if currenttime - v > 3600]:
            self.logger.info("Send info: Unknown sensor with id '{0}' has not been observed for an hour. Removing from list of unknown sensors...".format(sensor))
            del self.unmapped[sensor]
        if packet['id'] not in self.namemap:
            if packet['id'] not in self.unmapped:
                self.logger.info("Send info: Unknown sensor with id '{0}' discovered! Adding to list of unknown sensors...".format(packet['id']))
                self.unmapped[packet['id']] = currenttime
            return None

        # Format InfluxDB data packet
        fields = {}
        for key in packet:
            if key is not 'id' and key is not 'time':
                fields[key] = packet[key]
        timestamp = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime(packet['time']))
        return [{"measurement": self.table, "tags": {"source": self.namemap[packet['id']]}, "time": timestamp, "fields": fields}]
