# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.23

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2022.2.1\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2022.2.1\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\jackb\CLionProjects\Progetto_API_2022

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\jackb\CLionProjects\Progetto_API_2022\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Progetto_API.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Progetto_API.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Progetto_API.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Progetto_API.dir/flags.make

CMakeFiles/Progetto_API.dir/main.c.obj: CMakeFiles/Progetto_API.dir/flags.make
CMakeFiles/Progetto_API.dir/main.c.obj: ../main.c
CMakeFiles/Progetto_API.dir/main.c.obj: CMakeFiles/Progetto_API.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\jackb\CLionProjects\Progetto_API_2022\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/Progetto_API.dir/main.c.obj"
	C:\PROGRA~1\JETBRA~1\CLION2~1.1\bin\mingw\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/Progetto_API.dir/main.c.obj -MF CMakeFiles\Progetto_API.dir\main.c.obj.d -o CMakeFiles\Progetto_API.dir\main.c.obj -c C:\Users\jackb\CLionProjects\Progetto_API_2022\main.c

CMakeFiles/Progetto_API.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Progetto_API.dir/main.c.i"
	C:\PROGRA~1\JETBRA~1\CLION2~1.1\bin\mingw\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\jackb\CLionProjects\Progetto_API_2022\main.c > CMakeFiles\Progetto_API.dir\main.c.i

CMakeFiles/Progetto_API.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Progetto_API.dir/main.c.s"
	C:\PROGRA~1\JETBRA~1\CLION2~1.1\bin\mingw\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\jackb\CLionProjects\Progetto_API_2022\main.c -o CMakeFiles\Progetto_API.dir\main.c.s

# Object files for target Progetto_API
Progetto_API_OBJECTS = \
"CMakeFiles/Progetto_API.dir/main.c.obj"

# External object files for target Progetto_API
Progetto_API_EXTERNAL_OBJECTS =

Progetto_API.exe: CMakeFiles/Progetto_API.dir/main.c.obj
Progetto_API.exe: CMakeFiles/Progetto_API.dir/build.make
Progetto_API.exe: CMakeFiles/Progetto_API.dir/linklibs.rsp
Progetto_API.exe: CMakeFiles/Progetto_API.dir/objects1.rsp
Progetto_API.exe: CMakeFiles/Progetto_API.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\jackb\CLionProjects\Progetto_API_2022\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable Progetto_API.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\Progetto_API.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Progetto_API.dir/build: Progetto_API.exe
.PHONY : CMakeFiles/Progetto_API.dir/build

CMakeFiles/Progetto_API.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\Progetto_API.dir\cmake_clean.cmake
.PHONY : CMakeFiles/Progetto_API.dir/clean

CMakeFiles/Progetto_API.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\jackb\CLionProjects\Progetto_API_2022 C:\Users\jackb\CLionProjects\Progetto_API_2022 C:\Users\jackb\CLionProjects\Progetto_API_2022\cmake-build-debug C:\Users\jackb\CLionProjects\Progetto_API_2022\cmake-build-debug C:\Users\jackb\CLionProjects\Progetto_API_2022\cmake-build-debug\CMakeFiles\Progetto_API.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Progetto_API.dir/depend

