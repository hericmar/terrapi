cmake_minimum_required(VERSION 3.13)

project(terrapi)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} src/conf_parser.cpp src/main.cpp src/pch.cpp src/terra.cpp src/sensor.cpp)

target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-lwiringPi>)
