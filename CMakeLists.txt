cmake_minimum_required(VERSION 3.2.1)
project(solver)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE debug)
endif()
message("-- CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")
set(EXE de-solver)
set(DE_LIB de)
include_directories(inc)
add_executable(${EXE} 
    src/Optimizer.cpp 
    src/hspice_util.cpp
    src/Config.cpp
    src/Evolution.cpp
    src/main.cpp)
add_library(${DE_LIB} SHARED 
    src/Evolution.cpp
    inc/Evolution.h)

find_package(Boost 1.57 REQUIRED)
if(Boost_FOUND)
else(Boost_FOUND)
    message(FATAL_ERROR "Boost not found")
endif(Boost_FOUND)

if(CMAKE_INSTALL_PREFIX)
    message("-- Cmake install prefix: ${CMAKE_INSTALL_PREFIX}")
    install(TARGETS ${EXE} ${DE_LIB} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
    install(FILES inc/Evolution.h
        DESTINATION inc
        PERMISSIONS OWNER_READ GROUP_READ)
    install(FILES README.md
        DESTINATION doc
        PERMISSIONS OWNER_READ GROUP_READ)
endif()
