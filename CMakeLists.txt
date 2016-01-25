cmake_minimum_required(VERSION 3.2.1)
project(solver)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE debug)
endif()
message("-- CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")
set(EXE solver)
include_directories(inc)
add_executable(${EXE} 
    ./src/Optimizer.cpp 
    ./src/hspice_util.cpp
    ./src/Config.cpp
    ./src/Evolution.cpp
    ./src/main.cpp)

if(CMAKE_INSTALL_PREFIX)
    message("-- Cmake install prefix: ${CMAKE_INSTALL_PREFIX}")
    install(TARGETS ${EXE} RUNTIME DESTINATION bin)
endif()

find_package(Boost 1.57 REQUIRED)
if(Boost_FOUND)
else(Boost_FOUND)
    message(FATAL_ERROR "boost not found")
endif(Boost_FOUND)
