# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.13.1/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.13.1/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build

# Include any dependencies generated for this target.
include CMakeFiles/main.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/main.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/main.dir/flags.make

CMakeFiles/main.dir/src/helpers/column.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/helpers/column.cpp.o: ../src/helpers/column.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/main.dir/src/helpers/column.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/helpers/column.cpp.o -c /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/column.cpp

CMakeFiles/main.dir/src/helpers/column.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/helpers/column.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/column.cpp > CMakeFiles/main.dir/src/helpers/column.cpp.i

CMakeFiles/main.dir/src/helpers/column.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/helpers/column.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/column.cpp -o CMakeFiles/main.dir/src/helpers/column.cpp.s

CMakeFiles/main.dir/src/helpers/helpers.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/helpers/helpers.cpp.o: ../src/helpers/helpers.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/main.dir/src/helpers/helpers.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/helpers/helpers.cpp.o -c /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/helpers.cpp

CMakeFiles/main.dir/src/helpers/helpers.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/helpers/helpers.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/helpers.cpp > CMakeFiles/main.dir/src/helpers/helpers.cpp.i

CMakeFiles/main.dir/src/helpers/helpers.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/helpers/helpers.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/helpers.cpp -o CMakeFiles/main.dir/src/helpers/helpers.cpp.s

CMakeFiles/main.dir/src/helpers/measurements.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/helpers/measurements.cpp.o: ../src/helpers/measurements.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/main.dir/src/helpers/measurements.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/helpers/measurements.cpp.o -c /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/measurements.cpp

CMakeFiles/main.dir/src/helpers/measurements.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/helpers/measurements.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/measurements.cpp > CMakeFiles/main.dir/src/helpers/measurements.cpp.i

CMakeFiles/main.dir/src/helpers/measurements.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/helpers/measurements.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/measurements.cpp -o CMakeFiles/main.dir/src/helpers/measurements.cpp.s

CMakeFiles/main.dir/src/helpers/predicate.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/helpers/predicate.cpp.o: ../src/helpers/predicate.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/main.dir/src/helpers/predicate.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/helpers/predicate.cpp.o -c /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/predicate.cpp

CMakeFiles/main.dir/src/helpers/predicate.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/helpers/predicate.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/predicate.cpp > CMakeFiles/main.dir/src/helpers/predicate.cpp.i

CMakeFiles/main.dir/src/helpers/predicate.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/helpers/predicate.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/predicate.cpp -o CMakeFiles/main.dir/src/helpers/predicate.cpp.s

CMakeFiles/main.dir/src/helpers/query.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/helpers/query.cpp.o: ../src/helpers/query.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/main.dir/src/helpers/query.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/helpers/query.cpp.o -c /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/query.cpp

CMakeFiles/main.dir/src/helpers/query.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/helpers/query.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/query.cpp > CMakeFiles/main.dir/src/helpers/query.cpp.i

CMakeFiles/main.dir/src/helpers/query.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/helpers/query.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/query.cpp -o CMakeFiles/main.dir/src/helpers/query.cpp.s

CMakeFiles/main.dir/src/helpers/table.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/helpers/table.cpp.o: ../src/helpers/table.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/main.dir/src/helpers/table.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/helpers/table.cpp.o -c /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/table.cpp

CMakeFiles/main.dir/src/helpers/table.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/helpers/table.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/table.cpp > CMakeFiles/main.dir/src/helpers/table.cpp.i

CMakeFiles/main.dir/src/helpers/table.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/helpers/table.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/helpers/table.cpp -o CMakeFiles/main.dir/src/helpers/table.cpp.s

CMakeFiles/main.dir/src/indexes/abstract_index.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/indexes/abstract_index.cpp.o: ../src/indexes/abstract_index.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/main.dir/src/indexes/abstract_index.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/indexes/abstract_index.cpp.o -c /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/indexes/abstract_index.cpp

CMakeFiles/main.dir/src/indexes/abstract_index.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/indexes/abstract_index.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/indexes/abstract_index.cpp > CMakeFiles/main.dir/src/indexes/abstract_index.cpp.i

CMakeFiles/main.dir/src/indexes/abstract_index.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/indexes/abstract_index.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/indexes/abstract_index.cpp -o CMakeFiles/main.dir/src/indexes/abstract_index.cpp.s

