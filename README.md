## Weather station based on nRF24L01, DHT22, ATtiny84, and a Raspberry Pi

### Introduction
This is a complete system for collecting, storing, and displaying environmental
sensor data. The system consists of a number of embedded sensor nodes that
communicate wirelessly with a base station using the very popular nRF24L01
radio modules.

In its current form, the project is work-in-progress. Status on the different
parts as of 2014-10-26:

* Firmware: 100%
* Hardware: 100%
* Backend: 100% (minus some testing...)
* Frontend: 0%

I have successfully tested concurrent operation of 10 sensor nodes with a DHT22
sensor each. I have prepared a couple more for collecting wind and rain data
as well.

### Features
* Firmware optimized for ATtiny84 @ 1 MHz (8MHz crystal with the CKDV8 fuse set)
* Temperature and humidity measurement using a DHT22 sensor
* Wind speed and wind direction measurement using a La Crosse TX23 anemometer
* Rainfall measurement using a WS-2300-16 rain gauge
* Wireless operation using the very popular nRF24L01+ 2.4 GHz radio chip
* Very compact CRC32 implementation
* Battery powered operation using a very efficient 3.3v DC/DC converter
* Power management by sleeping the CPU and turning off unneeded hardware
* Battery voltage measurement included in data packet
* Typical battery draw while sleeping has been measured to around 17uA
* Total firmware size from 3.0 kiB to 3.5 kiB (depending on sensor type)

Eventually I'll get around to writing a bit about this on my blog at:
http://www.atbrask.dk/

/atbrask
