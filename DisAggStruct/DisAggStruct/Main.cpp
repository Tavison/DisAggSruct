/**
 * @file Main.cpp
 * @brief Entry point.
 */

#include "Tests/Tests.h"
#include "Sensor/SensorUseCases.h"
#include "Examples/DomainExamples.h"

int main() {
    RunUseCases();
    RunDomainExamples();
    return RunAllTests();
}
