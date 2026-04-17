/**
 * @file SensorFrontEnd.h
 * @brief Front-end API for the sensor retrieval family.
 *
 * @par Role: Front-end implementer
 * This file is written once by whoever wires the system together.  It
 * contains exactly one template function per operation family.
 *
 * The front-end developer needs to know:
 *  - Which back-end function family to call (@c retrieve in this case)
 *  - That DisAgg will route each field of @c T to the correct overload
 *
 * The front-end developer does NOT need to know:
 *  - What structs callers will pass in
 *  - How many fields those structs will have
 *  - What the back-end implementations do
 *
 * @c GetSensorData is a single template function that accepts any struct
 * composed of types from @c SensorTypes.h.  It never changes regardless
 * of how many different struct shapes callers invent.  The struct passed
 * in defines exactly what is requested — nothing more, nothing less.
 */

#pragma once
#include "Sensor/SensorBackEnd.h"
#include "Core/DisAggStruct.h"

/**
 * @brief Retrieves sensor data for any struct whose fields are known types.
 *
 * @c T may be any aggregate struct whose fields are drawn from the type
 * vocabulary in @c SensorTypes.h.  The back end is called once per field,
 * in declaration order.  Fields not present in @c T are never requested.
 *
 * @tparam T  Any aggregate struct of sensor types.  Deduced from @p s.
 * @param  s  A default-constructed (or partially filled) struct expressing
 *            which fields the caller wants populated.
 * @return    The same struct type with all fields populated by the back end.
 */
template<typename T>
T GetSensorData(T s) {
    return DisAgg::disaggregate(s, [](auto& field) { retrieve(field); });
}
