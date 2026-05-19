# DisAggStruct

Zero-overhead compile-time struct disaggregation and type-safe field dispatch for C++20.
**All dispatch resolves at compile time** — no virtual calls, no registries, no runtime reflection.

---

## The back story

A market data service has a dozen field types — price, bid, ask, volume, market cap, dividend yield, earnings date — each sourced differently. Price comes from a real-time tick feed. Market cap is derived. Dividend yield sits in a corporate actions database with its own protocol.

Different consumers need different subsets. The risk model needs `{ price, volume, market_cap }`. The display widget needs `{ price, bid, ask }`. The compliance snapshot needs everything. New consumer views appear every sprint.

The goal: each consumer issues **one SQL query** with exactly the columns its struct asks for. Each field's handler is written once, tuned for its data source, and reused automatically across every consumer view. Adding a new view requires **zero changes** to any existing code.

Here is what that looks like with this library:

```cpp
// Back-end — written once by the data-access developer.
// Each add() overload names its column and captures the assignment.
struct SqlFetcher {
    struct Column { const char* name; std::function<void()> assign; };
    std::vector<Column> cols;

    void add(LastPrice&     v) { cols.push_back({"last_price",     [&v]{ v = 182.47;    }}); }
    void add(MarketCap&     v) { cols.push_back({"market_cap",     [&v]{ v = 2.80e12;   }}); }
    void add(DividendYield& v) { cols.push_back({"dividend_yield", [&v]{ v = 0.0055;    }}); }
    void add(TradeVolume&   v) { cols.push_back({"trade_volume",   [&v]{ v = 4200000LL; }}); }

    void execute(const char* ticker) { /* SELECT <cols> FROM quotes WHERE ticker = ? */ }
};

// Front end — one template, written once, never touched again.
template<typename T>
T FetchQuote(const char* ticker, T s = {}) {
    SqlFetcher f;
    DisAgg::disaggregate_inplace(s, [&f](auto& field) { f.add(field); });
    f.execute(ticker);
    return s;
}
```

Callers define the struct they need. The query writes itself:

```cpp
struct PriceAndVolume { LastPrice price; TradeVolume volume;                                     };
struct FullQuote      { LastPrice price; MarketCap cap; DividendYield yield; TradeVolume volume; };
struct IncomeView     { LastPrice price; DividendYield yield;                                    };

auto pv = FetchQuote("AAPL", PriceAndVolume{});
// → SELECT last_price, trade_volume FROM quotes WHERE ticker = 'AAPL'

auto fq = FetchQuote("AAPL", FullQuote{});
// → SELECT last_price, market_cap, dividend_yield, trade_volume FROM quotes WHERE ticker = 'AAPL'

auto iv = FetchQuote("AAPL", IncomeView{});
// → SELECT last_price, dividend_yield FROM quotes WHERE ticker = 'AAPL'
```

Three views. Three queries with different column sets. **Zero changes to `SqlFetcher` or `FetchQuote`.** The full working mock is in [`Tests/Tests.cpp`](DisAggStruct/DisAggStruct/Tests/Tests.cpp).

---

## Multiple data sources

The same mechanism extends to multiple backends. A router struct holds one fetcher per source and routes each field type to the right one via overloaded `add()` methods. The struct the caller passes determines which backends are consulted — fields that aren't in the struct are never touched:

```cpp
struct NoSqlFetcher {
    // One GET per requested field, batched per struct
    void add(AnalystRating& v) { entries.push_back({"analyst:rating", [&v]{ v = 4.2;  }}); }
    void add(NewsSentiment& v) { entries.push_back({"news:sentiment", [&v]{ v = 0.65; }}); }
    void execute(const char* ticker) { /* GET analyst:rating:MSFT, etc. */ }
};

struct MultiSourceFetcher {
    SqlFetcher   sql;
    NoSqlFetcher nosql;

    void add(LastPrice&     v) { sql.add(v);   }   // exchange database  → batched SELECT
    void add(TradeVolume&   v) { sql.add(v);   }   // exchange database  → batched SELECT
    void add(AnalystRating& v) { nosql.add(v); }   // analyst doc store  → batched GET
    void add(NewsSentiment& v) { nosql.add(v); }   // news pipeline      → batched GET
    void add(RiskScore&     v) { v = compute(); }  // in-memory engine   → no I/O at all

    void execute(const char* ticker) {
        if (!sql.cols.empty())      sql.execute(ticker);
        if (!nosql.entries.empty()) nosql.execute(ticker);
    }
};

template<typename T>
T FetchInstrument(const char* ticker, T s = {}) {
    MultiSourceFetcher f;
    DisAgg::disaggregate_inplace(s, [&f](auto& field) { f.add(field); });
    f.execute(ticker);
    return s;
}
```

