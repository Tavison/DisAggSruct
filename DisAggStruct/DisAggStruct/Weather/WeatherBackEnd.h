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
 * @file WeatherBackEnd.h
 * @brief Back-end contract for the weather domain.
 *
 * @par Role: Back-end implementer
 * Write one retrieve and one save overload per weather type.
 * No knowledge of structs, templates, or callers required.
 * Passing a non-weather type to a weather front-end function
 * is a compile error — this header being absent from scope is
 * the entire enforcement mechanism.
 */

#pragma once
#include "Weather/WeatherTypes.h"

namespace Weather {
    void retrieve(Temperature& v);
    void retrieve(Humidity&    v);
    void retrieve(Pressure&    v);
    void retrieve(WindSpeed&   v);
    void retrieve(Rainfall&    v);

    void save(Temperature& v);
    void save(Humidity&    v);
    void save(Pressure&    v);
    void save(WindSpeed&   v);
    void save(Rainfall&    v);
}
