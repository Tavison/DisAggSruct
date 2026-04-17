/**
 * @file Morpheme.h
 * @brief Zero-friction strong typedef wrapper.
 *
 * A @c Morpheme<Tag, T> is identical to @c T in every practical sense —
 * it constructs from @c T implicitly and converts back to @c T implicitly.
 * All operators are handled by the compiler via that implicit conversion,
 * so @c Morpheme works with any underlying type, numeric or otherwise.
 *
 * The sole purpose of the @c Tag parameter is to make otherwise identical
 * underlying types distinct to the compiler.  Two @c Morpheme types built
 * on the same @c T but different tags cannot be silently mixed, and the
 * disaggregation layer can dispatch to the correct handler for each, while
 * all other code treats them as plain @c T values.
 *
 * @note Arithmetic on two @c Morpheme values decays to @c T (via the implicit
 *       conversion), not back to @c Morpheme.  This is the correct trade-off
 *       when @c T may be non-numeric: the tag does its job at the dispatch
 *       boundary, and expressions behave exactly like plain @c T everywhere else.
 *
 * @par Defining a named type
 * @code
 *   // The tag struct can be forward-declared inline in the template argument.
 *   // No macro needed — this is one line of standard C++.
 *   using UUID   = DisAgg::Morpheme<struct UUIDTag,   int>;
 *   using TermID = DisAgg::Morpheme<struct TermIDTag, int>;
 *   using Label  = DisAgg::Morpheme<struct LabelTag,  std::string>;
 *
 *   UUID   a = 1;    // implicit from int
 *   TermID b = 2;    // implicit from int
 *   int    c = a;    // implicit to int
 * @endcode
 */

#pragma once

namespace DisAgg {

/**
 * @brief Zero-friction strong typedef over an underlying type @c T.
 *
 * @tparam Tag  An incomplete struct used solely as a type discriminator.
 *              Two @c Morpheme instantiations with different @c Tag types
 *              are distinct types even if their @c T is identical.
 * @tparam T    The underlying value type.  May be any copyable type.
 */
template<typename Tag, typename T>
struct Morpheme {

    /// The raw underlying value.  Accessible when an explicit extraction is needed.
    T value{};

    constexpr Morpheme() noexcept = default;

    /**
     * @brief Implicit construction from the underlying type.
     *
     * Allows existing code that produces @c T values to pass them to
     * functions or initialisers expecting a @c Morpheme with no changes.
     *
     * @param v  The raw value to wrap.
     */
    constexpr Morpheme(T v) noexcept : value(v) {}

    /**
     * @brief Implicit conversion back to the underlying type.
     *
     * Allows a @c Morpheme to be passed to any context that expects a @c T
     * without an explicit cast.  The compiler uses this conversion to resolve
     * all operators, so no operator overloads are needed regardless of what
     * @c T is.
     */
    constexpr operator T() const noexcept { return value; }
};

} // namespace DisAgg

/**
 * @brief Declares a named @c Morpheme type in one line.
 *
 * Without this macro the equivalent declaration is two lines — a forward
 * declaration of the tag struct followed by the @c using alias.  The macro
 * collapses that into a single readable statement and keeps the tag type
 * name out of user code entirely.
 *
 * @code
 *   MORPHEME(UUID,        int);
 *   MORPHEME(TermID,      int);
 *   MORPHEME(Temperature, float);
 *   MORPHEME(Label,       std::string);
 * @endcode
 *
 * @param Name  The desired type alias.
 * @param T     The underlying type.
 */
#define MORPHEME(Name, T) \
    using Name = DisAgg::Morpheme<struct Name##Tag, T>
