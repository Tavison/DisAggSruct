/**
 * @file SensorTypes.h
 * @brief Shared Morpheme type definitions for the sensor subsystem.
 *
 * @par Role: System designer
 * This file is written once by whoever designs the system.  It declares
 * the vocabulary of types that the back end can provide and the front end
 * can request.  Neither the back-end nor the front-end developer invents
 * types — they both draw from this shared list.
 *
 * Adding a new type here and providing a back-end handler for it
 * immediately makes that type available to any struct any caller defines.
 * No other file needs to change.
 */

#pragma once
#include "Core/Morpheme.h"

namespace Sensor {
    MORPHEME(SensorId,    int);
    MORPHEME(Temperature, float);
    MORPHEME(Pressure,    float);
    MORPHEME(Humidity,    float);
    MORPHEME(Voltage,     float);
    MORPHEME(ErrorCode,   int);
    MORPHEME(Timestamp,   long long);
    MORPHEME(ActiveFlag,  bool);
}
