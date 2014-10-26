### Backend for "atbrask's Sensor Nodes v1.0"
Copyright (c) 2014 A.T.Brask <atbrask@gmail.com> (Except included libraries)
All rights reserved

Version history:
* v1.0 (2014-10-26) Initial release

This is the backend part of the project. It consists of a gateway server and
an InfluxDB server. The role of the gateway is to fetch data packets from the
sensor nodes and forward them to the database. In case of a database server
crash or reboot (or other availability issues) the gateway will buffer data
until it runs out of memory.

Installing and configuring InfluxDB is pretty easy, so I'll skip it in this
write-up. Let's then focus on the gateway itself. It's merely a python script
running on a Raspberry Pi with a highly specialized version of Tiny Core Linux
called piCore.

#### Prerequisites
* Raspberry Pi Model B
* SD card
* nRF24L01(+) radio module connected to the SPI bus (see below)
* A bunch of "atbrask's Sensor Nodes"
* A running [InfluxDB](http://influxdb.com) server
* A [disk image of piCore 5.3.1-SSH](http://tinycorelinux.net/5.x/armv6/releases/5.3/piCore-5.3.1-SSH.zip) (+ basic knowledge about how to use it)
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

#### Installation
* Install piCore 5.3.1-SSH onto an SD card using `dd` or similar tool.
* Plug in the SD card and boot the Pi.
* Log in to the system using SSH (username `tc` and password `piCore`).
* Change the password to something else using `passwd`.
* Run `tce-load -wi nano` to install nano.
* Open `/opt/.filetool.lst` (using nano) and remove the lines `usr/local/etc/ssh/ssh_config` and `usr/local/etc/ssh/sshd_config`. Instead insert the line `usr/local/etc/ssh` and save.
* Run `sudo filetool.sh -b` to save the changes.
* Expand the file system (and reboot). See [here](http://www.maketecheasier.com/review-of-picore/) for a how-to.
* When the system is ready, log in again.
* Run `tce-load -wi git` to install git.
* Run `git clone https://www.github.com/atbrask/RPIWeather.git`
* Type `cd RPIWeather/Backend` and run `./install.sh`.
* Open `/etc/sysconfig/tcedir/onboot.lst` and add the lines `python-spidev.tcz`, `python-nrf24.tcz`, `python-requests.tcz`, and `python-influxdb.tcz` at the bottom of the file.
* Run `sudo filetool.sh -b`to save the changes.
* Done!

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
the nRF radio chip.

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
