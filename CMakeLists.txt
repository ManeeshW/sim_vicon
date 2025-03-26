# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(SimViconTracker CXX)

# Set C++ standard to C++11 (VRPN works fine with this)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find VRPN package (assumes VRPN is installed system-wide)
find_library(VRPN_LIBRARY NAMES vrpn)
find_library(QUAT_LIBRARY NAMES quat)  # VRPN's quaternion library
find_path(VRPN_INCLUDE_DIR vrpn_Tracker.h)

# Check if VRPN was found
if (NOT VRPN_LIBRARY OR NOT QUAT_LIBRARY OR NOT VRPN_INCLUDE_DIR)
    message(FATAL_ERROR "VRPN library or headers not found! Please install VRPN and ensure it is in your system paths.")
endif()

# Include directories for VRPN headers
include_directories(${VRPN_INCLUDE_DIR})

# Add the executable
add_executable(sim_vicon_tracker sim_vicon_tracker.cpp)

# Link against VRPN and quat libraries
target_link_libraries(sim_vicon_tracker PRIVATE ${VRPN_LIBRARY} ${QUAT_LIBRARY})

# Optionally, add pthread if required (some systems need it for VRPN)
find_package(Threads)
target_link_libraries(sim_vicon_tracker PRIVATE ${CMAKE_THREAD_LIBS_INIT})