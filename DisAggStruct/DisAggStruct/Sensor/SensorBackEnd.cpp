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
 * @file SensorBackEnd.cpp
 * @brief Back-end retrieve implementations for the sensor subsystem.
 *
 * @par Role: Back-end implementer
 * In production these would query hardware registers, a database, or a
 * remote service.  The mock values here let the system run without real
 * hardware while demonstrating the full dispatch chain.
 *
 * Each function is completely independent.  They share no state and have
 * no knowledge of each other, of the structs callers will build, or of
 * how many fields any given request will contain.
 */

#include "Sensor/SensorBackEnd.h"

namespace Sensor {
    void retrieve(SensorId&    v) { v = SensorId{    42           }; }
    void retrieve(Temperature& v) { v = Temperature{ 21.5f        }; }
    void retrieve(Pressure&    v) { v = Pressure{    1013.25f     }; }
    void retrieve(Humidity&    v) { v = Humidity{    65.0f        }; }
    void retrieve(Voltage&     v) { v = Voltage{     3.3f         }; }
    void retrieve(ErrorCode&   v) { v = ErrorCode{   0            }; }
    void retrieve(Timestamp&   v) { v = Timestamp{   1713312000LL }; }
    void retrieve(ActiveFlag&  v) { v = ActiveFlag{  true         }; }
}
