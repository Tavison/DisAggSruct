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
 * @file FinanceBackEnd.h
 * @brief Back-end contract for the finance domain.
 *
 * @par Role: Back-end implementer
 * Write one retrieve and one save overload per finance type.
 * The back-end developer knows nothing about weather, sensors,
 * or any other domain — and nothing about the structs callers build.
 */

#pragma once
#include "Finance/FinanceTypes.h"

namespace Finance {
    void retrieve(StockPrice&    v);
    void retrieve(Volume&        v);
    void retrieve(MarketCap&     v);
    void retrieve(DividendYield& v);
    void retrieve(PriceChange&   v);

    void save(StockPrice&    v);
    void save(Volume&        v);
    void save(MarketCap&     v);
    void save(DividendYield& v);
    void save(PriceChange&   v);
}
