cmake_minimum_required(VERSION 3.10)
project(SimViconTracker CXX)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find VRPN package
find_library(VRPN_LIBRARY NAMES vrpn)
find_library(QUAT_LIBRARY NAMES quat)
find_path(VRPN_INCLUDE_DIR vrpn_Tracker.h)

if (NOT VRPN_LIBRARY OR NOT QUAT_LIBRARY OR NOT VRPN_INCLUDE_DIR)
    message(FATAL_ERROR "VRPN library or headers not found! Please install VRPN.")
endif()

include_directories(${VRPN_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)

# Add executable with all source files
add_executable(sim_vicon_tracker 
    src/main.cpp
    src/SimViconTracker.cpp
    src/RotationUtils.cpp
)

target_link_libraries(sim_vicon_tracker PRIVATE 
    ${VRPN_LIBRARY} 
    ${QUAT_LIBRARY} 
    ${CMAKE_THREAD_LIBS_INIT}
)

find_package(Threads REQUIRED)