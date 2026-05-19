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
 * @file DisAggStruct.h
 * @brief Compile-time struct disaggregation and dispatch framework.
 *
 * Provides utilities to:
 *  - Count the number of fields in an aggregate struct at compile time using
 *    a cascading std::is_constructible probe.
 *  - Decompose a struct into a @c std::tuple of references via C++17 structured
 *    bindings, without any runtime overhead.
 *  - Reconstruct a struct from a tuple using aggregate initialisation.
 *  - Disaggregate a struct through a policy callable, where the policy names
 *    a back-end free function and the compiler resolves the correct overload
 *    per field type.
 *
 * @note Only plain aggregates (no user-provided constructors, no virtual
 *       functions, no private/protected non-static data members) are supported.
 *       Structs with more than 16 fields are not supported.
 *
 * @par Example
 * @code
 *   struct Point { int x, y, z; };
 *
 *   // Field count deduced entirely at compile time.
 *   static_assert(DisAgg::field_count_v<Point> == 3);
 *
 *   Point p{ 1, 2, 3 };
 *
 *   // Decompose into a tuple of lvalue references.
 *   auto t = DisAgg::to_tuple(p);          // tuple<int&, int&, int&>
 *
 *   // Reconstruct a new Point from any compatible tuple.
 *   Point p2 = DisAgg::from_tuple<Point>(t);
 *
 *   // Disaggregate through a policy lambda.
 *   Point p3 = DisAgg::disaggregate(p, [](auto& f) { retrieve(f); });
 * @endcode
 */

#pragma once
#include <tuple>
#include <type_traits>
#include <utility>

/**
 * @namespace DisAgg
 * @brief Top-level namespace for the DisAggStruct framework.
 */
