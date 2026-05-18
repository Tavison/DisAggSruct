# DisAggStruct

A C++20 framework for compile-time struct disaggregation.

The problem it solves: a back-end that can supply many field types, but different
callers only want some of them. Without DisAggStruct you write NxM wiring — one
dispatch path for every combination of caller and field. With DisAggStruct you
write N+M pieces and they compose automatically.

## The three roles

**Back-end** — one free function per field type, no framework knowledge required:

```cpp
namespace Sensor {
    void retrieve(Temperature& v) { v = Temperature{ sensor.readTemp() }; }
    void retrieve(Humidity& v)    { v = Humidity{ sensor.readHumidity() }; }
    void retrieve(Voltage& v)     { v = Voltage{ sensor.readVoltage() }; }
    // one per type, written once, never changes
}
```

**Front-end** — one template function per operation, written once:

```cpp
template<typename T>
T GetSensorData(T s) {
    return DisAgg::disaggregate(s, [](auto& field) { retrieve(field); });
}
```

**Caller** — defines exactly the struct they need and passes it in:

```cpp
struct Thermometer  { Temperature temp; };
struct DeviceHealth { SensorId id; ErrorCode errorCode; ActiveFlag active; };
struct FullSnapshot { SensorId id; Temperature temp; Humidity humidity;
                      Pressure pressure; Voltage voltage; };

auto therm  = GetSensorData(Thermometer{});   // back end called once
auto health = GetSensorData(DeviceHealth{});  // back end called three times
auto full   = GetSensorData(FullSnapshot{});  // back end called five times
```

The back end and front end never change for new caller structs. The compiler
enforces that every field type has a matching back-end overload — a missing
overload is a build error, not a runtime surprise.

## How it works

`DisAgg::disaggregate` does three things at compile time:

1. **Field count** — probes aggregate constructibility with a cascading
   `requires` expression; no macros, no reflection required
2. **Struct to tuple** — decomposes the struct into lvalue references via
   C++17 structured bindings
3. **Dispatch** — calls the policy on each field; the compiler resolves
   the correct overload per field type

Zero runtime overhead. The `if constexpr` ladder is compiled away entirely.

```cpp
// Field count is a compile-time constant
static_assert(DisAgg::field_count_v<FullSnapshot> == 5);

// Low-level API is also available directly
Point p{ 1, 2, 3 };
auto t = DisAgg::to_tuple(p);      // tuple<int&, int&, int&>
std::get<0>(t) = 99;               // modifies p.x in place
Point p2 = DisAgg::from_tuple<Point>(t);
```

## Requirements

- C++20 (MSVC 19.29+, GCC 10+, Clang 12+)
- Header-only — copy `Core/DisAggStruct.h` and include it
- Aggregates with up to 16 fields supported

## Project layout

```
DisAggStruct/
  Core/       DisAggStruct.h, Morpheme.h
  Sensor/     Example domain: back-end, front-end, use cases
  Finance/    Example domain: back-end, front-end
  Weather/    Example domain: back-end, front-end
  Examples/   Side-by-side domain examples
  Tests/      Correctness tests
docs/         Doxygen HTML (generated — see Doxyfile to rebuild)
```

## License

MIT