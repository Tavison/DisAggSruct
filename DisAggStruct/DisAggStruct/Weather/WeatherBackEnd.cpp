/**
 * @file WeatherBackEnd.cpp
 * @brief Weather back-end implementations.
 *
 * @par Role: Back-end implementer
 * In production these would read from weather station hardware or a
 * meteorological service and persist to a time-series database.
 */

#include "Weather/WeatherBackEnd.h"

namespace Weather {
    void retrieve(Temperature& v) { v = Temperature{ 21.5f  }; }
    void retrieve(Humidity&    v) { v = Humidity{    65.0f  }; }
    void retrieve(Pressure&    v) { v = Pressure{    1013.f }; }
    void retrieve(WindSpeed&   v) { v = WindSpeed{   14.2f  }; }
    void retrieve(Rainfall&    v) { v = Rainfall{    2.4f   }; }

    void save(Temperature& v) { (void)v; /* persist to time-series db */ }
    void save(Humidity&    v) { (void)v; }
    void save(Pressure&    v) { (void)v; }
    void save(WindSpeed&   v) { (void)v; }
    void save(Rainfall&    v) { (void)v; }
}
