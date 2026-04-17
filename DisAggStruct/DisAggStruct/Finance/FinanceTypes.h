/**
 * @file FinanceTypes.h
 * @brief Morpheme type vocabulary for the finance domain.
 *
 * @par Role: System designer
 * Defines every type the finance subsystem can read or write.
 * Entirely separate from WeatherTypes.h — no type is shared
 * between domains, so cross-domain struct usage fails to compile.
 */

#pragma once
#include "Core/Morpheme.h"

namespace Finance {
    MORPHEME(StockPrice,    float);
    MORPHEME(Volume,        long long);
    MORPHEME(MarketCap,     float);
    MORPHEME(DividendYield, float);
    MORPHEME(PriceChange,   float);
}