The struct shape controls everything:

```cpp
struct TradingView  { LastPrice price; TradeVolume volume; RiskScore risk;                      };
struct ResearchView { LastPrice price; AnalystRating rating; NewsSentiment sent; RiskScore risk; };
struct RiskOnly     { RiskScore risk;                                                            };
```

```
TradingView:   SQL: SELECT last_price, trade_volume FROM quotes WHERE ticker = 'MSFT'
               Direct: risk_score (in-memory)

ResearchView:  SQL: SELECT last_price FROM quotes WHERE ticker = 'MSFT'
               NoSQL: GET analyst:rating:MSFT
               NoSQL: GET news:sentiment:MSFT
               Direct: risk_score (in-memory)

RiskOnly:      Direct: risk_score (in-memory)
               ← no SQL query, no NoSQL query
```

`RiskOnly` never opens a connection. `TradingView` never touches the document store. Each backend is called only when the caller's struct says it is needed — nothing more.

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

## The problem it solves

Picture a market data service at a large financial firm. It provides dozens of fields — last trade price, bid and ask, volume, market cap, dividend yield, earnings date, analyst consensus — each field sourced from a different place. Price comes from a real-time tick feed with microsecond-level latency requirements. Volume is an exchange summary updated every second. Market cap is derived: price times shares outstanding, pulled from a separate corporate database. Dividend yield comes from a corporate actions service with its own wire protocol.

Now picture the consumers. A risk model needs `{ price, volume, market_cap }`. A display widget needs `{ price, bid, ask, last_trade_time }`. An order management system needs `{ price, yield, earnings_date }`. A compliance snapshot needs everything. New consumers appear every release. Field coverage requirements change.

### The traditional paths, and what they cost

**One function per struct type.** Someone writes `FetchRiskSnapshot`, `FetchDisplayData`, `FetchOMSData`, and maintains them as structs evolve. When price's data source changes, every fetch function must be audited. Every new consumer view needs a new function. The wiring is manual, duplicated, and drifts.

**Runtime dispatch.** A type registry maps IDs to fetch functions at startup. Fields are fetched through an interface: a vtable lookup, a `std::visit`, a `std::function` call — an indirect branch the CPU can't predict until it is made. A missing registration is a silent runtime failure, discovered in production when a field comes back zero. In a system processing hundreds of thousands of messages per second, every unpredicted branch costs.

**Code generation.** Schemas drive a generator that emits struct definitions and dispatch tables. Adding a field is a schema edit, a regeneration, and a rebuild of every downstream header. The team now owns a build pipeline, a schema format, and the generator itself.

### What happens instead

The overload set *is* the dispatch table. The compiler builds it, checks it, and optimises it — at build time, not runtime.

Back-end developers write one free function per field type. Each function knows its data source intimately. The price handler talks directly to the tick feed and can be tuned for nanosecond reads. The market-cap handler runs a derived computation against two other values. The yield handler checks a cache before hitting the database. Each is as fast and as specialized as the problem demands. None of them know what struct they will be called from. None of them know how many other fields exist. None of them know this framework exists.

The front-end developer writes one template function per operation. It calls `DisAgg::disaggregate` and names the back-end function. The compiler does the rest: for each field in whatever struct a caller passes, it resolves the right overload and emits a **direct call** — not an indirect one, not a virtual one. The optimizer sees every call site and can inline across them. The assembly output is identical to a hand-written function that calls each handler by name.

Adding a new consumer struct generates zero new code from the back-end or front-end developer. The compiler reuses the handlers that already exist. Adding a new field type means one new `MORPHEME` line and one new handler. Nothing else changes anywhere.

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

### `DisAgg::disaggregate_inplace(s, policy)`

Same as `disaggregate` but takes `s` by reference and modifies it in place. Use this when the policy needs to store references to fields that outlive the call — for example, an accumulator that collects field addresses to build a batched query and assigns results after the traversal.

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
cmake -B build -S DisAggStruct -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/DisAggStruct        # Linux / macOS
build\DisAggStruct.exe      # Windows
```

Requires C++20. Tested with MSVC 2022; modern GCC and Clang with C++20 support should work.

### Visual Studio 2022

Open `DisAggStruct/DisAggStruct.sln`, set the target to **x64 Release**, then **Build → Build Solution**.

---

## Running

```
=== Sensor Use Cases ===
WeatherReading (3 fields)   temp=21.50  humidity=65.00  pressure=1013.25
Thermometer    (1 field)    temp=21.50
DeviceHealth   (3 fields)   id=42  errorCode=0  active=1
...
[TestSqlAccumulator]  [TestMultiSourceFetch]  [TestFieldCounting]  [TestRetrieve]

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
