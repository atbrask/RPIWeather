#!/bin/sh

# Get dependencies for the sensor nodes
git clone https://www.github.com/atbrask/python-requests-tcz.git
git clone https://www.github.com/atbrask/python-influxdb-tcz.git
git clone https://www.github.com/atbrask/python-spidev-tcz.git
git clone https://www.github.com/atbrask/python-nrf24-tcz.git

# Copy extension files
cp python-requests-tcz/python-requests.tcz* /etc/sysconfig/tcedir/optional/
cp python-influxdb-tcz/python-influxdb.tcz* /etc/sysconfig/tcedir/optional/
cp python-spidev-tcz/python-spidev.tcz* /etc/sysconfig/tcedir/optional/
cp python-nrf24-tcz/python-nrf24.tcz* /etc/sysconfig/tcedir/optional/

# Install dependencies for the sensor nodes
tce-load -w -i python
tce-load -w -i python-RPi.GPIO
tce-load -w -i ntp
tce-load -i python-influxdb
tce-load -i python-nrf24

# Install dependencies for the BMP085/180 sensor
tce-load -w -i i2c-tools
tce-load -w -i py-smbus

# Get dependencies for the BMP085/180 sensor
git clone https://www.github.com/adafruit/Adafruit_Python_BMP.git
git clone https://www.github.com/adafruit/Adafruit_Python_GPIO.git

# Create destination folder for script files
mkdir /opt/RPIWeather

# Copy BMP085/180 files
cp -r Adafruit_Python_BMP/Adafruit_BMP /opt/RPIWeather/
cp -r Adafruit_Python_GPIO/Adafruit_GPIO /opt/RPIWeather/

# Install dependency for 433 MHz receiver
tce-load -w -i pyserial.tcz

# Install script
cp *.py /opt/RPIWeather/
chmod +x /opt/RPIWeather/rpiweather.py

# Clean up a bit
rm -rf python-requests-tcz
rm -rf python-influxdb-tcz
rm -rf python-spidev-tcz
rm -rf python-nrf24-tcz
rm -rf Adafruit_Python_BMP
rm -rf Adafruit_Python_GPIO

# Commit changes to file system
sudo filetool.sh -b
