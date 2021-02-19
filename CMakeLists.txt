cmake_minimum_required(VERSION 3.13)

project(terrapi)

set(CMAKE_CXX_STANDARD 17)

include_directories(lib/wiringPi/include)
include_directories(lib/fmt/include)

add_subdirectory(lib/fmt)

add_executable(${PROJECT_NAME}
        src/conf_parser.cpp
        src/main.cpp
        src/pch.cpp
        src/terra.cpp
        src/sensor.cpp
        src/environment.h
        src/time_utils.h
        src/switch.cpp)

target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-lwiringPi>)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt)

# Tests
add_executable(terrapi_test
        src/conf_parser.h
        src/conf_parser.cpp
        src/sensor.hpp
        src/sensor.cpp
        src/switch.hpp
        src/switch.cpp
        src/terra.h
        src/terra.cpp
        test/dummy/wiringPi.c
        test/terrapi/conf_parser_test.hpp
        test/terrapi/main.cpp)

target_compile_definitions(terrapi_test PRIVATE TERRAPI_PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
target_include_directories(terrapi_test PRIVATE src)
target_link_libraries(terrapi_test PUBLIC fmt)
