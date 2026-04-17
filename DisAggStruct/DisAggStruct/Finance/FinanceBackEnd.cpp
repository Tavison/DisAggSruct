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
 * @file FinanceBackEnd.cpp
 * @brief Finance back-end implementations.
 *
 * @par Role: Back-end implementer
 * In production these would query a market data feed and persist
 * trades to a transaction ledger.
 */

#include "Finance/FinanceBackEnd.h"

namespace Finance {
    void retrieve(StockPrice&    v) { v = StockPrice{    182.47f   }; }
    void retrieve(Volume&        v) { v = Volume{        4200000LL }; }
    void retrieve(MarketCap&     v) { v = MarketCap{     2.8e12f   }; }
    void retrieve(DividendYield& v) { v = DividendYield{ 0.0055f   }; }
    void retrieve(PriceChange&   v) { v = PriceChange{   3.12f     }; }

    void save(StockPrice&    v) { (void)v; /* persist to ledger */ }
    void save(Volume&        v) { (void)v; }
    void save(MarketCap&     v) { (void)v; }
    void save(DividendYield& v) { (void)v; }
    void save(PriceChange&   v) { (void)v; }
}
