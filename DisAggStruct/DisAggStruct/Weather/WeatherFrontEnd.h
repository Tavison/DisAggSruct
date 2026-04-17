/**
 * @file WeatherFrontEnd.h
 * @brief Front-end API for the weather domain.
 *
 * @par Role: Front-end implementer
 * One template function per operation family.  Written once, never changes.
 * The struct passed in defines what is requested — any combination of
 * weather types, in any order, in any quantity.
 *
 * Because this file includes only WeatherBackEnd.h, passing a struct
 * containing a non-weather type (e.g. StockPrice) is a compile error.
 * The domain wall is enforced entirely by the include graph.
 */

#pragma once
#include "Weather/WeatherBackEnd.h"
#include "Core/DisAggStruct.h"

/// @brief Populate any weather struct from the back end.
template<typename T>
T FetchWeather(T s) {
    return DisAgg::disaggregate(s, [](auto& f) { retrieve(f); });
}

/// @brief Persist any weather struct to the back end.
template<typename T>
T LogWeather(T s) {
    return DisAgg::disaggregate(s, [](auto& f) { save(f); });
}
