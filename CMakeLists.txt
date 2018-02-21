cmake_minimum_required(VERSION 3.6)
project(multidemionsionalindexing)

set(CMAKE_CXX_STANDARD 11)
include_directories(src/cracking)
include_directories(src/fullindex)
include_directories(src/util)


set(SOURCE_FILES
        src/util/file_manager.cpp
        src/cracking/avl_tree.cpp
        src/cracking/kd_tree.cpp
        src/cracking/standard_cracking.cpp
        src/fullindex/binary_search.cpp
        src/fullindex/bulkloading_bp_tree.cpp
        src/fullindex/hybrid_radix_insert_sort.cpp
       )

add_executable(multidemionsionalindexing ${SOURCE_FILES}  src/main.cpp)

add_executable(GenerateData src/gendata.cpp)

add_executable(ProgressiveIndexing src/util/file_manager.cpp src/cracking/kd_tree.cpp src/progressive/main.cpp)
