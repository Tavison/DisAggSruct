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
