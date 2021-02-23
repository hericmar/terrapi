cmake_minimum_required(VERSION 3.13)

project(terrapi VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)

include_directories(lib/wiringPi/include)
include_directories(lib/fmt/include)

add_subdirectory(lib/fmt)

set(TERRAPI_SOURCE
        src/ConfParser.cpp
        src/Defs.h
        src/Main.cpp
        src/Log.h
        src/Pch.cpp
        src/Terra.cpp
        src/Sensor.cpp
        src/Environment.h
        src/Environment.cpp
        src/TimeUtils.h
        src/Switch.cpp
        src/Utils.h)

add_executable(${PROJECT_NAME} ${TERRAPI_SOURCE})

target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-lwiringPi>)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt)

# Tests
add_executable(terrapi_test
        ${TERRAPI_SOURCE}
        test/dummy/wiringPi.c
        test/terrapi/conf_parser_test.hpp
        test/terrapi/Main.cpp)

target_compile_definitions(terrapi_test PRIVATE TERRAPI_PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
target_include_directories(terrapi_test PRIVATE src)
target_link_libraries(terrapi_test PUBLIC fmt)
