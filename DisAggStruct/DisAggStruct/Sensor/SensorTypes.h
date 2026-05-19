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
    MORPHEMES(float,     Temperature, Pressure, Humidity, Voltage);
    MORPHEMES(int,       SensorId, ErrorCode);
    MORPHEMES(long long, Timestamp);
    MORPHEMES(bool,      ActiveFlag);
}
