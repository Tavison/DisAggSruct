# DisAggStruct

Compile-time struct disaggregation and type-safe field dispatch for C++20.

---

## The idea

A back end is written once. A front end is written once. After that, **callers define whatever struct they need** — any subset of the known types, in any field order — and it just works. No registration. No code generation. No changes anywhere.

---

## Seeing it in action

Eight back-end handlers. One front-end template. Written once, never touched again:

```cpp
// Back-end — one function per type, no struct or framework knowledge
void retrieve(SensorId&    v) { v = SensorId{42};           }
void retrieve(Temperature& v) { v = Temperature{21.5f};     }
void retrieve(Pressure&    v) { v = Pressure{1013.25f};     }
void retrieve(Humidity&    v) { v = Humidity{65.0f};        }
void retrieve(Voltage&     v) { v = Voltage{3.3f};          }
void retrieve(ErrorCode&   v) { v = ErrorCode{0};           }
void retrieve(Timestamp&   v) { v = Timestamp{1713312000LL};}
void retrieve(ActiveFlag&  v) { v = ActiveFlag{true};       }

// Front-end — one template, handles any struct shape
template<typename T>
T GetSensorData(T s) {
    return DisAgg::disaggregate(s, [](auto& f) { retrieve(f); });
}
```

Now callers define the struct they need — any subset, any field order:

```cpp
struct Thermometer    { Temperature temp; };
struct WeatherReading { Temperature temp; Humidity humidity; Pressure pressure; };
struct PowerInfo      { Voltage voltage; Timestamp timestamp; };
struct DeviceHealth   { SensorId id; ErrorCode code; ActiveFlag active; };
struct FullSnapshot   { SensorId id; Temperature temp; Humidity humidity;
                        Pressure pressure; Voltage voltage; ErrorCode code;
                        Timestamp timestamp; ActiveFlag active; };

// Field order doesn't matter — these resolve identically
struct AltOrder       { Pressure pressure; Temperature temp; SensorId id; };

auto a = GetSensorData(Thermometer{});     //  1 back-end call
auto b = GetSensorData(WeatherReading{});  //  3 back-end calls
auto c = GetSensorData(PowerInfo{});       //  2 back-end calls
auto d = GetSensorData(DeviceHealth{});    //  3 different back-end calls
auto e = GetSensorData(FullSnapshot{});    //  8 back-end calls
auto f = GetSensorData(AltOrder{});        //  3 back-end calls, different order
```

Six struct shapes. Six function calls. **Zero changes to the back end or front end.**

A field type with no handler is a **compile error** — handler completeness is enforced at build time, not discovered at runtime.

---

## How to set it up

### 1 — Define the type vocabulary

`MORPHEME` declares a distinct named type over a primitive. `Temperature` and `Pressure` both wrap `float` but are different types to the compiler, so the dispatcher can tell them apart:

```cpp
// SensorTypes.h — written once by the system designer
MORPHEME(SensorId,    int);
MORPHEME(Temperature, float);
MORPHEME(Pressure,    float);   // distinct from Temperature despite same underlying type
MORPHEME(Humidity,    float);
MORPHEME(Voltage,     float);
MORPHEME(ErrorCode,   int);
MORPHEME(Timestamp,   long long);
MORPHEME(ActiveFlag,  bool);
```

Morpheme types convert implicitly to and from their underlying type, so no casts are needed in surrounding code.

### 2 — Implement the back end

One free function per type. No structs, no templates, no framework knowledge:

```cpp
void retrieve(Temperature& v) { v = Temperature{21.5f}; }
void retrieve(Pressure&    v) { v = Pressure{1013.25f}; }
// ...one per type
```

### 3 — Wire the front end

One template per operation. Names the back-end function; the compiler resolves the right overload for each field:

```cpp
template<typename T>
T GetSensorData(T s) {
    return DisAgg::disaggregate(s, [](auto& f) { retrieve(f); });
}
```

