# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /gpfs/fs1/home/mrahman/custom-tools/cmake-3.23.3/cmake-3.23.3-linux-x86_64/bin/cmake

# The command to remove a file.
RM = /gpfs/fs1/home/mrahman/custom-tools/cmake-3.23.3/cmake-3.23.3-linux-x86_64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/mrahman/custom-tools/SZ3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mrahman/custom-tools/SZ3/build

# Utility rule file for NightlyStart.

# Include any custom commands dependencies for this target.
include CMakeFiles/NightlyStart.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/NightlyStart.dir/progress.make

CMakeFiles/NightlyStart:
	/gpfs/fs1/home/mrahman/custom-tools/cmake-3.23.3/cmake-3.23.3-linux-x86_64/bin/ctest -D NightlyStart

NightlyStart: CMakeFiles/NightlyStart
NightlyStart: CMakeFiles/NightlyStart.dir/build.make
.PHONY : NightlyStart

# Rule to build all files generated by this target.
CMakeFiles/NightlyStart.dir/build: NightlyStart
.PHONY : CMakeFiles/NightlyStart.dir/build

CMakeFiles/NightlyStart.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/NightlyStart.dir/cmake_clean.cmake
.PHONY : CMakeFiles/NightlyStart.dir/clean

CMakeFiles/NightlyStart.dir/depend:
	cd /home/mrahman/custom-tools/SZ3/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mrahman/custom-tools/SZ3 /home/mrahman/custom-tools/SZ3 /home/mrahman/custom-tools/SZ3/build /home/mrahman/custom-tools/SZ3/build /home/mrahman/custom-tools/SZ3/build/CMakeFiles/NightlyStart.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/NightlyStart.dir/depend
