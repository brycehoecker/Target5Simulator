cmake_minimum_required(VERSION 3.10)

# Set some basic project attributes
project(Simple_SCT_Simulator
	VERSION 0.1
	DESCRIPTION "A Simple SCT Simulator Project")

# Option switches for profiling and debug
option(ENABLE_PROFILING "Enable profiling for the build" OFF)
option(ENABLE_DEBUG "Enable debug symbols for the build" OFF)

# This project will output an executable file
add_executable(${PROJECT_NAME} Simple_SCT_Simulator.cpp)

# Create a simple configuration header
configure_file(config.h.in config.h)

# Include the configuration header in the build
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")

# Profiling with GProf
if(ENABLE_PROFILING)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

# Debugging with Perf or Callgrind
if(ENABLE_DEBUG)
  set(CMAKE_BUILD_TYPE Debug)
endif()
