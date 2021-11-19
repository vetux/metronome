cmake_minimum_required(VERSION 3.17)

project(Metronome)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_RELEASE -O3)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

find_package (Threads REQUIRED)

set(HDR_GUI src/mainwindow.hpp)

file(GLOB_RECURSE SRC src/*.cpp)

qt5_wrap_cpp(SRC_GUI_WRAP ${HDR_GUI})

add_executable(metronome ${SRC} ${SRC_GUI_WRAP})

target_link_libraries(metronome Qt5::Core Qt5::Widgets Threads::Threads sndfile openal)

target_include_directories(metronome PUBLIC include/)
target_include_directories(metronome PUBLIC src/)

file(COPY samples DESTINATION ${CMAKE_CURRENT_BINARY_DIR})