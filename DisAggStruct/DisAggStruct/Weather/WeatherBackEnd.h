/**
 * @file WeatherBackEnd.h
 * @brief Back-end contract for the weather domain.
 *
 * @par Role: Back-end implementer
 * Write one retrieve and one save overload per weather type.
 * No knowledge of structs, templates, or callers required.
 * Passing a non-weather type to a weather front-end function
 * is a compile error — this header being absent from scope is
 * the entire enforcement mechanism.
 */

#pragma once
#include "Weather/WeatherTypes.h"

namespace Weather {
    void retrieve(Temperature& v);
    void retrieve(Humidity&    v);
    void retrieve(Pressure&    v);
    void retrieve(WindSpeed&   v);
    void retrieve(Rainfall&    v);

    void save(Temperature& v);
    void save(Humidity&    v);
    void save(Pressure&    v);
    void save(WindSpeed&   v);
    void save(Rainfall&    v);
}
