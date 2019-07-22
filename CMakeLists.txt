cmake_minimum_required(VERSION 2.8.9)
project(MultidimensionalAdaptiveIndex)
set (CMAKE_CXX_STANDARD 14)
add_definitions("-g -Wall")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(SOURCES ${CMAKE_SOURCE_DIR}/src/helpers/column.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/helpers.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/measurements.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/predicate.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/query.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/table.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/abstract_index.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/full_scan.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/cracking_kd_tree_broad.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/cracking_kd_tree_narrow.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/median_kd_tree.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/average_kd_tree.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/indexes.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/kd_tree/kd_tree.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/kd_tree/kd_node.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/index_factory.cpp
            )

add_executable(main ${SOURCES} ${CMAKE_SOURCE_DIR}/src/main.cpp)

add_executable(test_algorithms ${SOURCES} ${CMAKE_SOURCE_DIR}/src/test/test.cpp)

add_executable(generator ${CMAKE_SOURCE_DIR}/src/data/generator.cpp)