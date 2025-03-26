cmake_minimum_required(VERSION 3.10)
project(SimViconTracker CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find VRPN
find_library(VRPN_LIBRARY NAMES vrpn)
find_library(QUAT_LIBRARY NAMES quat)
find_path(VRPN_INCLUDE_DIR vrpn_Tracker.h)
if (NOT VRPN_LIBRARY OR NOT QUAT_LIBRARY OR NOT VRPN_INCLUDE_DIR)
    message(FATAL_ERROR "VRPN library or headers not found!")
endif()

# Find Qt6
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Include directories
include_directories(${VRPN_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)

# List source files
set(SOURCES
    src/main.cpp
    src/SimViconTracker.cpp
    src/RotationUtils.cpp
    src/ViconGUI.cpp
    src/Config.cpp
)

# List headers that need moc processing
set(HEADERS
    include/ViconGUI.h
)

# Generate moc files for headers with Q_OBJECT
qt6_wrap_cpp(MOC_SOURCES ${HEADERS})

# Add executable with sources and moc files
add_executable(sim_vicon_tracker 
    ${SOURCES}
    ${MOC_SOURCES}
)

# Link libraries
target_link_libraries(sim_vicon_tracker PRIVATE 
    ${VRPN_LIBRARY} 
    ${QUAT_LIBRARY} 
    ${CMAKE_THREAD_LIBS_INIT}
    Qt6::Widgets
)

find_package(Threads REQUIRED)