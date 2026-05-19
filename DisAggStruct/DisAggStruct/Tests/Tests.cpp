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
 * @file Tests.cpp
 * @brief Test suite for the DisAggStruct framework.
 */

#include "Tests/Tests.h"
#include "Core/DisAggStruct.h"
#include "Core/Morpheme.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

// Unconditional check — unlike assert(), never compiled away under NDEBUG.
#define CHECK(expr) do { if (!(expr)) { \
    std::cerr << "FAILED: " #expr "\n"; std::abort(); } } while(false)
#define CHECK_NEAR(a, b, eps) CHECK(std::abs((a) - (b)) < (eps))

// ═══════════════════════════════════════════════════════════════════════════════
// SHARED TYPES
// Morpheme types and structs would normally live in a shared header.
// ═══════════════════════════════════════════════════════════════════════════════

MORPHEME(SensorId,    int);
MORPHEME(Temperature, float);
MORPHEME(ErrorCode,   int);
MORPHEME(ActiveFlag,  bool);

// Finance types — SQL accumulator test.
MORPHEME(LastPrice,     double);
MORPHEME(MarketCap,     double);
MORPHEME(DividendYield, double);
MORPHEME(TradeVolume,   long long);

// Finance types — multi-source dispatch test.
MORPHEME(AnalystRating, double);
MORPHEME(NewsSentiment, double);
MORPHEME(RiskScore,     double);

struct Reading {
    SensorId    sensorId;
    Temperature temperature;
    ErrorCode   errorCode;
    ActiveFlag  active;
};

struct MinimalReading {
    SensorId    sensorId;
    Temperature temperature;
};

// ═══════════════════════════════════════════════════════════════════════════════
// BACK-END
// The back-end developer writes one free function per type.
// No templates. No structs. No framework knowledge.
// ═══════════════════════════════════════════════════════════════════════════════

namespace {
    void retrieve(SensorId&    v) { v = SensorId{    99    }; }
    void retrieve(Temperature& v) { v = Temperature{ 36.6f }; }
    void retrieve(ErrorCode&   v) { v = ErrorCode{   0     }; }
    void retrieve(ActiveFlag&  v) { v = ActiveFlag{  true  }; }

    void save(SensorId&    v) { (void)v; }
    void save(Temperature& v) { (void)v; }
    void save(ErrorCode&   v) { (void)v; }
    void save(ActiveFlag&  v) { (void)v; }
}

// ═══════════════════════════════════════════════════════════════════════════════
// WIRING
// Written once per operation.  Names the back-end function.  Template so that
// any struct whose fields are in the type vocabulary works without touching
// this layer or the back end.
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
T get_reading(T s = {}) {
    return DisAgg::disaggregate(s, [](auto& field) { retrieve(field); });
}

