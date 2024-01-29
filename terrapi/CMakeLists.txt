cmake_minimum_required(VERSION 3.13)

project(terrapi VERSION 1.0.0)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	message(FATAL_ERROR "Only Linux is supported")
endif()

set(CMAKE_CXX_STANDARD 17 REQUIRED)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/target/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/target/bin)

include_directories(lib/fmt/include)
include_directories(lib/toml++)
include_directories(lib/WiringPi/wiringPi)

find_package(CURL REQUIRED)
add_subdirectory(lib/fmt)
link_libraries(${CURL_LIBRARIES} fmt)

################################################################################

file(GLOB_RECURSE TERRAPI_SOURCE "src/*.cpp")
list(REMOVE_ITEM TERRAPI_SOURCE "${CMAKE_SOURCE_DIR}/src/main.cpp")

add_executable(terrapi ${TERRAPI_SOURCE} src/main.cpp)

# global includes, for app and tests
include_directories(include src)

target_link_libraries(terrapi PRIVATE wiringPi)

################################################################################

file(GLOB_RECURSE WIRING_PI_STUB_SOURCE "test/lib/WiringPi/*.c")

# Tests
add_executable(terrapi_test
        ${TERRAPI_SOURCE}
		${WIRING_PI_STUB_SOURCE}
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

# Test client
add_executable(terrapi_test_client
		${TERRAPI_SOURCE}
		${WIRING_PI_STUB_SOURCE}
		test/terrapi/common.cpp
		test/terrapi_test_client/main.cpp)

target_include_directories(terrapi_test_client PRIVATE
		test/terrapi)
