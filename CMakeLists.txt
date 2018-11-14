cmake_minimum_required(VERSION 3.6)
project(multidimensionalindexing)

set(CMAKE_CXX_STANDARD 11)
include_directories(src/cracking)
include_directories(src/quasii)
include_directories(src/util)


set(SOURCE_FILES
        src/util/file_manager.cpp
        src/cracking/avl_tree.cpp
        src/cracking/kd_tree.cpp
        src/cracking/standard_cracking.cpp
        src/cracking/sideways/sideways_cracking.cpp
        src/quasii/quasii.cpp
        src/util/util.cpp
        src/test/test.cpp
        )

set(SOURCE_FILES_TPCH
        src/util/file_manager.cpp
        src/cracking/avl_tree.cpp
        src/cracking/kd_tree.cpp
        src/cracking/standard_cracking.cpp
        src/cracking/sideways/sideways_cracking.cpp
        src/quasii/quasii.cpp
        src/util/util.cpp
        src/test/test_tpch.cpp
        )

add_executable(multidimensionalindexing ${SOURCE_FILES}  src/main.cpp)

add_executable(multidimensionalindexing_tpch ${SOURCE_FILES_TPCH}  src/maintpch.cpp)

add_executable(GenerateData src/gendata.cpp)

add_executable(ProgressiveIndexing src/util/file_manager.cpp src/cracking/kd_tree.cpp)
