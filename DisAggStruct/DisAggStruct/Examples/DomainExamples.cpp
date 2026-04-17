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
 * @file DomainExamples.cpp
 * @brief End-user demonstration of the Weather and Finance domains.
 *
 * @par Role: End user
 * This file only knows about the front-end functions and the struct shapes
 * it cares about.  It has no knowledge of back-end implementations, the
 * disaggregation framework, or any other domain.
 *
 * @par Domain sealing
 * WeatherFrontEnd.h and FinanceFrontEnd.h each include only their own
 * back-end header.  Putting a finance type inside a weather struct (or
 * vice versa) is a compile error — the offending type is simply never
 * in scope for the other domain's front-end.
 */

#include <cstdio>
#include "Weather/WeatherFrontEnd.h"
#include "Finance/FinanceFrontEnd.h"

using namespace Weather;
using namespace Finance;

// ---------------------------------------------------------------------------
// Weather structs — any subset of WeatherTypes, any order
// ---------------------------------------------------------------------------

struct DailyForecast {
    Temperature temperature;
    Humidity    humidity;
    Pressure    pressure;
};

struct StormAlert {
    WindSpeed wind;
    Rainfall  rain;
};

struct FullWeatherSnapshot {
    Temperature temperature;
    Humidity    humidity;
    Pressure    pressure;
    WindSpeed   wind;
    Rainfall    rain;
};

// ---------------------------------------------------------------------------
// Finance structs — any subset of FinanceTypes, any order
// ---------------------------------------------------------------------------

struct TickerSummary {
    StockPrice price;
    Volume     volume;
};

struct FundamentalsView {
    MarketCap     cap;
    DividendYield yield;
    PriceChange   change;
};

struct FullQuote {
    StockPrice    price;
    Volume        volume;
    MarketCap     cap;
    DividendYield yield;
    PriceChange   change;
};

// ---------------------------------------------------------------------------
// Demonstration
// ---------------------------------------------------------------------------

void RunDomainExamples() {
    // --- Weather: retrieve ---
    auto forecast  = FetchWeather(DailyForecast{});
    auto storm     = FetchWeather(StormAlert{});
    auto wxSnap    = FetchWeather(FullWeatherSnapshot{});

    std::printf("=== Weather ===\n");
    std::printf("Forecast  — temp: %.1f  humidity: %.1f  pressure: %.1f\n",
        (float)forecast.temperature, (float)forecast.humidity, (float)forecast.pressure);
    std::printf("Storm     — wind: %.1f  rain: %.1f\n",
        (float)storm.wind, (float)storm.rain);
    std::printf("Snapshot  — temp: %.1f  wind: %.1f\n",
        (float)wxSnap.temperature, (float)wxSnap.wind);

    // --- Weather: persist ---
    LogWeather(forecast);

    // --- Finance: retrieve ---
    auto ticker = FetchQuote(TickerSummary{});
    auto fundamentals = FetchQuote(FundamentalsView{});
    auto quote  = FetchQuote(FullQuote{});

    std::printf("\n=== Finance ===\n");
    std::printf("Ticker       — price: %.2f  volume: %lld\n",
        (float)ticker.price, (long long)ticker.volume);
    std::printf("Fundamentals — cap: %.2e  yield: %.4f  change: %.2f\n",
        (float)fundamentals.cap, (float)fundamentals.yield, (float)fundamentals.change);
    std::printf("Full quote   — price: %.2f  cap: %.2e\n",
        (float)quote.price, (float)quote.cap);

    // --- Finance: persist ---
    RecordTrade(ticker);
}
