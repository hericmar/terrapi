cmake_minimum_required(VERSION 3.13)

project(terrapi VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)

include_directories(lib/fmt/include)
include_directories(lib/wiringPi/include)

if (WIN32)
    set(CURL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/curl/windows/include")
    set(CURL_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/curl/windows/lib/LIBCURL.lib")
    set(CURL_VERSION_STRING 7.84.0)

    include_directories(lib/curl/windows/include)
    link_directories(lib/curl/windows/lib)

    find_package(CURL)
endif()

add_subdirectory(lib/fmt)

################################################################################

set(TERRAPI_SOURCE
        src/config.cpp
        src/http.cpp
        src/sensor.cpp
        src/switch.cpp
        src/terrapi.cpp
        src/utils.cpp)

add_executable(terrapi ${TERRAPI_SOURCE} src/main.cpp)

target_compile_options(terrapi PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-lwiringPi>)
target_link_libraries(terrapi PUBLIC fmt)

################################################################################

# Tests
add_executable(terrapi_test
        ${TERRAPI_SOURCE}
        test/lib/wiringPi.c
        test/terrapi/main.cpp
        test/terrapi/test_config.cpp)

target_compile_definitions(terrapi_test PRIVATE TERRAPI_PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
target_include_directories(terrapi_test PRIVATE include src)
target_link_libraries(terrapi_test PUBLIC fmt)

# message(FATAL_ERROR ${CURL_LIBRARIES})

################################################################################

add_executable(terrapi_test_app
        ${TERRAPI_SOURCE}
        test/lib/wiringPi.c
        test/terrapi/test_app.cpp)

target_compile_definitions(terrapi_test_app PRIVATE TERRAPI_PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
target_include_directories(terrapi_test_app PRIVATE include src)
target_link_libraries(terrapi_test_app PUBLIC "${CURL_LIBRARIES}" fmt)
