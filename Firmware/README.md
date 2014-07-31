### Firmware to "atbrask's Sensor Nodes v1.0"
Copyright (c) 2014 A.T.Brask <atbrask@gmail.com> (Except the RF24 library)
All rights reserved

Version history:
* v1.0 (2014-04-26) First finished version supporting the DHT22 thermo/hygrometer
* v1.1 (2014-06-08) Added support for the La Crosse TX23 anemometer
* v1.2 (2014-07-25) Added support for the WS-2300-16 rain gauge
* v1.3 (2014-07-30) Fixed a lot of small issues

Please note that the code uses a few C++11 features. It might not work with
older versions of avr-gcc.

Set correct fuses:

    make fuses


Read back fuses:

    make rdfuses


Rebuild code for unit 01:02 with VCC = 3.32v and a DHT22 sensor on board:

    make rebuild UNITID=0x0102 VCC=3320 SENSORTYPE=DHT22


The default build rule rebuilds the firmware and attempts to flash the device:

    make UNITID=0x0102 VCC=3320 SENSORTYPE=DHT22


Possible values for SENSORTYPE:
* DHT22 - Temperature/humidity sensor
* TX23 - Anemometer (wind speed and wind direction)
* WS_2300_16 - Rain gauge


You can get the most recent version of this code and all the schematics at:
https://www.github.com/atbrask/RPIWeather/

Eventually I'll get around to writing a bit about this on my blog at:
http://www.atbrask.dk/
