# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.11

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/matheus/Workspace/College/tpch-dbgen

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/matheus/Workspace/College/tpch-dbgen

# Include any dependencies generated for this target.
include CMakeFiles/dbgen.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/dbgen.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/dbgen.dir/flags.make

CMakeFiles/dbgen.dir/bcd2.c.o: CMakeFiles/dbgen.dir/flags.make
CMakeFiles/dbgen.dir/bcd2.c.o: bcd2.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matheus/Workspace/College/tpch-dbgen/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/dbgen.dir/bcd2.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dbgen.dir/bcd2.c.o   -c /home/matheus/Workspace/College/tpch-dbgen/bcd2.c

CMakeFiles/dbgen.dir/bcd2.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dbgen.dir/bcd2.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/matheus/Workspace/College/tpch-dbgen/bcd2.c > CMakeFiles/dbgen.dir/bcd2.c.i

CMakeFiles/dbgen.dir/bcd2.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dbgen.dir/bcd2.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/matheus/Workspace/College/tpch-dbgen/bcd2.c -o CMakeFiles/dbgen.dir/bcd2.c.s

CMakeFiles/dbgen.dir/bm_utils.c.o: CMakeFiles/dbgen.dir/flags.make
CMakeFiles/dbgen.dir/bm_utils.c.o: bm_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matheus/Workspace/College/tpch-dbgen/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/dbgen.dir/bm_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dbgen.dir/bm_utils.c.o   -c /home/matheus/Workspace/College/tpch-dbgen/bm_utils.c

CMakeFiles/dbgen.dir/bm_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dbgen.dir/bm_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/matheus/Workspace/College/tpch-dbgen/bm_utils.c > CMakeFiles/dbgen.dir/bm_utils.c.i

CMakeFiles/dbgen.dir/bm_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dbgen.dir/bm_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/matheus/Workspace/College/tpch-dbgen/bm_utils.c -o CMakeFiles/dbgen.dir/bm_utils.c.s

CMakeFiles/dbgen.dir/build.c.o: CMakeFiles/dbgen.dir/flags.make
CMakeFiles/dbgen.dir/build.c.o: build.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matheus/Workspace/College/tpch-dbgen/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/dbgen.dir/build.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dbgen.dir/build.c.o   -c /home/matheus/Workspace/College/tpch-dbgen/build.c

CMakeFiles/dbgen.dir/build.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dbgen.dir/build.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/matheus/Workspace/College/tpch-dbgen/build.c > CMakeFiles/dbgen.dir/build.c.i

CMakeFiles/dbgen.dir/build.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dbgen.dir/build.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/matheus/Workspace/College/tpch-dbgen/build.c -o CMakeFiles/dbgen.dir/build.c.s

CMakeFiles/dbgen.dir/driver.c.o: CMakeFiles/dbgen.dir/flags.make
CMakeFiles/dbgen.dir/driver.c.o: driver.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matheus/Workspace/College/tpch-dbgen/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/dbgen.dir/driver.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dbgen.dir/driver.c.o   -c /home/matheus/Workspace/College/tpch-dbgen/driver.c

CMakeFiles/dbgen.dir/driver.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dbgen.dir/driver.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/matheus/Workspace/College/tpch-dbgen/driver.c > CMakeFiles/dbgen.dir/driver.c.i

CMakeFiles/dbgen.dir/driver.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dbgen.dir/driver.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/matheus/Workspace/College/tpch-dbgen/driver.c -o CMakeFiles/dbgen.dir/driver.c.s

CMakeFiles/dbgen.dir/load_stub.c.o: CMakeFiles/dbgen.dir/flags.make
CMakeFiles/dbgen.dir/load_stub.c.o: load_stub.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matheus/Workspace/College/tpch-dbgen/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/dbgen.dir/load_stub.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dbgen.dir/load_stub.c.o   -c /home/matheus/Workspace/College/tpch-dbgen/load_stub.c