template<typename T>
void put_reading(T r) {
    DisAgg::disaggregate(r, [](auto& field) { save(field); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// FRONT-END
// The caller picks whatever struct shape they need and passes it in.
// The back end and wiring layer never change regardless of the struct shape.
// ═══════════════════════════════════════════════════════════════════════════════

static void TestRetrieve() {
    std::cout << "[TestRetrieve]\n";

    Reading r = get_reading(Reading{});

    std::cout << "  sensorId    = " << static_cast<int>(r.sensorId)      << "  (expected 99)\n";
    std::cout << "  temperature = " << static_cast<float>(r.temperature) << "  (expected 36.6)\n";
    std::cout << "  errorCode   = " << static_cast<int>(r.errorCode)     << "  (expected 0)\n";
    std::cout << "  active      = " << static_cast<bool>(r.active)       << "  (expected 1)\n";

    CHECK(static_cast<int>(r.sensorId)       == 99);
    CHECK_NEAR(static_cast<float>(r.temperature), 36.6f, 1e-4f);
    CHECK(static_cast<int>(r.errorCode)      == 0);
    CHECK(static_cast<bool>(r.active)        == true);

    // Same wiring function, smaller struct — back end and wiring are untouched.
    MinimalReading m = get_reading(MinimalReading{});

    std::cout << "  MinimalReading — sensorId=" << static_cast<int>(m.sensorId)
              << "  temperature=" << static_cast<float>(m.temperature) << '\n';

    CHECK(static_cast<int>(m.sensorId)       == 99);
    CHECK_NEAR(static_cast<float>(m.temperature), 36.6f, 1e-4f);
}

static void TestSave() {
    std::cout << "[TestSave]\n";

    Reading r{ SensorId{1}, Temperature{22.5f}, ErrorCode{3}, ActiveFlag{false} };
    put_reading(r);

    // Same wiring function with a different struct shape.
    MinimalReading m{ SensorId{2}, Temperature{18.0f} };
    put_reading(m);

    std::cout << "  save completed without error\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// SQL ACCUMULATOR
// The back-end developer writes this once.  Each add() overload names its
// column and captures the assignment lambda.  After the traversal, execute()
// builds one SELECT with exactly the columns the caller's struct asked for,
// then fires each assignment to populate the fields.
//
// disaggregate_inplace is used (not disaggregate) so that the lambdas capture
// references into the caller's struct, not into a temporary copy.
// ═══════════════════════════════════════════════════════════════════════════════

struct SqlFetcher {
    struct Column {
        const char*           name;
        std::function<void()> assign;
    };
    std::vector<Column> cols;

    void add(LastPrice&     v) { cols.push_back({"last_price",     [&v]{ v = 182.47;    }}); }
    void add(MarketCap&     v) { cols.push_back({"market_cap",     [&v]{ v = 2.80e12;   }}); }
    void add(DividendYield& v) { cols.push_back({"dividend_yield", [&v]{ v = 0.0055;    }}); }
    void add(TradeVolume&   v) { cols.push_back({"trade_volume",   [&v]{ v = 4200000LL; }}); }

    void execute(const char* ticker) {
        // Build the SELECT from whatever columns were accumulated.
        std::string q = "SELECT ";
        for (std::size_t i = 0; i < cols.size(); ++i) {
            if (i) q += ", ";
            q += cols[i].name;
        }
        q += " FROM quotes WHERE ticker = '";
        q += ticker;
        q += "'";
        std::printf("  SQL: %s\n", q.c_str());

        // In production: run the query, read each column from the result set.
        // Here: assign hard-coded mock values that stand in for DB results.
        for (auto& c : cols)
            c.assign();
    }
};

// Front-end — one template, written once, handles any struct whose fields
// are in the type vocabulary above.
template<typename T>
T FetchQuote(const char* ticker, T s = {}) {
    SqlFetcher f;
    DisAgg::disaggregate_inplace(s, [&f](auto& field) { f.add(field); });
    f.execute(ticker);
    return s;
}

// ═══════════════════════════════════════════════════════════════════════════════
// MULTI-SOURCE DISPATCH
// The same disaggregate_inplace call routes each field type to the right
// backend.  The struct shape determines which backends are consulted — a struct
// with no SQL fields issues no SQL query; a struct with no NoSQL fields never
// touches the document store.
// ═══════════════════════════════════════════════════════════════════════════════

// NoSQL fetcher — one key-value GET per requested field, batched per struct.
struct NoSqlFetcher {
    struct Entry { std::string key; std::function<void()> assign; };
    std::vector<Entry> entries;

    void add(AnalystRating& v) { entries.push_back({"analyst:rating", [&v]{ v = 4.2;  }}); }
    void add(NewsSentiment& v) { entries.push_back({"news:sentiment", [&v]{ v = 0.65; }}); }

    void execute(const char* ticker) {
        for (auto& e : entries) {
            std::printf("  NoSQL:  GET %s:%s\n", e.key.c_str(), ticker);
            e.assign();
        }
    }
};

// Router — each add() overload sends its field to the right backend.
// SQL fields are batched into one SELECT.
// NoSQL fields are batched into one GET per key.
// Direct fields are assigned immediately with no I/O.
struct MultiSourceFetcher {
    SqlFetcher   sql;
    NoSqlFetcher nosql;
    std::vector<const char*> direct;

    void add(LastPrice&     v) { sql.add(v);   }   // exchange database
    void add(TradeVolume&   v) { sql.add(v);   }   // exchange database
    void add(AnalystRating& v) { nosql.add(v); }   // analyst document store
    void add(NewsSentiment& v) { nosql.add(v); }   // news pipeline
    void add(RiskScore&     v) { v = 0.73; direct.push_back("risk_score"); }  // in-memory

    void execute(const char* ticker) {
        if (!sql.cols.empty())      sql.execute(ticker);
        if (!nosql.entries.empty()) nosql.execute(ticker);
        for (auto* name : direct)
            std::printf("  Direct: %s (in-memory)\n", name);
    }
};

template<typename T>
T FetchInstrument(const char* ticker, T s = {}) {
    MultiSourceFetcher f;
    DisAgg::disaggregate_inplace(s, [&f](auto& field) { f.add(field); });
    f.execute(ticker);
    return s;
}

// ═══════════════════════════════════════════════════════════════════════════════
// FRAMEWORK TESTS
// Low-level verification that the machinery underneath works correctly.
// ═══════════════════════════════════════════════════════════════════════════════

struct Point { int x; int y; int z; };

static void TestFieldCounting() {
    std::cout << "[TestFieldCounting]\n";

    static_assert(DisAgg::field_count_v<Point>   == 3);
    static_assert(DisAgg::field_count_v<Reading> == 4);

    std::cout << "  Point   : " << DisAgg::field_count_v<Point>   << "  (expected 3)\n";
    std::cout << "  Reading : " << DisAgg::field_count_v<Reading> << "  (expected 4)\n";
}

static void TestRoundTrip() {
    std::cout << "[TestRoundTrip]\n";

    Point p{ 1, 2, 3 };
    auto  t  = DisAgg::to_tuple(p);
    Point p2 = DisAgg::from_tuple<Point>(t);

    std::cout << "  " << p2.x << ", " << p2.y << ", " << p2.z << "  (expected 1, 2, 3)\n";

    CHECK(p2.x == 1 && p2.y == 2 && p2.z == 3);
}

static void TestSqlAccumulator() {
    std::printf("[TestSqlAccumulator]\n");

    // Three different views of the same quote data.
    // Each one issues one SQL query containing exactly its own columns —
    // nothing more.  SqlFetcher and FetchQuote are never touched.
    struct PriceAndVolume { LastPrice price; TradeVolume volume;                                       };
    struct FullQuote      { LastPrice price; MarketCap cap; DividendYield yield; TradeVolume volume;   };
    struct IncomeView     { LastPrice price; DividendYield yield;                                      };

    auto pv = FetchQuote("AAPL", PriceAndVolume{});
    auto fq = FetchQuote("AAPL", FullQuote{});
    auto iv = FetchQuote("AAPL", IncomeView{});

    std::printf("  PriceAndVolume — price=%.2f  volume=%lld\n",
                (double)pv.price, (long long)pv.volume);
    std::printf("  FullQuote      — price=%.2f  cap=%.2e  yield=%.4f  volume=%lld\n",
                (double)fq.price, (double)fq.cap, (double)fq.yield, (long long)fq.volume);
    std::printf("  IncomeView     — price=%.2f  yield=%.4f\n",
                (double)iv.price, (double)iv.yield);

    CHECK_NEAR((double)pv.price,  182.47,  0.01);
    CHECK     ((long long)pv.volume == 4200000LL);
    CHECK_NEAR((double)fq.cap,    2.80e12, 1e9);
    CHECK_NEAR((double)fq.yield,  0.0055,  1e-5);
    CHECK_NEAR((double)iv.price,  182.47,  0.01);
    CHECK_NEAR((double)iv.yield,  0.0055,  1e-5);
}

static void TestMultiSourceFetch() {
    std::printf("[TestMultiSourceFetch]\n");

    // Four views of the same instrument.  Each struct shape determines exactly
    // which backends are consulted — nothing more is ever called.
    struct TradingView  { LastPrice price; TradeVolume volume; RiskScore risk;                          };
    struct ResearchView { LastPrice price; AnalystRating rating; NewsSentiment sentiment; RiskScore risk; };
    struct RiskOnly     { RiskScore risk;                                                               };
    struct FullView     { LastPrice price; TradeVolume volume;
                          AnalystRating rating; NewsSentiment sentiment; RiskScore risk;                };

    std::printf("  -- TradingView: SQL + direct --\n");
    auto tv = FetchInstrument("MSFT", TradingView{});

    std::printf("  -- ResearchView: SQL + NoSQL + direct --\n");
    auto rv = FetchInstrument("MSFT", ResearchView{});

    std::printf("  -- RiskOnly: direct only (no SQL, no NoSQL issued) --\n");
    auto ro = FetchInstrument("MSFT", RiskOnly{});

    std::printf("  -- FullView: all three sources --\n");
    auto fv = FetchInstrument("MSFT", FullView{});

    CHECK_NEAR((double)tv.price,      182.47, 0.01);
    CHECK     ((long long)tv.volume   == 4200000LL);
    CHECK_NEAR((double)tv.risk,       0.73,   0.001);
    CHECK_NEAR((double)rv.rating,     4.2,    0.01);
    CHECK_NEAR((double)rv.sentiment,  0.65,   0.001);
    CHECK_NEAR((double)ro.risk,       0.73,   0.001);
    CHECK_NEAR((double)fv.price,      182.47, 0.01);
    CHECK_NEAR((double)fv.sentiment,  0.65,   0.001);
}

static void TestNestedStruct() {
    std::cout << "[TestNestedStruct]\n";

    // A plain aggregate nested inside another — disaggregate recurses into it.
    // No handler for InnerReading itself; the leaf handlers for SensorId and
    // Temperature are called directly.
    struct InnerReading {
        SensorId    sensorId;
        Temperature temperature;
    };
    struct NestedReading {
        InnerReading inner;
        ErrorCode    errorCode;
        ActiveFlag   active;
    };

    NestedReading r = get_reading(NestedReading{});

    std::cout << "  inner.sensorId    = " << static_cast<int>(r.inner.sensorId)      << "  (expected 99)\n";
    std::cout << "  inner.temperature = " << static_cast<float>(r.inner.temperature) << "  (expected 36.6)\n";
    std::cout << "  errorCode         = " << static_cast<int>(r.errorCode)           << "  (expected 0)\n";
    std::cout << "  active            = " << static_cast<bool>(r.active)             << "  (expected 1)\n";

    CHECK(static_cast<int>(r.inner.sensorId)               == 99);
    CHECK_NEAR(static_cast<float>(r.inner.temperature), 36.6f, 1e-4f);
    CHECK(static_cast<int>(r.errorCode)                    == 0);
    CHECK(static_cast<bool>(r.active)                      == true);
}

// ── Entry point ───────────────────────────────────────────────────────────────

int RunAllTests() {
    TestSqlAccumulator();
    TestMultiSourceFetch();
    TestFieldCounting();
    TestRoundTrip();
    TestRetrieve();
    TestSave();
    TestNestedStruct();

    std::cout << "\nAll tests passed.\n";
    return 0;
}
