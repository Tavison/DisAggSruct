/**
 * @file WeatherTypes.h
 * @brief Morpheme type vocabulary for the weather domain.
 *
 * @par Role: System designer
 * Defines every type the weather subsystem can read or write.
 * Both the back-end and any front-end function in this domain
 * draw exclusively from this list.
 */

#pragma once
#include "Core/Morpheme.h"

namespace Weather {
    MORPHEME(Temperature, float);
    MORPHEME(Humidity,    float);
    MORPHEME(Pressure,    float);
    MORPHEME(WindSpeed,   float);
    MORPHEME(Rainfall,    float);
}
