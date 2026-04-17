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
#include <iostream>
#include <cassert>

// ═══════════════════════════════════════════════════════════════════════════════
// SHARED TYPES
// Morpheme types and structs would normally live in a shared header.
// ═══════════════════════════════════════════════════════════════════════════════

MORPHEME(SensorId,    int);
MORPHEME(Temperature, float);
MORPHEME(ErrorCode,   int);
MORPHEME(ActiveFlag,  bool);

struct Reading {
    SensorId    sensorId;
    Temperature temperature;
    ErrorCode   errorCode;
    ActiveFlag  active;
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
// Written once per operation.  Names the back-end function and the struct type.
// Neither the front-end nor the back-end developer touches this layer.
// ═══════════════════════════════════════════════════════════════════════════════

Reading get_reading() {
    return DisAgg::disaggregate(Reading{}, [](auto& field) { retrieve(field); });
}

void put_reading(Reading r) {
    DisAgg::disaggregate(r, [](auto& field) { save(field); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// FRONT-END
// The front-end developer sees only get_reading() and put_reading().
// No templates, no Morpheme, no DisAgg.
// ═══════════════════════════════════════════════════════════════════════════════

static void TestRetrieve() {
    std::cout << "[TestRetrieve]\n";

    Reading r = get_reading();

    std::cout << "  sensorId    = " << static_cast<int>(r.sensorId)    << "  (expected 99)\n";
    std::cout << "  temperature = " << static_cast<float>(r.temperature) << "  (expected 36.6)\n";
    std::cout << "  errorCode   = " << static_cast<int>(r.errorCode)   << "  (expected 0)\n";
    std::cout << "  active      = " << static_cast<bool>(r.active)     << "  (expected 1)\n";

    assert(static_cast<int>(r.sensorId)      == 99);
    assert(static_cast<float>(r.temperature) == 36.6f);
    assert(static_cast<int>(r.errorCode)     == 0);
    assert(static_cast<bool>(r.active)       == true);
}

static void TestSave() {
    std::cout << "[TestSave]\n";

    Reading r{ SensorId{1}, Temperature{22.5f}, ErrorCode{3}, ActiveFlag{false} };
    put_reading(r);

    std::cout << "  save completed without error\n";
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

    assert(p2.x == 1 && p2.y == 2 && p2.z == 3);
}

// ── Entry point ───────────────────────────────────────────────────────────────

int RunAllTests() {
    TestFieldCounting();
    TestRoundTrip();
    TestRetrieve();
    TestSave();

    std::cout << "\nAll tests passed.\n";
    return 0;
}
