cmake_minimum_required(VERSION 3.16)
project(terrapi)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES src/*)
add_executable(terrapi src/main.cpp src/pch.cpp src/conf_parser.cpp src/terra.cpp src/switch.hpp src/sensor.hpp)