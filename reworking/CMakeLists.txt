cmake_minimum_required(VERSION 2.8.9)
project(MultidimensionalAdaptiveIndex)
set (CMAKE_CXX_STANDARD 14)
add_definitions("-Wall")

include_directories(${CMAKE_SOURCE_DIR}/src)

set(SOURCES ${CMAKE_SOURCE_DIR}/src/helpers/column.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/helpers.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/measurements.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/predicate.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/query.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/table.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/abstract_index.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/full_scan.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/indexes.cpp
            ${CMAKE_SOURCE_DIR}/src/index_factory.cpp
            ${CMAKE_SOURCE_DIR}/src/main.cpp
            )

add_executable(main ${SOURCES})