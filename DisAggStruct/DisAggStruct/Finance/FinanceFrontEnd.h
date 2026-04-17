/**
 * @file FinanceFrontEnd.h
 * @brief Front-end API for the finance domain.
 *
 * @par Role: Front-end implementer
 * One template function per operation family.  Written once, never changes.
 * The struct passed in defines what is requested — any combination of
 * finance types, in any order, in any quantity.
 *
 * Because this file includes only FinanceBackEnd.h, passing a struct
 * containing a non-finance type (e.g. Temperature) is a compile error.
 * The domain wall is enforced entirely by the include graph.
 */

#pragma once
#include "Finance/FinanceBackEnd.h"
#include "Core/DisAggStruct.h"

/// @brief Populate any finance struct from the back end.
template<typename T>
T FetchQuote(T s) {
    return DisAgg::disaggregate(s, [](auto& f) { retrieve(f); });
}

/// @brief Persist any finance struct to the back end.
template<typename T>
T RecordTrade(T s) {
    return DisAgg::disaggregate(s, [](auto& f) { save(f); });
}