CMakeFiles/main.dir/src/indexes/full_scan.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/indexes/full_scan.cpp.o: ../src/indexes/full_scan.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/main.dir/src/indexes/full_scan.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/indexes/full_scan.cpp.o -c /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/indexes/full_scan.cpp

CMakeFiles/main.dir/src/indexes/full_scan.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/indexes/full_scan.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/indexes/full_scan.cpp > CMakeFiles/main.dir/src/indexes/full_scan.cpp.i

CMakeFiles/main.dir/src/indexes/full_scan.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/indexes/full_scan.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/indexes/full_scan.cpp -o CMakeFiles/main.dir/src/indexes/full_scan.cpp.s

CMakeFiles/main.dir/src/indexes/indexes.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/indexes/indexes.cpp.o: ../src/indexes/indexes.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/main.dir/src/indexes/indexes.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/indexes/indexes.cpp.o -c /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/indexes/indexes.cpp

CMakeFiles/main.dir/src/indexes/indexes.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/indexes/indexes.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/indexes/indexes.cpp > CMakeFiles/main.dir/src/indexes/indexes.cpp.i

CMakeFiles/main.dir/src/indexes/indexes.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/indexes/indexes.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/indexes/indexes.cpp -o CMakeFiles/main.dir/src/indexes/indexes.cpp.s

CMakeFiles/main.dir/src/index_factory.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/index_factory.cpp.o: ../src/index_factory.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/main.dir/src/index_factory.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/index_factory.cpp.o -c /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/index_factory.cpp

CMakeFiles/main.dir/src/index_factory.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/index_factory.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/index_factory.cpp > CMakeFiles/main.dir/src/index_factory.cpp.i

CMakeFiles/main.dir/src/index_factory.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/index_factory.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/index_factory.cpp -o CMakeFiles/main.dir/src/index_factory.cpp.s

CMakeFiles/main.dir/src/main.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/main.dir/src/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/main.cpp.o -c /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/main.cpp

CMakeFiles/main.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/main.cpp > CMakeFiles/main.dir/src/main.cpp.i

CMakeFiles/main.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/src/main.cpp -o CMakeFiles/main.dir/src/main.cpp.s

# Object files for target main
main_OBJECTS = \
"CMakeFiles/main.dir/src/helpers/column.cpp.o" \
"CMakeFiles/main.dir/src/helpers/helpers.cpp.o" \
"CMakeFiles/main.dir/src/helpers/measurements.cpp.o" \
"CMakeFiles/main.dir/src/helpers/predicate.cpp.o" \
"CMakeFiles/main.dir/src/helpers/query.cpp.o" \
"CMakeFiles/main.dir/src/helpers/table.cpp.o" \
"CMakeFiles/main.dir/src/indexes/abstract_index.cpp.o" \
"CMakeFiles/main.dir/src/indexes/full_scan.cpp.o" \
"CMakeFiles/main.dir/src/indexes/indexes.cpp.o" \
"CMakeFiles/main.dir/src/index_factory.cpp.o" \
"CMakeFiles/main.dir/src/main.cpp.o"

# External object files for target main
main_EXTERNAL_OBJECTS =

main: CMakeFiles/main.dir/src/helpers/column.cpp.o
main: CMakeFiles/main.dir/src/helpers/helpers.cpp.o
main: CMakeFiles/main.dir/src/helpers/measurements.cpp.o
main: CMakeFiles/main.dir/src/helpers/predicate.cpp.o
main: CMakeFiles/main.dir/src/helpers/query.cpp.o
main: CMakeFiles/main.dir/src/helpers/table.cpp.o
main: CMakeFiles/main.dir/src/indexes/abstract_index.cpp.o
main: CMakeFiles/main.dir/src/indexes/full_scan.cpp.o
main: CMakeFiles/main.dir/src/indexes/indexes.cpp.o
main: CMakeFiles/main.dir/src/index_factory.cpp.o
main: CMakeFiles/main.dir/src/main.cpp.o
main: CMakeFiles/main.dir/build.make
main: CMakeFiles/main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Linking CXX executable main"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/main.dir/build: main

.PHONY : CMakeFiles/main.dir/build

CMakeFiles/main.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/main.dir/cmake_clean.cmake
.PHONY : CMakeFiles/main.dir/clean

CMakeFiles/main.dir/depend:
	cd /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build /Users/matheusan/Workspace/MultidimensionalAdaptiveIndexing/reworking/build/CMakeFiles/main.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/main.dir/depend

