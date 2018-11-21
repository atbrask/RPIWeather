### Backend for "atbrask's Sensor Nodes v1.0"
Copyright (c) 2014-2018 A.T.Brask <atbrask@gmail.com> (Except included libs)
All rights reserved

Version history:
* v1.5 (2018-11-21) Upgraded to piCore 9.0.3 and enabled HTTPS support.
* v1.4 (2017-01-16) Upgraded to piCore 8.0 and InfluxDB >=1.0
* v1.3 (2016-01-22) Added support for 433MHz sensors. Lots of refactoring.
* v1.2 (2015-11-11) Fixed compatibility issues
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

Finally, I have also added support for a few commonly available wireless
433MHz weather station thermometers. This requires a little radio receiver
circuit to be added to the Pi and connected to its on-board UART pins. It
is based around a SYN470 receiver IC and an ATtiny84 MCU. Just comment out
the relevant lines if you don't have this receiver installed.

Installing and configuring InfluxDB is pretty easy, so I'll skip it in this
write-up. Let's then focus on the gateway itself. It's merely a python script
running on a Raspberry Pi with a highly specialized version of Tiny Core Linux
called piCore.

#### Prerequisites
* Raspberry Pi Model B
* SD card
* nRF24L01(+) radio module connected to the SPI bus (see below)
* Optional: BMP085/180 barometric sensor connected to the I2C bus (see below)
* Optional: A 433MHz receiver and a bunch of compatible sensors (see below)
* A bunch of "atbrask's Sensor Nodes"
* A running [InfluxDB](http://influxdb.com) server
* A [disk image of piCore 9.0.3](http://tinycorelinux.net/9.x/armv6/releases/RPi/piCore-9.0.3.zip) (+ basic knowledge about how to use it)
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

#### Connecting a 433MHz receiver to the Pi
I have created a small 433MHz receiver for commonly available weather station
temperature sensors. It is based on a SYN470 receiver IC and an ATtiny84 that 
collects the radio data and forwards it to the Pi. Look elsewhere in this
repository for a schematic and some firmware. The receiver uses the RX pin of
the Pi's on-board UART, so you'll need to disable the serial console that
PiCore exposes on these pins. When this is done, the receiver streams data
to /dev/ttyAMA0 whenever it receives some.

#### Installation
* Install piCore 9.0.3 onto an SD card using `dd` or similar tool.
* Plug in the SD card and boot the Pi.
* Log in to the system using SSH (username `tc` and password `piCore`).
* Change the password to something else using `passwd`.
* Run `filetool.sh -b` to save the newly generated SSH keys.
* Expand the file system (and reboot). See [here](http://www.maketecheasier.com/review-of-picore/) for a how-to.
* When the system is ready, log in again.
* Refresh tcz packages by running `tce-update` followed by a reboot.
* Run `tce-load -wi git` to install git.
* Run `git clone https://www.github.com/atbrask/RPIWeather.git`
* Type `cd RPIWeather/Backend` and run `./install.sh`.
* You can now safely remove the installer code by running `cd ~` followed by `rm -rf RPIWeather`.
* Run `tce-load -wi nano` to install the nano text editor.
* Configure the main RPIWeather script in `/opt/RPIWeather/rpiweather.py` (see details in next section).
* Done! You can reboot if you wish to make sure that everything loads as it should.

If you want to use the 433MHz receiver feature, you'll need a couple of extra steps to free up the on-board UART pins:

* Run `sudo mount /dev/mmcblk0p1`
* Open `/mnt/mmcblk0p1/cmdline.txt` and remove all references `/dev/ttyAMA0`. In my case, I removed `console=ttyAMA0,115200`
* Run `sudo umount /dev/mmcblk0p1`
* Open `/opt/bootlocal.sh`
* Remove or comment out the line `/usr/sbin/startserialtty &`
* Run `sudo filetool.sh -b`to save the changes.
* Reboot

#### Configuration
Now everything is installed and put into the right places. But before running 
the backend, it needs to be configured. 

* Open `/opt/RPIWeather/rpiweather.py` and make the necessary changes. It's a regular python script with some configuration settings. You'll need to put in details about the sensor nodes, how the nRF radio is connected to the Pi, and login details for the InfluxDB server.
* Save your changes.
* Each time you change a setting, run `sudo filetool.sh -b` to persist the changes

#### Running the damn thing
To run the script from the command line write:

    sudo /opt/RPIWeather/rpiweather.py


We need to use `sudo` in order to access the GPIO pins that are connected to
the nRF radio chip and the BMP085/180 sensor.

This is useful for testing the configuration. Remember to check the log file to
see if everything works as expected. But running the gateway from the command
line is impractical for the long run. We need to add it to piCore's list of 
services to be started at boot.

* Open `/opt/bootlocal.sh` and add the line `sudo /opt/RPIWeather/rpiweather.py &`
* Run `sudo filetool.sh -b` to save the changes.
* Reboot to test that it auto-starts

Now the server should be running and ready for use!

#### File locations
* Code entry point: `/opt/RPIWeather/rpiweather.py`
* Log file: `/var/log/rpiweather.log`

That is all! Have fun hacking!

/atbrask
