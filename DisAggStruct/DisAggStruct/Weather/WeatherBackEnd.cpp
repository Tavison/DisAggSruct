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
 * @file WeatherBackEnd.cpp
 * @brief Weather back-end implementations.
 *
 * @par Role: Back-end implementer
 * In production these would read from weather station hardware or a
 * meteorological service and persist to a time-series database.
 */

#include "Weather/WeatherBackEnd.h"

namespace Weather {
    void retrieve(Temperature& v) { v = Temperature{ 21.5f  }; }
    void retrieve(Humidity&    v) { v = Humidity{    65.0f  }; }
    void retrieve(Pressure&    v) { v = Pressure{    1013.f }; }
    void retrieve(WindSpeed&   v) { v = WindSpeed{   14.2f  }; }
    void retrieve(Rainfall&    v) { v = Rainfall{    2.4f   }; }

    void save(Temperature& v) { (void)v; /* persist to time-series db */ }
    void save(Humidity&    v) { (void)v; }
    void save(Pressure&    v) { (void)v; }
    void save(WindSpeed&   v) { (void)v; }
    void save(Rainfall&    v) { (void)v; }
}
