cmake_minimum_required(VERSION 3.6)
project(multidemionsionalindexing)

set(CMAKE_CXX_STANDARD 11)
include_directories(src/cracking)
include_directories(src/fullindex)
include_directories(src/util)


set(SOURCE_FILES
        src/cracking/avl_tree.cpp
        src/cracking/avl_tree.h
        src/cracking/standard_cracking.h
        src/cracking/standard_cracking.cpp
       )

add_executable(multidemionsionalindexing ${SOURCE_FILES}  src/main.cpp)

add_executable(GenerateData ${SOURCE_FILES}  src/gendata.cpp)
