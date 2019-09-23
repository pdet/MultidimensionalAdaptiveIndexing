cmake_minimum_required(VERSION 3.11)
project(MultidimensionalAdaptiveIndex)
set (CMAKE_CXX_STANDARD 14)
add_definitions("-g -Wall")

#Find Packages
find_package (SQLite3)

# Setting output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Header files
include_directories(${CMAKE_SOURCE_DIR}/include/helpers)
include_directories(${CMAKE_SOURCE_DIR}/include/indexes)
include_directories(${CMAKE_SOURCE_DIR}/include/indexes/kd_tree)
include_directories(${CMAKE_SOURCE_DIR}/include/indexes/quasii)
include_directories(${CMAKE_SOURCE_DIR}/include/data)
include_directories(${SQLite3_INCLUDE_DIRS})

# External libs
include_directories(/usr/include)
link_directories(/usr/lib)



set(SOURCES ${CMAKE_SOURCE_DIR}/src/helpers/column.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/measurements.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/predicate.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/query.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/table.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/full_scan.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/cracking_kd_tree_broad.cpp
            #${CMAKE_SOURCE_DIR}/src/indexes/cracking_kd_tree_narrow.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/median_kd_tree.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/average_kd_tree.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/quasii/slice.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/quasii.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/kd_tree/kd_tree.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/kd_tree/kd_node.cpp
            #${CMAKE_SOURCE_DIR}/src/indexes/standard_cracking.cpp
            ${CMAKE_SOURCE_DIR}/src/data/data_reader.cpp
            ${CMAKE_SOURCE_DIR}/src/data/generator.cpp
            )

add_library(MDAI_lib ${SOURCES})
target_link_libraries (MDAI_lib ${SQLite3_LIBRARIES})

add_executable(main ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(main MDAI_lib)

add_executable(generator ${CMAKE_SOURCE_DIR}/src/generator.cpp)
target_link_libraries(generator MDAI_lib)

add_subdirectory("test")


