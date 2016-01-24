# Makefile for the firmware to "433MHz Receiver v1.0"
# Copyright (c) 2016 A.T.Brask <atbrask@gmail.com>
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

############################
# AVR PROGRAMMING SETTINGS #
############################

PART=attiny84
F_CPU=8000000
PROGRAMMER=avrisp2 -P usb
AVRDUDE=avrdude -c $(PROGRAMMER) -p $(PART)
OBJ2HEX=avr-objcopy

#####################
# COMPILER SETTINGS #
#####################

CC=avr-gcc
CFLAGS=-Os -fdata-sections -ffunction-sections -Wl,--gc-sections -Wall \
       -mcall-prologues -mrelax -mmcu=$(PART) -std=c++0x -DF_CPU=$(F_CPU)
SRC=src
BIN=bin
SOURCES=$(wildcard $(SRC)/*.cpp) $(wildcard $(SRC)/*/*.cpp) $(wildcard $(SRC)/*.S) $(wildcard $(SRC)/*/*.S)

#################
# BUILD TARGETS #
#################

default: rebuild program

build: $(BIN)/firmware.hex

rebuild: clean build

program:
	$(AVRDUDE) -e -U flash:w:$(BIN)/firmware.hex -B 5

rdfuses:
	$(AVRDUDE) -B 100 -v -q

# Sets fuse bits for internal 8 MHz RC-osc
# see: http://www.engbedded.com/fusecalc/
fuses:
	$(AVRDUDE) -p t84 -c avrisp2 -P usb -U lfuse:w:0xE2:m -U hfuse:w:0xDF:m \
	-U efuse:w:0xFF:m -U lock:w:0xFF:m

$(BIN)/firmware.obj: $(SOURCES) Makefile
	$(CC) $(CFLAGS) $(SOURCES) -o $@
	avr-size --mcu=$(PART) -C $@

%.hex: %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

clean:
	rm -f $(BIN)/*.obj
	rm -f $(BIN)/*.hex
