cmake_minimum_required(VERSION 3.2.1)
project(solver)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE debug)
endif()
message("-- CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")
include_directories(inc)
set(EXE    de-solver)
set(TEST   de-test)
set(DE_LIB de)

# source list
set(DE_INC
    inc/DE/DERandomF.h
    inc/DE/DEOrigin.h
    inc/DE/strategy/DEInterface.h
    inc/DE/strategy/DEBuiltInStrategy.h)
set(DE_SRC 
    src/DE/DERandomF.cpp
    src/DE/DEOrigin.cpp
    src/DE/strategy/DEInterface.cpp
    src/DE/strategy/DEBuiltInStrategy.cpp)
set(SRC_LIST
    src/Optimizer.cpp 
    src/hspice_util.cpp
    src/Config.cpp
    src/main.cpp
    ${DE_SRC})
add_executable(${TEST}
               test/test.cpp
               ${DE_SRC})
add_executable(${EXE} 
               ${DE_SRC}
               src/Optimizer.cpp 
               src/hspice_util.cpp
               src/Config.cpp
               src/main.cpp)
add_library(${DE_LIB} SHARED ${DE_SRC})

find_package(Boost 1.57 REQUIRED)
if(Boost_FOUND)
else(Boost_FOUND)
    message(FATAL_ERROR "Boost not found")
endif(Boost_FOUND)

if(CMAKE_INSTALL_PREFIX)
    message(STATUS "Cmake install prefix: ${CMAKE_INSTALL_PREFIX}")
    install(TARGETS ${EXE} ${TEST} ${DE_LIB} 
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib)
    install(FILES inc/DifferentialEvolution.h
            DESTINATION inc
            PERMISSIONS OWNER_READ GROUP_READ)
    install(DIRECTORY inc/DE 
            DESTINATION inc)
    install(FILES README.md
            DESTINATION doc
            PERMISSIONS OWNER_READ GROUP_READ)
endif()
