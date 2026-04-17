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
