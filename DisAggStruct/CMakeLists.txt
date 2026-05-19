cmake_minimum_required(VERSION 3.20)
project(DisAggStruct LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(DisAggStruct
    DisAggStruct/Main.cpp
    DisAggStruct/Tests/Tests.cpp
    DisAggStruct/Sensor/SensorUseCases.cpp
    DisAggStruct/Sensor/SensorBackEnd.cpp
    DisAggStruct/Weather/WeatherBackEnd.cpp
    DisAggStruct/Finance/FinanceBackEnd.cpp
    DisAggStruct/Examples/DomainExamples.cpp
)

target_include_directories(DisAggStruct PRIVATE DisAggStruct)

# /Zc:preprocessor enables the conforming C++20 preprocessor on MSVC (available
# since VS2019 16.5).  Without it MSVC's old preprocessor collapses __VA_ARGS__
# packs when they cross a macro boundary, breaking variadic helper macros like
# MORPHEMES.  The conforming preprocessor is the correct default for C++20 code.
if(MSVC)
    target_compile_options(DisAggStruct PRIVATE /Zc:preprocessor)
endif()
