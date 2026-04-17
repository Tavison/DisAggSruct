/**
 * @file SensorBackEnd.h
 * @brief Back-end retrieve contract for the sensor subsystem.
 *
 * @par Role: Back-end implementer
 * This file defines the back-end developer's entire obligation to the system:
 * one @c retrieve function per type they are responsible for providing.
 *
 * The back-end developer needs to know:
 *  - The type vocabulary from @c SensorTypes.h
 *  - How to obtain a value for each type (hardware, database, service, etc.)
 *
 * The back-end developer does NOT need to know:
 *  - Anything about structs, templates, or tuples
 *  - What data any caller will actually request
 *  - That DisAgg exists at all
 *
 * Adding a new type to the system means adding one function here.
 * Nothing else changes.
 */

#pragma once
#include "Sensor/SensorTypes.h"

namespace Sensor {
    void retrieve(SensorId&    v);
    void retrieve(Temperature& v);
    void retrieve(Pressure&    v);
    void retrieve(Humidity&    v);
    void retrieve(Voltage&     v);
    void retrieve(ErrorCode&   v);
    void retrieve(Timestamp&   v);
    void retrieve(ActiveFlag&  v);
}
