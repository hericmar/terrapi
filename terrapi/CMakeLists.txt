cmake_minimum_required(VERSION 3.13)

project(terrapi VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/target/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/target/bin)

include_directories(lib/toml++)
include_directories(lib/wiringPi/include)

if (WIN32)
    set(CURL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/curl/windows/include")
    set(CURL_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/curl/windows/lib/LIBCURL.lib")
    set(CURL_VERSION_STRING 7.84.0)

    include_directories(lib/curl/windows/include)
    link_directories(lib/curl/windows/lib)
endif()

find_package(CURL)

################################################################################

set(TERRAPI_SOURCE
        src/config.cpp
        src/context.cpp
        src/datetime.cpp
        src/expr.cpp
        src/http.cpp
        src/logger.cpp
        src/sensor.cpp
        src/switch.cpp)

add_executable(terrapi ${TERRAPI_SOURCE} src/main.cpp)

# target_compile_options(terrapi PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-lwiringPi>)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(terrapi PRIVATE -lwiringPi)
endif()

# global includes, for app and tests
include_directories(include src)

target_link_libraries(terrapi PUBLIC "${CURL_LIBRARIES}")

################################################################################

# Tests
add_executable(terrapi_test
        ${TERRAPI_SOURCE}
        test/lib/wiringPi.c
        test/terrapi/main.cpp
        test/terrapi/test_config.cpp
        test/terrapi/test_rules.cpp)

target_compile_definitions(terrapi_test PRIVATE
        TERRAPI_PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
target_include_directories(terrapi_test PRIVATE
        include
        src
        lib/doctest)
target_link_libraries(terrapi_test PUBLIC
        "${CURL_LIBRARIES}"
        fmt)