namespace DisAgg {

// ── Field counting ────────────────────────────────────────────────────────────

/**
 * @brief Universal probe type that implicitly converts to any type.
 *
 * Used exclusively inside unevaluated @c decltype contexts to test whether
 * a struct @c T can be aggregate-initialised with @c N arguments.  Because
 * the conversion operator is never actually called, no definition is needed.
 *
 * @warning Must never be used in evaluated (runtime) code.
 */
struct any_type {
    /**
     * @brief Implicit conversion to any type @c T.
     * @tparam T The target type.  Unconstrained by design.
     */
    template<typename T>
    constexpr operator T() const noexcept;
};

/**
 * @namespace DisAgg::detail
 * @brief Internal implementation details.  Not part of the public API.
 */
namespace detail {

/**
 * @brief Tests whether @c T can be aggregate-initialised with exactly @c N
 *        arguments of type @c any_type.
 *
 * Uses a C++20 @c requires expression to evaluate the validity of
 * @code T{ any_type{}, any_type{}, ... }  // N times @endcode
 * and returns @c true if the expression is well-formed, @c false otherwise.
 *
 * @note The @c decltype(..., std::true_type{}) / variadic-fallback pattern
 *       that works on Clang/GCC triggers a hard error on MSVC when the
 *       aggregate initialiser has too many arguments — MSVC does not treat
 *       that as a silent SFINAE failure.  A @c requires expression is the
 *       correct tool: the standard mandates it returns @c false rather than
 *       propagating the error, and MSVC implements this correctly.
 *
 * The index sequence @p Is is used only to repeat @c any_type{} exactly @c N
 * times via a comma-operator fold; the actual index values are discarded.
 *
 * @tparam T  The aggregate struct type under test.
 * @tparam Is An index pack of size @c N, produced by @c std::make_index_sequence.
 * @return    @c true if the initialisation is well-formed, @c false otherwise.
 */
template<typename T, std::size_t... Is>
constexpr bool try_construct_n(std::index_sequence<Is...>) {
    return requires { T{ (static_cast<void>(Is), any_type{})... }; };
}

/**
 * @brief Boolean constant that is @c true when @c T is constructible with
 *        exactly @c N @c any_type arguments.
 *
 * @tparam T  Aggregate struct type.
 * @tparam N  Number of constructor arguments to probe.
 */
template<typename T, std::size_t N>
constexpr bool constructible_with_n = try_construct_n<T>(std::make_index_sequence<N>{});

/**
 * @brief Recursively determines the number of fields in aggregate @c T.
 *
 * The cascade works by attempting to construct @c T with @c N+1 @c any_type
 * arguments.  If that succeeds, the field count is at least @c N+1, so the
 * function recurses.  When adding one more argument fails, the current @c N is
 * the exact field count.
 *
 * Because the entire chain is @c constexpr and only touches unevaluated
 * contexts, there is zero runtime cost.
 *
 * @tparam T  The aggregate struct type.
 * @tparam N  Current probe depth (starts at 0, grows with each recursion).
 * @return    The number of fields in @c T.
 */
template<typename T, std::size_t N = 0>
constexpr std::size_t count_fields() {
    if constexpr (constructible_with_n<T, N + 1>)
        return count_fields<T, N + 1>();
    else
        return N;
}

} // namespace detail

/**
 * @brief Compile-time field count for aggregate struct @c T.
 *
 * Strips cv-qualifiers and references from @c T before probing, so
 * @c field_count_v<const Point&> and @c field_count_v<Point> both work.
 *
 * @tparam T  Any aggregate struct type.
 *
 * @par Example
 * @code
 *   struct Vec3 { float x, y, z; };
 *   static_assert(DisAgg::field_count_v<Vec3> == 3);
 * @endcode
 */
template<typename T>
inline constexpr std::size_t field_count_v = detail::count_fields<std::remove_cvref_t<T>>();

// ── Struct → tuple ────────────────────────────────────────────────────────────

/**
 * @brief Decomposes an aggregate struct into a @c std::tuple of lvalue references.
 *
 * Uses C++17 structured bindings together with an @c if @c constexpr ladder
 * keyed on @c field_count_v<T> to bind all fields by reference and pack them
 * into a @c std::tie result.  Because @c field_count_v is a compile-time
 * constant, only the matching branch is instantiated — every other branch is
 * discarded by the compiler.
 *
 * @tparam T  Aggregate struct type.  Deduced from the argument.
 * @param  s  The struct to decompose.  Must outlive the returned tuple.
 * @return    A @c std::tuple<F0&, F1&, ...> holding lvalue references to each
 *            field of @p s in declaration order.
 *
 * @pre  @c T must be an aggregate with between 0 and 16 fields (inclusive).
 *
 * @par Example
 * @code
 *   Point p{ 1, 2, 3 };
 *   auto t = DisAgg::to_tuple(p);   // tuple<int&, int&, int&>
 *   std::get<0>(t) = 99;            // modifies p.x directly
 * @endcode
 */
template<typename T>
auto to_tuple(T& s) {
    static_assert(std::is_aggregate_v<T>, "T must be a plain aggregate struct");
    constexpr std::size_t N = field_count_v<T>;
    static_assert(N <= 16, "to_tuple supports structs with up to 16 fields");

    if constexpr      (N == 0)  { return std::tuple<>{}; }
    else if constexpr (N == 1)  { auto& [a] = s; return std::tie(a); }
    else if constexpr (N == 2)  { auto& [a,b] = s; return std::tie(a,b); }
    else if constexpr (N == 3)  { auto& [a,b,c] = s; return std::tie(a,b,c); }
    else if constexpr (N == 4)  { auto& [a,b,c,d] = s; return std::tie(a,b,c,d); }
    else if constexpr (N == 5)  { auto& [a,b,c,d,e] = s; return std::tie(a,b,c,d,e); }
    else if constexpr (N == 6)  { auto& [a,b,c,d,e,f] = s; return std::tie(a,b,c,d,e,f); }
    else if constexpr (N == 7)  { auto& [a,b,c,d,e,f,g] = s; return std::tie(a,b,c,d,e,f,g); }
    else if constexpr (N == 8)  { auto& [a,b,c,d,e,f,g,h] = s; return std::tie(a,b,c,d,e,f,g,h); }
    else if constexpr (N == 9)  { auto& [a,b,c,d,e,f,g,h,i] = s; return std::tie(a,b,c,d,e,f,g,h,i); }
    else if constexpr (N == 10) { auto& [a,b,c,d,e,f,g,h,i,j] = s; return std::tie(a,b,c,d,e,f,g,h,i,j); }
    else if constexpr (N == 11) { auto& [a,b,c,d,e,f,g,h,i,j,k] = s; return std::tie(a,b,c,d,e,f,g,h,i,j,k); }
    else if constexpr (N == 12) { auto& [a,b,c,d,e,f,g,h,i,j,k,l] = s; return std::tie(a,b,c,d,e,f,g,h,i,j,k,l); }
    else if constexpr (N == 13) { auto& [a,b,c,d,e,f,g,h,i,j,k,l,m] = s; return std::tie(a,b,c,d,e,f,g,h,i,j,k,l,m); }
    else if constexpr (N == 14) { auto& [a,b,c,d,e,f,g,h,i,j,k,l,m,n] = s; return std::tie(a,b,c,d,e,f,g,h,i,j,k,l,m,n); }
    else if constexpr (N == 15) { auto& [a,b,c,d,e,f,g,h,i,j,k,l,m,n,o] = s; return std::tie(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o); }
    else                        { auto& [a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p] = s; return std::tie(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p); }
}

// ── Tuple → struct ────────────────────────────────────────────────────────────

/**
 * @brief Reconstructs an aggregate struct @c T from a tuple.
 *
 * Uses @c std::apply to unpack the tuple elements as arguments to @c T{...},
 * which performs aggregate initialisation.  Works with both value-tuples and
 * reference-tuples (such as those produced by @c to_tuple).
 *
 * @tparam T      The aggregate struct type to construct.  Must be specified
 *                explicitly since it cannot be deduced from the tuple alone.
 * @tparam Tuple  Any tuple-like type whose elements are compatible with the
 *                fields of @c T in declaration order.
 * @param  t      The source tuple.  Forwarded into the aggregate initialiser,
 *                so move semantics are preserved for value-tuples.
 * @return        A new instance of @c T initialised from the tuple elements.
 *
 * @par Example
 * @code
 *   auto t = std::make_tuple(7, 8, 9);
 *   Point p = DisAgg::from_tuple<Point>(t);  // p.x=7, p.y=8, p.z=9
 * @endcode
 */
template<typename T, typename Tuple>
T from_tuple(Tuple&& t) {
    return std::apply(
        [](auto&&... args) -> T {
            return T{ std::forward<decltype(args)>(args)... };
        },
        std::forward<Tuple>(t)
    );
}

// ── for_each_field ────────────────────────────────────────────────────────────

/**
 * @brief Applies a callable to every element of a tuple.
 *
 * Visits each element in index order and calls @p fn on it.  The primary
 * use case is building dispatchers that call an overloaded handler function
 * on each struct field without any knowledge of the specific types involved.
 *
 * @tparam Tuple  Tuple type, typically a reference-tuple from @c to_tuple.
 * @tparam Fn     Unary callable.  Must be invocable with every element type
 *                in @p t — a missing overload is a compile error by design.
 * @param  t      The tuple to iterate.
 * @param  fn     The function to call on each element.
 *
 * @par Example
 * @code
 *   // Define one overload per field type — the compiler enforces completeness.
 *   void process(UUID&      v) { ... }
 *   void process(ErrorCode& v) { ... }
 *
 *   auto dispatcher = [](auto& t) {
 *       DisAgg::for_each_field(t, [](auto& field) { process(field); });
 *       return t;
 *   };
 * @endcode
 */
template<typename Tuple, typename Fn>
void for_each_field(Tuple& t, Fn&& fn) {
    constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<Tuple>>;
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        (fn(std::get<Is>(t)), ...);
    }(std::make_index_sequence<N>{});
}

