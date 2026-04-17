/**
 * @file FinanceBackEnd.cpp
 * @brief Finance back-end implementations.
 *
 * @par Role: Back-end implementer
 * In production these would query a market data feed and persist
 * trades to a transaction ledger.
 */

#include "Finance/FinanceBackEnd.h"

namespace Finance {
    void retrieve(StockPrice&    v) { v = StockPrice{    182.47f   }; }
    void retrieve(Volume&        v) { v = Volume{        4200000LL }; }
    void retrieve(MarketCap&     v) { v = MarketCap{     2.8e12f   }; }
    void retrieve(DividendYield& v) { v = DividendYield{ 0.0055f   }; }
    void retrieve(PriceChange&   v) { v = PriceChange{   3.12f     }; }

    void save(StockPrice&    v) { (void)v; /* persist to ledger */ }
    void save(Volume&        v) { (void)v; }
    void save(MarketCap&     v) { (void)v; }
    void save(DividendYield& v) { (void)v; }
    void save(PriceChange&   v) { (void)v; }
}