That's it. Callers now define any struct they like and call `GetSensorData`.

---

## Core API

### `DisAgg::disaggregate(s, policy)`

Decomposes aggregate `s` into a tuple of field references, calls `policy(field)` on each in declaration order, and returns the updated struct. `policy` is typically a generic lambda that names a back-end function; the compiler resolves the correct overload for each field type via ADL.

### `DisAgg::field_count_v<T>`

Compile-time field counter. Probes aggregate constructibility with cascading C++20 `requires` expressions at zero runtime cost.

```cpp
struct Vec3 { float x, y, z; };
static_assert(DisAgg::field_count_v<Vec3> == 3);
```

### `DisAgg::to_tuple(s)` / `DisAgg::from_tuple<T>(t)`

Decompose a struct into a `std::tuple<F0&, F1&, …>` of lvalue references and back. Building blocks for custom visitors.

### `MORPHEME(Name, T)` and `MORPHEMES(T, ...)`

The most common objection to strong types is "I have to change all my code." With Morpheme you don't — it converts implicitly in both directions, so existing code keeps compiling unchanged:

```cpp
// Before — two floats, indistinguishable to the compiler
struct Reading {
    float temperature;
    float pressure;
};

// After — one line per type, existing code untouched
MORPHEME(Temperature, float);
MORPHEME(Pressure,    float);

struct Reading {
    Temperature temperature;   // field name unchanged
    Pressure    pressure;      // field name unchanged
};

// Every line of existing client code still compiles:
float t = reading.temperature;         // implicit to float
reading.temperature = 21.5f;           // implicit from float
if (reading.pressure > 1000.0f) { }   // implicit in expressions
printf("%.1f", (float)reading.temp);  // explicit cast when needed
```

The only thing that changed is the compiler can now tell `Temperature` and `Pressure` apart — enabling the type-safe dispatch.

When a domain has several quantities sharing the same primitive type (common in finance and sensor work), `MORPHEMES` collapses the vocabulary to one line per primitive:

```cpp
// Five distinct float types — one line
MORPHEMES(float, Temperature, Humidity, Pressure, WindSpeed, Rainfall);

// Mix underlying types with separate calls
MORPHEMES(float,     StockPrice, MarketCap, DividendYield, PriceChange);
MORPHEMES(long long, Volume, TradeId, Timestamp);
MORPHEMES(bool,      ActiveFlag, IsValid);
```

**Macro-free C++20 alternative.** Each `decltype([]{})` at a different source location is a unique type — no macro required:

```cpp
using Temperature = DisAgg::Morpheme<decltype([]{}), float>;
using Pressure    = DisAgg::Morpheme<decltype([]{}), float>;  // distinct type
```

---

## Three-role model

| Role | Knows | Does not need to know |
|------|-------|-----------------------|
| **System designer** | The type vocabulary | What structs callers will define |
| **Back-end implementer** | One handler per type | Struct shapes, templates, the framework |
| **Caller** | The function name and the type vocabulary | Back-end implementations, the framework |

Adding a new type: one new `MORPHEME` line, one new handler. Nothing else changes.

---

## Domain sealing

Each domain front-end includes only its own back-end header. Putting a finance type inside a weather struct is a compile error — the type is simply not in scope:

```cpp
struct BadMix { Weather::Temperature temp; Finance::StockPrice price; };
FetchWeather(BadMix{});   // compile error: no retrieve(Finance::StockPrice&) in scope
```

The domain wall is the include graph. No runtime check, no registry, no attribute.

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

```
=== Sensor Use Cases ===
WeatherReading (3 fields)   temp=21.50  humidity=65.00  pressure=1013.25
Thermometer    (1 field)    temp=21.50
DeviceHealth   (3 fields)   id=42  errorCode=0  active=1
...
[TestFieldCounting]  [TestRoundTrip]  [TestRetrieve]  [TestSave]

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
