# DisAggStruct

Compile-time struct disaggregation and type-safe field dispatch for C++20.

The framework solves a recurring pain in service and hardware layers: different callers need different subsets of back-end data in different struct shapes. Without DisAggStruct you write bespoke glue code for every struct, and adding a field touches every producer and every consumer. With DisAggStruct, callers define the struct they need. The compiler routes each field to the right handler. The back end and front end never change.

---

## What it looks like

Define a type vocabulary with `MORPHEME` — zero-friction strong typedefs that make semantically different values of the same primitive type distinct to the compiler:

```cpp
// SensorTypes.h — written once by the system designer
MORPHEME(SensorId,    int);
MORPHEME(Temperature, float);
MORPHEME(ErrorCode,   int);
MORPHEME(ActiveFlag,  bool);
```

Implement one back-end handler per type:

```cpp
// SensorBackEnd.cpp — no structs, no templates, no framework knowledge
void retrieve(SensorId&    v) { v = SensorId{42}; }
void retrieve(Temperature& v) { v = Temperature{21.5f}; }
void retrieve(ErrorCode&   v) { v = ErrorCode{0}; }
void retrieve(ActiveFlag&  v) { v = ActiveFlag{true}; }
```

Wire everything together once in the front end:

```cpp
// SensorFrontEnd.h — one template, handles any struct shape, never changes
template<typename T>
T GetSensorData(T s) {
    return DisAgg::disaggregate(s, [](auto& f) { retrieve(f); });
}
```

Now any caller defines the struct they actually need:

```cpp
struct WeatherReading { Temperature temp; Humidity humidity; Pressure pressure; };
auto wx = GetSensorData(WeatherReading{});   // 3 back-end calls

struct Thermometer { Temperature temp; };
auto th = GetSensorData(Thermometer{});      // 1 back-end call

struct DeviceHealth { SensorId id; ErrorCode code; ActiveFlag active; };
auto dh = GetSensorData(DeviceHealth{});     // 3 different back-end calls
```

No registration. No code generation. No framework knowledge in the back end.  
A field type with no handler is a **compile error** — handler completeness is enforced at build time.

---

## Core API

### `DisAgg::disaggregate(s, policy)`

Decomposes aggregate `s` into a tuple of field references, calls `policy(field)` on each in declaration order, and returns the updated struct. `policy` is typically a generic lambda that names a back-end function; the compiler resolves the correct overload for each field type via ADL.

### `DisAgg::field_count_v<T>`

Compile-time field counter. Probes aggregate constructibility with cascading C++20 `requires` expressions to determine the number of fields in any aggregate at zero runtime cost.

```cpp
struct Vec3 { float x, y, z; };
static_assert(DisAgg::field_count_v<Vec3> == 3);
```

### `DisAgg::to_tuple(s)` / `DisAgg::from_tuple<T>(t)`

Decompose a struct into a `std::tuple<F0&, F1&, …>` of lvalue references and back. Useful as building blocks for custom visitors.

### `MORPHEME(Name, T)`

Declares a distinct named type wrapping `T`. `Temperature` and `Pressure` both wrap `float` but are different types to the compiler. All operators work implicitly via the underlying type — no operator overloads are needed.

```cpp
MORPHEME(Temperature, float);
MORPHEME(Pressure,    float);

Temperature t = 21.5f;  // implicit from float
Pressure    p = 1013.f; // distinct type — can't silently mix with t
float       v = t;       // implicit back to float
```

---

## Three-role model

The framework cleanly separates three responsibilities, each with minimal knowledge of the others:

| Role | Knows | Does not need to know |
|------|-------|-----------------------|
| **System designer** | The type vocabulary | What structs callers will define |
| **Back-end implementer** | One handler per type | Struct shapes, templates, the framework |
| **End user / caller** | The function name and the type vocabulary | Back-end implementations, the framework |

Adding a new type requires: one new `MORPHEME` line, one new handler. Nothing else changes.

---

## Domain sealing

Each domain front-end includes only its own back-end header, so mixing types across domains is a compile error:

```cpp
// WeatherFrontEnd.h includes only WeatherBackEnd.h
// FinanceFrontEnd.h includes only FinanceBackEnd.h

struct BadMix { Weather::Temperature temp; Finance::StockPrice price; };
FetchWeather(BadMix{});   // compile error: no retrieve(Finance::StockPrice&) in scope
```

The domain wall is the include graph. There is no runtime check, no registry, no attribute.

---

## Building

### CMake (Linux, macOS, Windows)

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/DisAggStruct        # Linux / macOS
build\DisAggStruct.exe      # Windows
```

Requires C++20. Tested with MSVC 2022; modern GCC and Clang with C++20 support should work.

### Visual Studio 2022

Open `DisAggStruct.sln`, set the target to **x64 Release**, then **Build → Build Solution**.

---

## Running

The executable runs three domain demos followed by the test suite:

```
=== Sensor Use Cases ===
WeatherReading (3 fields)
  temp     = 21.5
  humidity = 65
  pressure = 1013.25
...
=== Weather ===
Forecast  — temp: 21.5  humidity: 65.0  pressure: 1013.0
...
=== Finance ===
Ticker       — price: 182.47  volume: 4200000
...
[TestFieldCounting]
[TestRoundTrip]
[TestRetrieve]
[TestSave]

All tests passed.
```

---

## Repository layout

```
DisAggStruct/
├── Core/
│   ├── DisAggStruct.h   # framework: field counting, decomposition, dispatch
│   └── Morpheme.h       # strong typedef wrapper
├── Sensor/              # domain example: IoT sensor subsystem
├── Weather/             # domain example: meteorological data
├── Finance/             # domain example: market data
├── Examples/            # end-user code demonstrating domain sealing
├── Tests/               # framework and integration tests
└── Main.cpp
```

---

## Requirements

- C++20 or later (tested with MSVC 2022; modern GCC and Clang should work)
- No external dependencies

---

## License

MIT — see [LICENSE](LICENSE).
