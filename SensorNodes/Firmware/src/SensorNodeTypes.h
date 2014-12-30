/*
A few struct types for "atbrask's Sensor Nodes v1.0"
Copyright (c) 2014 A.T.Brask <atbrask@gmail.com>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT 
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef SENSORNODETYPES_H
#define SENSORNODETYPES_H

#include <stdint.h>

// Add new sensor types here
enum SensorTypeEnum : uint8_t
{
    NoSensor            = 0x00,
    TemperatureSensor   = 0x01,
    HumiditySensor      = 0x02,
    WindSpeedSensor     = 0x03,
    WindDirectionSensor = 0x04,
    RainGaugeSensor     = 0x05
};

// We use the same quality values as OPC (except the limit bits)
enum QualityEnum : uint8_t
{
    QualityBadNonSpecific             = 0x00,
    QualityBadConfigurationError      = 0x04,
    QualityBadNotConnected            = 0x08,
    QualityBadDeviceFailure           = 0x0C,
    QualityBadSensorFailure           = 0x10,
    QualityBadLastKnownValue          = 0x14,
    QualityBadCommFailure             = 0x18,
    QualityBadOutOfService            = 0x1C,

    QualityUncertainNonSpecific       = 0x40,
    QualityUncertainLastUsableValue   = 0x44,
    QualityUncertainSensorNotAccurate = 0x50,
    QualityUncertainEngUnitsExceeded  = 0x54,
    QualityUncertainSubNormal         = 0x58,
    
    QualityGoodNonSpecific            = 0xC0,
    QualityGoodLocalOverride          = 0xD8,
    
    QualityNoValue                    = 0xFF
};

// A single reading
struct SensorReading
{
    SensorTypeEnum Type = NoSensor;
    QualityEnum Quality = QualityBadNotConnected;
    int16_t Value       = 0;
} __attribute__((packed));

// The entire send buffer of 32 bytes
struct SendBuffer
{
    uint32_t MagicNumber     = MAGICNUMBER;
    uint16_t FirmwareVersion = FIRMWAREVERSION;
    uint16_t UnitID          = UNITID;
    uint16_t SequenceNumber;
    uint16_t BatteryVoltage;
    SensorReading Readings[4];
    uint32_t Checksum;
} __attribute__((packed));

#endif