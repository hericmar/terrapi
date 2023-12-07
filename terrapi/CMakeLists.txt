cmake_minimum_required(VERSION 3.13)

project(terrapi VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/target/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/target/bin)

include_directories(lib/toml++)
include_directories(lib/wiringPi/include)

find_package(CURL REQUIRED)

################################################################################

set(TERRAPI_SOURCE
		src/core/bus.cpp
		src/core/context.cpp
		src/core/core.cpp
        src/core/sensor.cpp
        src/core/timer.cpp
        src/config.cpp
        src/datetime.cpp
        src/expr.cpp
        # src/http.cpp
        src/logger.cpp
        src/switch.cpp
        src/terra.cpp)

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
        test/terrapi/common.cpp
        test/terrapi/test_config.cpp
        test/terrapi/test_device.cpp
        test/terrapi/test_rules.cpp
        test/terrapi/test_switch.cpp)

target_compile_definitions(terrapi_test PRIVATE
        TERRAPI_PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
target_include_directories(terrapi_test PRIVATE
        lib/doctest)
target_link_libraries(terrapi_test PUBLIC "${CURL_LIBRARIES}")

# Test client
add_executable(terrapi_test_client
		${TERRAPI_SOURCE}
		test/lib/wiringPi.c
		test/terrapi/common.cpp
		test/terrapi_test_client/main.cpp)

target_include_directories(terrapi_test_client PRIVATE
		test/terrapi)
target_link_libraries(terrapi_test_client PUBLIC "${CURL_LIBRARIES}")
