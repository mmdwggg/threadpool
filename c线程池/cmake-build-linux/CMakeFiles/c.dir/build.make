# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gly/test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gly/test/cmake-build-linux

# Include any dependencies generated for this target.
include CMakeFiles/c.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/c.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/c.dir/flags.make

CMakeFiles/c.dir/main.c.o: CMakeFiles/c.dir/flags.make
CMakeFiles/c.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gly/test/cmake-build-linux/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/c.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/c.dir/main.c.o   -c /home/gly/test/main.c

CMakeFiles/c.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/c.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gly/test/main.c > CMakeFiles/c.dir/main.c.i

CMakeFiles/c.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/c.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gly/test/main.c -o CMakeFiles/c.dir/main.c.s

CMakeFiles/c.dir/threadpool.c.o: CMakeFiles/c.dir/flags.make
CMakeFiles/c.dir/threadpool.c.o: ../threadpool.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gly/test/cmake-build-linux/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/c.dir/threadpool.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/c.dir/threadpool.c.o   -c /home/gly/test/threadpool.c

CMakeFiles/c.dir/threadpool.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/c.dir/threadpool.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gly/test/threadpool.c > CMakeFiles/c.dir/threadpool.c.i

CMakeFiles/c.dir/threadpool.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/c.dir/threadpool.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gly/test/threadpool.c -o CMakeFiles/c.dir/threadpool.c.s

# Object files for target c
c_OBJECTS = \
"CMakeFiles/c.dir/main.c.o" \
"CMakeFiles/c.dir/threadpool.c.o"

# External object files for target c
c_EXTERNAL_OBJECTS =

c : CMakeFiles/c.dir/main.c.o
c : CMakeFiles/c.dir/threadpool.c.o
c : CMakeFiles/c.dir/build.make
c : CMakeFiles/c.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gly/test/cmake-build-linux/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable c"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/c.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/c.dir/build: c

.PHONY : CMakeFiles/c.dir/build

CMakeFiles/c.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/c.dir/cmake_clean.cmake
.PHONY : CMakeFiles/c.dir/clean

CMakeFiles/c.dir/depend:
	cd /home/gly/test/cmake-build-linux && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gly/test /home/gly/test /home/gly/test/cmake-build-linux /home/gly/test/cmake-build-linux /home/gly/test/cmake-build-linux/CMakeFiles/c.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/c.dir/depend

