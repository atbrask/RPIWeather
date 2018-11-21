from setuptools import setup, find_packages

setup(name = 'RPIWeather',
      version = '1.5.0',
      author = 'A.T.Brask',
      author_email = 'atbrask@gmail.com',
      description = 'Weather station based on nRF24L01, DHT22, ATtiny84, and a Raspberry Pi',
      license = 'GPL-2.0+',
      url = 'https://www.github.com/atbrask/RPIWeather/',
      install_requires = ['influxdb', 'pyserial', 'Adafruit-BMP', 'RPi.GPIO', 'spidev'],
      packages = find_packages())
