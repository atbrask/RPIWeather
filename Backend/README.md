### Backend for "atbrask's Sensor Nodes v1.0"
Copyright (c) 2014 A.T.Brask <atbrask@gmail.com> (Except included libraries)
All rights reserved

Version history:
* v1.1 (2015-02-25) Added support for the Bosch BMP085/180 barometric sensor
* v1.0 (2014-10-26) Initial release

This is the backend part of the project. It consists of a gateway server and
an InfluxDB server. The role of the gateway is to fetch data packets from the
sensor nodes and forward them to the database. In case of a database server
crash or reboot (or other availability issues) the gateway will buffer data
until it runs out of memory.

It also supports a Bosch BMP085/180 baremetric sensor connected directly to
the Pi's I2C pins. These measurements are sent to the InfluxDB server just as
the ones from the sensor nodes. Just comment out the relevant lines if you
don't have this sensor installed.

Installing and configuring InfluxDB is pretty easy, so I'll skip it in this
write-up. Let's then focus on the gateway itself. It's merely a python script
running on a Raspberry Pi with a highly specialized version of Tiny Core Linux
called piCore.

#### Prerequisites
* Raspberry Pi Model B
* SD card
* nRF24L01(+) radio module connected to the SPI bus (see below)
* BMP085/180 barometric sensor connected to the I2C bus (see below)
* A bunch of "atbrask's Sensor Nodes"
* A running [InfluxDB](http://influxdb.com) server
* A [disk image of piCore 6.1-SSH](http://tinycorelinux.net/6.x/armv6/releases/images/piCore-6.1-SSH.zip) (+ basic knowledge about how to use it)
* Knowledge of the python programming language
* An internet connection :-)

#### Connecting an nRF24L01 radio module to the Pi
The Raspberry Pi has a directly accessible SPI bus on the big 26 pin header. I
have connected my radio module as follows. I have not yet experimented with
connecting the IRQ pin. The nRF module is one of the common ones from eBay.
They all seem to have the same 8-pin connector, but double-check it to be sure!

RPi pin | meaning | nRF pin
--------|---------|--------
17      | 3.3v    | 2
19      | MOSI    | 6
20      | GND     | 1
21      | MISO    | 7
22      | CE      | 3
23      | SCLK    | 5
24      | CSN     | 4
??      | IRQ     | 8

#### Connecting a Bosch BMP085/180 barometric sensor module to the Pi
Like the SPI bus above, the Pi has a I2C bus exposed in the same big header.
There are a lot of different BMP085/180 breakout boards out there, so you need
to find out if your board needs 3.3v or 5v, and whether or not you need to add
pull-up resistors to the data pins. I used the fairly well-known GY-68 board.
These can be picked up for $1-2 on Ebay. It has a 3.3v regulator and the
necessary pull-up resistors, so it can be attached directly.

RPi pin | GY-68 pin
--------|----------
3       | SDA
5       | SCL
6       | GND
2       | VCC

If you have a breakout board without the 3.3v regulator, you'll need to use
pin 1 instead of pin 2 for VCC.

#### Installation
* Install piCore 6.1-SSH onto an SD card using `dd` or similar tool.
* Plug in the SD card and boot the Pi.
* Log in to the system using SSH (username `tc` and password `piCore`).
* Change the password to something else using `passwd`.
* Run `sudo filetool.sh -b` to save the newly generated SSH keys.
* Expand the file system (and reboot). See [here](http://www.maketecheasier.com/review-of-picore/) for a how-to.
* When the system is ready, log in again.
* Run `tce-load -w -i git` to install git.
* Run `git clone https://www.github.com/atbrask/RPIWeather.git`
* Type `cd RPIWeather/Backend` and run `./install.sh`.
* Run `tce-load -w -i nano` to install nano.
* Open `/etc/sysconfig/tcedir/onboot.lst` and add the lines `python-spidev.tcz`, `python-nrf24.tcz`, `python-requests.tcz`, and `python-influxdb.tcz` at the bottom of the file.
* Open `/opt/bootlocal.sh` and add the line `ntpd`
* Run `sudo filetool.sh -b`to save the changes.
* Done! You can reboot if you wish to make sure that everything loads as it should.

#### Configuration
Now everything is installed and put into the right places. But before running 
the backend, it needs to be configured. 

* Open `/opt/rpiweather.py` and make the necessary changes. It's a regular python script with some configuration settings. You'll need to put in details about the sensor nodes, how the nRF radio is connected to the Pi, and login details for the InfluxDB server.
* Save your changes.
* Each time you change a setting, run `sudo filetool.sh -b` to persist the changes

#### Running the damn thing
To run the script from the command line write:

    sudo /opt/rpiweather.py


We need to use `sudo` in order to access the GPIO pins that are connected to
the nRF radio chip and the BMP085/180 sensor.

This is useful for testing the configuration. Remember to check the log file to
see if everything works as expected. But running the gateway from the command
line is impractical for the long run. We need to add it to piCore's list of 
services to be started at boot.

* Open `/opt/bootlocal.sh` and add the line `sudo /opt/rpiweather.py &`
* Run `sudo filetool.sh -b` to save the changes.
* Reboot to test that it auto-starts

Now the server should be running and ready for use!

#### File locations
* Code file: `/opt/rpiweather.py`
* Log file: `/var/log/rpiweather.log`

That is all! Have fun hacking!

/atbrask
