/**
 * @file SensorBackEnd.cpp
 * @brief Back-end retrieve implementations for the sensor subsystem.
 *
 * @par Role: Back-end implementer
 * In production these would query hardware registers, a database, or a
 * remote service.  The mock values here let the system run without real
 * hardware while demonstrating the full dispatch chain.
 *
 * Each function is completely independent.  They share no state and have
 * no knowledge of each other, of the structs callers will build, or of
 * how many fields any given request will contain.
 */

#include "Sensor/SensorBackEnd.h"

namespace Sensor {
    void retrieve(SensorId&    v) { v = SensorId{    42           }; }
    void retrieve(Temperature& v) { v = Temperature{ 21.5f        }; }
    void retrieve(Pressure&    v) { v = Pressure{    1013.25f     }; }
    void retrieve(Humidity&    v) { v = Humidity{    65.0f        }; }
    void retrieve(Voltage&     v) { v = Voltage{     3.3f         }; }
    void retrieve(ErrorCode&   v) { v = ErrorCode{   0            }; }
    void retrieve(Timestamp&   v) { v = Timestamp{   1713312000LL }; }
    void retrieve(ActiveFlag&  v) { v = ActiveFlag{  true         }; }
}
