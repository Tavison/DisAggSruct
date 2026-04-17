/**
 * @file FinanceBackEnd.h
 * @brief Back-end contract for the finance domain.
 *
 * @par Role: Back-end implementer
 * Write one retrieve and one save overload per finance type.
 * The back-end developer knows nothing about weather, sensors,
 * or any other domain — and nothing about the structs callers build.
 */

#pragma once
#include "Finance/FinanceTypes.h"

namespace Finance {
    void retrieve(StockPrice&    v);
    void retrieve(Volume&        v);
    void retrieve(MarketCap&     v);
    void retrieve(DividendYield& v);
    void retrieve(PriceChange&   v);

    void save(StockPrice&    v);
    void save(Volume&        v);
    void save(MarketCap&     v);
    void save(DividendYield& v);
    void save(PriceChange&   v);
}
