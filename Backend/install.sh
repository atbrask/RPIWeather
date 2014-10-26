#!/bin/sh

# Get dependencies
git clone https://www.github.com/atbrask/python-requests-tcz.git
git clone https://www.github.com/atbrask/python-influxdb-tcz.git
git clone https://www.github.com/atbrask/python-spidev-tcz.git
git clone https://www.github.com/atbrask/python-nrf24-tcz.git

# Copy extension files
cp python-requests-tcz/python-requests.tcz* /etc/sysconfig/tcedir/optional/
cp python-influxdb-tcz/python-influxdb.tcz* /etc/sysconfig/tcedir/optional/
cp python-spidev-tcz/python-spidev.tcz* /etc/sysconfig/tcedir/optional/
cp python-nrf24-tcz/python-nrf24.tcz* /etc/sysconfig/tcedir/optional/

# Clean up a bit
rm -rf python-requests-tcz
rm -rf python-influxdb-tcz
rm -rf python-spidev-tcz
rm -rf python-nrf24-tcz

# Install depencies
tce-load -i python-influxdb
tce-load -i python-nrf24

# Install script
cp rpiweather.py /opt/
chmod +x /opt/rpiweather.py
sudo filetool.sh -b