CMakeFiles/dbgen.dir/load_stub.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dbgen.dir/load_stub.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/matheus/Workspace/College/tpch-dbgen/load_stub.c > CMakeFiles/dbgen.dir/load_stub.c.i

CMakeFiles/dbgen.dir/load_stub.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dbgen.dir/load_stub.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/matheus/Workspace/College/tpch-dbgen/load_stub.c -o CMakeFiles/dbgen.dir/load_stub.c.s

CMakeFiles/dbgen.dir/permute.c.o: CMakeFiles/dbgen.dir/flags.make
CMakeFiles/dbgen.dir/permute.c.o: permute.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matheus/Workspace/College/tpch-dbgen/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/dbgen.dir/permute.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dbgen.dir/permute.c.o   -c /home/matheus/Workspace/College/tpch-dbgen/permute.c

CMakeFiles/dbgen.dir/permute.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dbgen.dir/permute.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/matheus/Workspace/College/tpch-dbgen/permute.c > CMakeFiles/dbgen.dir/permute.c.i

CMakeFiles/dbgen.dir/permute.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dbgen.dir/permute.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/matheus/Workspace/College/tpch-dbgen/permute.c -o CMakeFiles/dbgen.dir/permute.c.s

CMakeFiles/dbgen.dir/print.c.o: CMakeFiles/dbgen.dir/flags.make
CMakeFiles/dbgen.dir/print.c.o: print.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matheus/Workspace/College/tpch-dbgen/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/dbgen.dir/print.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dbgen.dir/print.c.o   -c /home/matheus/Workspace/College/tpch-dbgen/print.c

CMakeFiles/dbgen.dir/print.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dbgen.dir/print.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/matheus/Workspace/College/tpch-dbgen/print.c > CMakeFiles/dbgen.dir/print.c.i

CMakeFiles/dbgen.dir/print.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dbgen.dir/print.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/matheus/Workspace/College/tpch-dbgen/print.c -o CMakeFiles/dbgen.dir/print.c.s

CMakeFiles/dbgen.dir/rnd.c.o: CMakeFiles/dbgen.dir/flags.make
CMakeFiles/dbgen.dir/rnd.c.o: rnd.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matheus/Workspace/College/tpch-dbgen/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/dbgen.dir/rnd.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dbgen.dir/rnd.c.o   -c /home/matheus/Workspace/College/tpch-dbgen/rnd.c

CMakeFiles/dbgen.dir/rnd.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dbgen.dir/rnd.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/matheus/Workspace/College/tpch-dbgen/rnd.c > CMakeFiles/dbgen.dir/rnd.c.i

CMakeFiles/dbgen.dir/rnd.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dbgen.dir/rnd.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/matheus/Workspace/College/tpch-dbgen/rnd.c -o CMakeFiles/dbgen.dir/rnd.c.s

CMakeFiles/dbgen.dir/rng64.c.o: CMakeFiles/dbgen.dir/flags.make
CMakeFiles/dbgen.dir/rng64.c.o: rng64.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matheus/Workspace/College/tpch-dbgen/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/dbgen.dir/rng64.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dbgen.dir/rng64.c.o   -c /home/matheus/Workspace/College/tpch-dbgen/rng64.c

CMakeFiles/dbgen.dir/rng64.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dbgen.dir/rng64.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/matheus/Workspace/College/tpch-dbgen/rng64.c > CMakeFiles/dbgen.dir/rng64.c.i

CMakeFiles/dbgen.dir/rng64.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dbgen.dir/rng64.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/matheus/Workspace/College/tpch-dbgen/rng64.c -o CMakeFiles/dbgen.dir/rng64.c.s

CMakeFiles/dbgen.dir/speed_seed.c.o: CMakeFiles/dbgen.dir/flags.make
CMakeFiles/dbgen.dir/speed_seed.c.o: speed_seed.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matheus/Workspace/College/tpch-dbgen/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object CMakeFiles/dbgen.dir/speed_seed.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dbgen.dir/speed_seed.c.o   -c /home/matheus/Workspace/College/tpch-dbgen/speed_seed.c

