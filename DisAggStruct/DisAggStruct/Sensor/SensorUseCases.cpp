// Copyright (c) 2026 James Edwards
// SPDX-License-Identifier: MIT
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/**
 * @file SensorUseCases.cpp
 * @brief Use case demonstrations for the sensor subsystem.
 *
 * @par Role: End user / caller
 * This file represents the end user's world.  They need to know:
 *  - The type vocabulary from @c SensorTypes.h
 *  - The function name @c GetSensorData
 *
 * They do NOT need to know:
 *  - That DisAgg exists
 *  - How the back end works
 *  - How the front end is wired
 *  - What other structs anyone else has defined
 *
 * The key point demonstrated here: the end user defines whatever struct
 * expresses exactly what they need.  Two fields, eight fields, any order.
 * @c GetSensorData accepts all of them without modification.  The back end
 * never changes.  The front end never changes.  Only the struct changes,
 * and the struct lives entirely in the caller's code.
 */

#include "Sensor/SensorUseCases.h"
#include "Sensor/SensorFrontEnd.h"
#include <cstdio>

using namespace Sensor;

// ── Each caller defines exactly what they need ────────────────────────────────
// None of these structs are registered anywhere.  They work simply because
// each field type has a back-end retrieve function.

/// Caller A only cares about atmospheric conditions.
struct WeatherReading {
    Temperature temp;
    Humidity    humidity;
    Pressure    pressure;
};

/// Caller B only needs a quick temperature check.
struct Thermometer {
    Temperature temp;
};

/// Caller C wants device health — completely different fields.
struct DeviceHealth {
    SensorId   id;
    ErrorCode  errorCode;
    ActiveFlag active;
};

/// Caller D wants power and timing data.
struct PowerInfo {
    Voltage   voltage;
    Timestamp timestamp;
};

/// Caller E wants everything the back end can provide.
struct FullSnapshot {
    SensorId    id;
    Temperature temp;
    Humidity    humidity;
    Pressure    pressure;
    Voltage     voltage;
    ErrorCode   errorCode;
    Timestamp   timestamp;
    ActiveFlag  active;
};

// ── Demonstrations ────────────────────────────────────────────────────────────

void RunUseCases() {
    std::printf("=== Sensor Use Cases ===\n\n");

    // Each call uses the same GetSensorData function.
    // The struct passed in determines what gets retrieved.
    // The back end and front end are untouched for every one of these.

    auto weather = GetSensorData(WeatherReading{});
    std::printf("WeatherReading (3 fields)\n");
    std::printf("  temp     = %.2f\n", (float)weather.temp);
    std::printf("  humidity = %.2f\n", (float)weather.humidity);
    std::printf("  pressure = %.2f\n\n", (float)weather.pressure);

    auto therm = GetSensorData(Thermometer{});
    std::printf("Thermometer (1 field)\n");
    std::printf("  temp = %.2f\n\n", (float)therm.temp);

    auto health = GetSensorData(DeviceHealth{});
    std::printf("DeviceHealth (3 different fields)\n");
    std::printf("  id        = %d\n",  (int)health.id);
    std::printf("  errorCode = %d\n",  (int)health.errorCode);
    std::printf("  active    = %d\n\n",(int)(bool)health.active);

    auto power = GetSensorData(PowerInfo{});
    std::printf("PowerInfo (2 fields)\n");
    std::printf("  voltage   = %.2f\n",  (float)power.voltage);
    std::printf("  timestamp = %lld\n\n",(long long)power.timestamp);

    auto full = GetSensorData(FullSnapshot{});
    std::printf("FullSnapshot (all 8 fields)\n");
    std::printf("  id        = %d\n",   (int)full.id);
    std::printf("  temp      = %.2f\n", (float)full.temp);
    std::printf("  humidity  = %.2f\n", (float)full.humidity);
    std::printf("  pressure  = %.2f\n", (float)full.pressure);
    std::printf("  voltage   = %.2f\n", (float)full.voltage);
    std::printf("  errorCode = %d\n",   (int)full.errorCode);
    std::printf("  timestamp = %lld\n", (long long)full.timestamp);
    std::printf("  active    = %d\n\n", (int)(bool)full.active);
}
