cmake_minimum_required(VERSION 3.11)
project(MultidimensionalAdaptiveIndex)
set (CMAKE_CXX_STANDARD 14)
add_definitions("-Wall -Wextra -Wpedantic")

# generate a compilation database
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

# Setting output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Header files
include_directories(${CMAKE_SOURCE_DIR}/include/helpers)
include_directories(${CMAKE_SOURCE_DIR}/include/indexes)
include_directories(${CMAKE_SOURCE_DIR}/include/indexes/kd_tree)
include_directories(${CMAKE_SOURCE_DIR}/include/indexes/quasii)
include_directories(${CMAKE_SOURCE_DIR}/include/data)
include_directories(${CMAKE_SOURCE_DIR}/include/data/generators)
include_directories(${CMAKE_SOURCE_DIR}/include/third_party)

set(SOURCES ${CMAKE_SOURCE_DIR}/src/helpers/column.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/cost_model.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/measurements.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/predicate.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/query.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/workload.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/table.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/full_scan.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/full_scan_candidate_list.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/full_scan_slow.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/progressive_indexing.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/median_kd_tree.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/average_kd_tree.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/cracking_kd_tree.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/cracking_kd_tree_per_dimension.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/quasii/slice.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/quasii.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/kd_tree/kd_tree.cpp
            ${CMAKE_SOURCE_DIR}/src/indexes/kd_tree/kd_node.cpp
            ${CMAKE_SOURCE_DIR}/src/data/generators/uniform_generator.cpp
            ${CMAKE_SOURCE_DIR}/src/data/generators/skewed_generator.cpp
            ${CMAKE_SOURCE_DIR}/src/data/generators/sequential_generator.cpp
            ${CMAKE_SOURCE_DIR}/src/data/generators/periodic_generator.cpp
            ${CMAKE_SOURCE_DIR}/src/data/generators/zoom_generator.cpp
            ${CMAKE_SOURCE_DIR}/src/data/generators/alternating_zoom_generator.cpp
            ${CMAKE_SOURCE_DIR}/src/data/generators/sequential_zoom_generator.cpp
            ${CMAKE_SOURCE_DIR}/src/data/generators/genome_generator.cpp
            ${CMAKE_SOURCE_DIR}/src/data/generators/power_generator.cpp
            )

set(FILTERBENCHSOURCES ${CMAKE_SOURCE_DIR}/src/helpers/column.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/predicate.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/query.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/workload.cpp
            ${CMAKE_SOURCE_DIR}/src/helpers/table.cpp
            ${CMAKE_SOURCE_DIR}/src/data/generators/uniform_generator.cpp
            ${CMAKE_SOURCE_DIR}/src/third_party/bitvector.cpp
            )

add_library(FB_lib ${FILTERBENCHSOURCES})

add_executable(filter_bench_main ${CMAKE_SOURCE_DIR}/src/micro_benchmark/filter_scan.cpp)
target_link_libraries(filter_bench_main FB_lib)

add_library(MDAI_lib ${SOURCES})

add_executable(main ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(main MDAI_lib)

add_executable(uniform_generator ${CMAKE_SOURCE_DIR}/src/data/uniform_generator_main.cpp)
target_link_libraries(uniform_generator MDAI_lib)

add_executable(skewed_generator ${CMAKE_SOURCE_DIR}/src/data/skewed_generator_main.cpp)
target_link_libraries(skewed_generator MDAI_lib)

add_executable(sequential_generator ${CMAKE_SOURCE_DIR}/src/data/sequential_generator_main.cpp)
target_link_libraries(sequential_generator MDAI_lib)

add_executable(periodic_generator ${CMAKE_SOURCE_DIR}/src/data/periodic_generator_main.cpp)
target_link_libraries(periodic_generator MDAI_lib)

add_executable(zoom_generator ${CMAKE_SOURCE_DIR}/src/data/zoom_generator_main.cpp)
target_link_libraries(zoom_generator MDAI_lib)


add_executable(sequential_zoom_generator ${CMAKE_SOURCE_DIR}/src/data/sequential_zoom_generator_main.cpp)
target_link_libraries(sequential_zoom_generator MDAI_lib)


add_executable(alternating_zoom_generator ${CMAKE_SOURCE_DIR}/src/data/alternating_zoom_generator_main.cpp)
target_link_libraries(alternating_zoom_generator MDAI_lib)


add_executable(genome_generator ${CMAKE_SOURCE_DIR}/src/data/genome_generator_main.cpp)
target_link_libraries(genome_generator MDAI_lib)

add_executable(power_generator ${CMAKE_SOURCE_DIR}/src/data/power_generator_main.cpp)
target_link_libraries(power_generator MDAI_lib)

add_subdirectory("test")
