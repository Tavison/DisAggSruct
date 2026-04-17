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
 * @file WeatherFrontEnd.h
 * @brief Front-end API for the weather domain.
 *
 * @par Role: Front-end implementer
 * One template function per operation family.  Written once, never changes.
 * The struct passed in defines what is requested — any combination of
 * weather types, in any order, in any quantity.
 *
 * Because this file includes only WeatherBackEnd.h, passing a struct
 * containing a non-weather type (e.g. StockPrice) is a compile error.
 * The domain wall is enforced entirely by the include graph.
 */

#pragma once
#include "Weather/WeatherBackEnd.h"
#include "Core/DisAggStruct.h"

/// @brief Populate any weather struct from the back end.
template<typename T>
T FetchWeather(T s) {
    return DisAgg::disaggregate(s, [](auto& f) { retrieve(f); });
}

/// @brief Persist any weather struct to the back end.
template<typename T>
T LogWeather(T s) {
    return DisAgg::disaggregate(s, [](auto& f) { save(f); });
}
