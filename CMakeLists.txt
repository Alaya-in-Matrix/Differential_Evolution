cmake_minimum_required(VERSION 3.2.1)
project(solver)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE debug)
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")
include_directories(inc)
set(EXE       de-solver)
set(TEST      de-test)
set(DE_SHARED de)
set(DE_STATIC de-static)

# source list
set(DE_INC
    inc/DE/SaDE.h
    inc/DE/DERandomF.h
    inc/DE/DEOrigin.h
    inc/DE/strategy/DEInterface.h
    inc/DE/strategy/DEBuiltInStrategy.h)
set(DE_SRC 
    src/DE/DERandomF.cpp
    src/DE/DEOrigin.cpp
    src/DE/strategy/DEInterface.cpp
    src/DE/strategy/DEBuiltInStrategy.cpp)
set(SOLVER_INC
    inc/Config.h
    inc/DifferentialEvolution.h
    inc/hspice_util.h
    inc/Optimizer.h
    inc/util.h)
set(SOLVER_SRC
    src/Config.cpp
    src/hspice_util.cpp
    src/main.cpp
    src/Optimizer.cpp)
set(TEST_INC test/testbench.h)
set(TEST_SRC test/test.cpp)
if(WIN32) # for visual studio
    set(DE_SRC     ${DE_SRC}     ${DE_INC})
    set(SOLVER_SRC ${SOLVER_SRC} ${SOLVER_INC})
    set(TEST_SRC   ${TEST_SRC}   ${TEST_INC})
endif(WIN32)
add_library(${DE_SHARED} SHARED ${DE_SRC})
add_library(${DE_STATIC} STATIC ${DE_SRC})
add_executable(${TEST} ${TEST_SRC})
add_executable(${EXE} ${SOLVER_SRC})
target_link_libraries(${TEST} ${DE_STATIC})
target_link_libraries(${EXE} ${DE_STATIC})

# use can use -DBOOST_ROOT or -DBOOST_INCLUDEDIR and -DBOOST_LIBRARYDIR as hint
find_package(Boost 1.57 REQUIRED)
if(Boost_FOUND)
    message(STATUS "boost inc dir: ${Boost_INCLUDE_DIR}")
    message(STATUS "boost lib dir: ${Boost_LIB_DIR}")
    include_directories(${Boost_INCLUDE_DIR})
else(Boost_FOUND)
    message(FATAL_ERROR "Boost not found")
endif(Boost_FOUND)
find_package(OpenMP)
if(OPENMP_FOUND)
    message(STATUS "Support OpenMP: ${OpenMP_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else(OPENMP_FOUND)
	message(FATAL_ERROR "OpenMP not found")
endif(OPENMP_FOUND)

if(CMAKE_INSTALL_PREFIX)
    message(STATUS "Cmake install prefix: ${CMAKE_INSTALL_PREFIX}")
    install(TARGETS ${DE_SHARED} ${DE_STATIC} ${EXE} ${TEST} 
            RUNTIME DESTINATION bin
            ARCHIVE DESTINATION lib
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