// ── Recursive field visitor ───────────────────────────────────────────────────

namespace detail {

// Forward declaration — visit_all and visit_field are mutually recursive.
template<typename Field, typename Policy>
void visit_field(Field& f, Policy& policy);

/**
 * @brief Decomposes @p s and calls visit_field on each of its fields.
 */
template<typename T, typename Policy>
void visit_all(T& s, Policy& policy) {
    auto t = to_tuple(s);
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        (visit_field(std::get<Is>(t), policy), ...);
    }(std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<decltype(t)>>>{});
}

/**
 * @brief Dispatches a single field: recurses into nested aggregates, calls
 *        policy on leaves (Morpheme types, primitives, and anything else that
 *        is not a plain aggregate struct).
 *
 * @c Morpheme has a user-provided constructor and therefore fails
 * @c std::is_aggregate_v — it correctly lands in the @c else branch.
 * Only plain structs with no user-provided constructors (nested structs) take
 * the recursive path.  Arrays are excluded to avoid misuse.
 */
template<typename Field, typename Policy>
void visit_field(Field& f, Policy& policy) {
    using F = std::remove_cvref_t<Field>;
    if constexpr (std::is_aggregate_v<F> && !std::is_array_v<F>) {
        visit_all(f, policy);
    } else {
        policy(f);
    }
}

} // namespace detail

// ── disaggregate ─────────────────────────────────────────────────────────────

/**
 * @brief Primary public entry point.
 *
 * Disaggregates @p s recursively, applies @p policy to every leaf field, and
 * returns the modified copy as @c T.  Fields that are themselves plain
 * aggregate structs are recursed into automatically — @p policy is called only
 * on non-aggregate leaves (Morpheme types, primitives, etc.).
 *
 * @p policy is any callable with a generic @c operator() — typically a lambda
 * that calls a named free function and lets the compiler resolve the correct
 * overload for each field type via ADL.  The policy knows the function name; it
 * does not know the field types.  A field type with no matching overload is a
 * compile error, enforcing handler completeness at build time.
 *
 * @tparam T       Aggregate struct type.  Deduced from @p s.
 * @tparam Policy  Any callable: lambda, function object, or @c std::function.
 * @param  s       The struct to process.  Passed by value; the caller's
 *                 original is never modified.
 * @param  policy  The callable to invoke on each leaf field.
 * @return         A new @c T with each field populated by its handler.
 *
 * @par Flat struct example
 * @code
 *   void retrieve(Temperature& v) { v = Temperature{ sensor.read() }; }
 *
 *   Reading get_reading() {
 *       return DisAgg::disaggregate(Reading{}, [](auto& f) { retrieve(f); });
 *   }
 * @endcode
 *
 * @par Nested struct example
 * @code
 *   struct GeoCoord   { Latitude lat; Longitude lon; };
 *   struct StationData { GeoCoord location; Temperature temp; };
 *
 *   // GeoCoord is a plain aggregate — disaggregate recurses into it.
 *   // retrieve is called on Latitude, Longitude, and Temperature individually.
 *   StationData s = DisAgg::disaggregate(StationData{}, [](auto& f) { retrieve(f); });
 * @endcode
 */
template<typename T, typename Policy>
T disaggregate(T s, Policy policy) {
    detail::visit_all(s, policy);
    return s;
}

} // namespace DisAgg