CMakeFiles/dbgen.dir/speed_seed.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dbgen.dir/speed_seed.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/matheus/Workspace/College/tpch-dbgen/speed_seed.c > CMakeFiles/dbgen.dir/speed_seed.c.i

CMakeFiles/dbgen.dir/speed_seed.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dbgen.dir/speed_seed.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/matheus/Workspace/College/tpch-dbgen/speed_seed.c -o CMakeFiles/dbgen.dir/speed_seed.c.s

CMakeFiles/dbgen.dir/text.c.o: CMakeFiles/dbgen.dir/flags.make
CMakeFiles/dbgen.dir/text.c.o: text.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matheus/Workspace/College/tpch-dbgen/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object CMakeFiles/dbgen.dir/text.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dbgen.dir/text.c.o   -c /home/matheus/Workspace/College/tpch-dbgen/text.c

CMakeFiles/dbgen.dir/text.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dbgen.dir/text.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/matheus/Workspace/College/tpch-dbgen/text.c > CMakeFiles/dbgen.dir/text.c.i

CMakeFiles/dbgen.dir/text.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dbgen.dir/text.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/matheus/Workspace/College/tpch-dbgen/text.c -o CMakeFiles/dbgen.dir/text.c.s

# Object files for target dbgen
dbgen_OBJECTS = \
"CMakeFiles/dbgen.dir/bcd2.c.o" \
"CMakeFiles/dbgen.dir/bm_utils.c.o" \
"CMakeFiles/dbgen.dir/build.c.o" \
"CMakeFiles/dbgen.dir/driver.c.o" \
"CMakeFiles/dbgen.dir/load_stub.c.o" \
"CMakeFiles/dbgen.dir/permute.c.o" \
"CMakeFiles/dbgen.dir/print.c.o" \
"CMakeFiles/dbgen.dir/rnd.c.o" \
"CMakeFiles/dbgen.dir/rng64.c.o" \
"CMakeFiles/dbgen.dir/speed_seed.c.o" \
"CMakeFiles/dbgen.dir/text.c.o"

# External object files for target dbgen
dbgen_EXTERNAL_OBJECTS =

dbgen: CMakeFiles/dbgen.dir/bcd2.c.o
dbgen: CMakeFiles/dbgen.dir/bm_utils.c.o
dbgen: CMakeFiles/dbgen.dir/build.c.o
dbgen: CMakeFiles/dbgen.dir/driver.c.o
dbgen: CMakeFiles/dbgen.dir/load_stub.c.o
dbgen: CMakeFiles/dbgen.dir/permute.c.o
dbgen: CMakeFiles/dbgen.dir/print.c.o
dbgen: CMakeFiles/dbgen.dir/rnd.c.o
dbgen: CMakeFiles/dbgen.dir/rng64.c.o
dbgen: CMakeFiles/dbgen.dir/speed_seed.c.o
dbgen: CMakeFiles/dbgen.dir/text.c.o
dbgen: CMakeFiles/dbgen.dir/build.make
dbgen: CMakeFiles/dbgen.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/matheus/Workspace/College/tpch-dbgen/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Linking C executable dbgen"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dbgen.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/dbgen.dir/build: dbgen

.PHONY : CMakeFiles/dbgen.dir/build

CMakeFiles/dbgen.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/dbgen.dir/cmake_clean.cmake
.PHONY : CMakeFiles/dbgen.dir/clean

CMakeFiles/dbgen.dir/depend:
	cd /home/matheus/Workspace/College/tpch-dbgen && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/matheus/Workspace/College/tpch-dbgen /home/matheus/Workspace/College/tpch-dbgen /home/matheus/Workspace/College/tpch-dbgen /home/matheus/Workspace/College/tpch-dbgen /home/matheus/Workspace/College/tpch-dbgen/CMakeFiles/dbgen.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/dbgen.dir/depend
