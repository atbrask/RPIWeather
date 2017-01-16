#!/bin/sh

# Install basic dependencies
tce-load -wi python
tce-load -wi python-dev
tce-load -wi compiletc
tce-load -wi setuptools
tce-load -wi ntp
tce-load -wi i2c-tools

# Install RPIWeather lib
sudo python setup.py install

# Install init script
mkdir /opt/RPIWeather
cp rpiweather.py /opt/RPIWeather/
chmod +x /opt/RPIWeather/rpiweather.py

# Setup NTP client
grep -q -F '/usr/local/bin/ntpd -g' /opt/bootlocal.sh || echo '/usr/local/bin/ntpd -g' >> /opt/bootlocal.sh

# Commit changes to file system
grep -q -F 'usr/local/lib/python2.7/site-packages' /opt/.filetool.lst || echo 'usr/local/lib/python2.7/site-packages' >> /opt/.filetool.lst
sudo filetool.sh -b
