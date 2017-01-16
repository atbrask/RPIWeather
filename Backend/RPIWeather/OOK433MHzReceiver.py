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
import serial
import sys
import threading
import time

class OOK433MHzReceiver:
    def __init__(self, port, dataqueue, logger):
        self.dataqueue = dataqueue
        self.logger = logger
        self.connection = serial.Serial(port = port,
                                        baudrate = 115200,
                                        timeout = None)

    def start(self):
        self.logger.info("Starting 433MHz listening thread...")
        thread = threading.Thread(target = self.loop)
        thread.daemon = True
        thread.start()
        self.logger.info("433MHz listening thread started!")

    def loop(self):
        self.connection.flushInput()
        received = ""
        while True:
            try:
                received = self.connection.readline().strip()
                if len(received) > 0:
                    data = self.processPacket(received)
                    self.dataqueue.put(data)
                time.sleep(0.01)
            except:
                self.logger.error("433MHz receiver error: '{0}' (Last data = {1})".format(sys.exc_info()[1].message, received))

    def processPacket(self, datastring):
                # Verify token count
                data = datastring.split()
                if len(data) < 3 or len(data[1]) < 2:
                    raise Exception("Insufficient message tokens.")

                # Verify CRC32
                CRC = binascii.crc32(binascii.unhexlify(data[1])) & 0xffffffff
                providedCRC = int(data[2], 16)
                if CRC != providedCRC:
                    raise Exception("Bad CRC32! Check serial connection to the receiver.")

                # Parse payload
                nibbles = [int(nibble, 16) for nibble in data[1]]
                if data[0] == "TX3":
                    return self.decodeTX3(nibbles)
                elif data[0] == "CHN":
                    return self.decodeCHN(nibbles)
                elif data[0] == "OS1":
                    return self.decodeOS1(nibbles)
                elif data[0] == "OS2":
                    return self.decodeOS2(nibbles)
                else:
                    raise Exception("Unknown decoder: {0}".format(data[0]))

    def decodeTX3(self, data):
        result = dict()
        result["time"] = int(time.time())

        # Sanity checks    
        if len(data) != 12:
            raise Exception("TX3 error: Bad signal length! Got {0} nibbles but expected 12".format(len(data)))
    
        if data[0] != 0x0 or data[1] != 0xa:
            raise Exception("TX3 error: Bad header! Got 0x{0:02X} but expected 0x0A".format((data[0]<<4) | data[1]))

        if data[5] != data[8] or data[6] != data[9]:
            raise Exception("TX3 error: Rendundancy check failed.")

        # Compare checksum
        checksum = sum(data[:10]) & 0xf
        providedChecksum = data[10]
        if checksum != providedChecksum:
            raise Exception("TX3 error: Bad checksum! Got 0x{0:X} but calculated 0x{1:X}".format(providedChecksum, checksum))

        # Determine sensor type
        typeNibble = data[2]
        isTempSensor = typeNibble == 0x0
        isHumSensor = typeNibble == 0xe
        if not (isTempSensor or isHumSensor):
            raise Exception("TX3 error: Unknown sensor type: {0}".format(typeNibble))

        # Sensor ID
        result["id"] = "TX3-{0:02X}".format((data[3] << 3) | (data[4] & 0xe))

        # Measurement
        tens = data[5]
        ones = data[6]
        tenths = data[7]
        if tenths > 9 or ones > 9 or tens > 9:
            raise Exception("TX3 error: Corrupted measurement data!")

        measurement = (tens * 10 + ones + tenths / 10.0) - 50
        if isTempSensor:
            result["temperature"] = measurement
        else:
            result["humidity"] = measurement

        return result

    def decodeCHN(self, data):
        result = dict()
        result["time"] = int(time.time())

        # Sanity checks    
        if len(data) != 10:
            raise Exception("CHN error: Bad signal length! Got {0} nibbles but expected 10".format(len(data)))
    
        if data[0] != 0x5:
            raise Exception("CHN error: Bad header! Got 0x{0:X} but expected 0x5".format(data[0]))

        # Sensor ID    
        result["id"] = "CHN-{0:02X}".format((data[1] << 4) | data[2])
    
        # Forced transmission or just turned on?
        result["forced"] = data[3] & 0x4 != 0

        # Measurement    
        temp = (data[4]<<8) | (data[5]<<4) | data[6]
        if temp & 0x800 != 0:
            temp -= 0x1000
        result["temperature"] = temp / 10.0

        # Compare checksum
        checksum = 35
        if temp > 249:
            checksum -= (temp-33)/20
        elif temp > 99:
            checksum -= (temp+53)/20
        elif temp > 8:
            checksum -= (temp+11)/20

        providedChecksum = (data[7] << 4) | data[8]

        if checksum != providedChecksum:
            raise Exception("CHN error: Bad checksum! Got 0x{0:02X} but calculated 0x{1:02X}".format(providedChecksum, checksum))

        return result

    def decodeOS1(self, data):
        result = dict()
        result["time"] = int(time.time())

        # Length check
        if len(data) != 8:
            raise Exception("OS1 error: Bad signal length! Got {0} nibbles but expected 8".format(len(data)))
    
        # Compare checksum
        checksum = ((data[1] + data[3] + data[5])<<4) + data[0] + data[2] + data[4]
        checksum = checksum & 0xff
        providedChecksum = (data[7]<<4) | data[6]
        if checksum != providedChecksum:
            raise Exception("OS1 error: Bad checksum! Got 0x{0:02X} but calculated 0x{1:02X}".format(providedChecksum, checksum))
    
        # ID and channel
        sensorId = data[0]
        channel = ((data[1]>>2)&0x3) + 1
        result["id"] = "OS1-{0:02X}-{1:X}".format(sensorId, channel)

        # Low battery?
        result["lowbattery"] = data[5] & 0x8 != 0
    
        # Measurement
        tenths = data[2]
        ones = data[3]
        tens = data[4]
        if tenths > 9 or ones > 9 or tens > 9:
            raise Exception("OS1 error: Corrupted temperature data!")
    
        temp = tens * 10 + ones + tenths / 10.0
        if data[5] & 0x2 != 0:
            temp = -temp
        result["temperature"] = temp
    
        return result

    def calcCRC8(self, data, poly, init):
        crc = init
        for d in data:
            crc ^= d
            for i in range(8):
                if crc & 0x80:
                    crc = (crc << 1) ^ poly
                else:
                    crc <<= 1
        return crc & 0xff

    def decodeOS2(self, data):
        result = dict()
        result["time"] = int(time.time())

        # Sanity checks
        if len(data) < 18:
            raise Exception("OS2 error: Bad signal length! Got {0} nibbles but expected at least 18".format(len(data)))

        preamble = data[0]
        if preamble != 0xA:
            raise Exception("OS2 error: Bad preamble! Got 0x{0:X} but expected 0xA".format(preamble))

        data = data[1:17]
        header = (data[0] << 12) | (data[1] << 8) | (data[2] << 4) | data[3]
        headerOK = header == 0xEC41
        if not headerOK:
            raise Exception("OS2 error: Bad header! Only the AWR129 temperature sensor is currently supported! Got 0x{0:04X} but expected 0xEC41".format(header))

        # Compare checksum
        checksum = sum(data[0:12])
        providedChecksum = (data[13]<<4) | data[12]
        if checksum != providedChecksum:
            raise Exception("OS2 error: Bad checksum! Got 0x{0:02X} but calculated 0x{1:02X}".format(providedChecksum, checksum))

        # Compare CRC8
        crcnibbles = data[:5]+data[7:-4]
        crcbytes = [(crcnibbles[i]<<4) | crcnibbles[i+1] for i in range(0, len(crcnibbles), 2)]
        CRC = self.calcCRC8(crcbytes, 0x07, 0x9a)
        providedCRC = (data[15]<<4) | data[14]
        if checksum != providedChecksum:
            raise Exception("OS2 error: Bad CRC8! Got 0x{0:02X} but calculated 0x{1:02X}".format(providedCRC, CRC))

        # ID and channel
        sensorId = (data[5] << 4) | data[6]
        channel = data[4]
        result["id"] = "OS2-{0:02X}-{1:X}".format(sensorId, channel)
        
        # Low battery?
        result["lowbattery"] = data[7] & 0x4 != 0

        # Has the sensor just been reset?
        result["justreset"] = data[7] & 0x8 != 0

        # Is the probe itself connected to the transmitter?
        result["probeok"] = data[11] & 0xc != 0xc

        # Measurement
        tenths = data[8]
        ones = data[9]
        tens = data[10]
        hundreds = data[11] & 0x3 # TODO TEST
        isBCD = tenths < 10 and ones < 10 and tens < 10
        if tenths > 9 or ones > 9 or tens > 9 or hundreds > 9:
            raise Exception("OS2 error: Corrupted temperature data!")
        temp = hundreds * 100 + tens * 10 + ones + tenths / 10.0

        # check for negative temperature
        if data[11] & 0x8 != 0:
            temp = -temp

        result["temperature"] = temp

        return result
