cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# export for auto completion
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set build type
IF(DEFINED CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are:  Debug Release RelWithDebInfo MinSizeRel.")
ELSE()
    SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are:  Debug Release RelWithDebInfo MinSizeRel.")
ENDIF()

add_subdirectory(src)
