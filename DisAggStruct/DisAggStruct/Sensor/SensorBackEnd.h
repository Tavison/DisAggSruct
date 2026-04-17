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
