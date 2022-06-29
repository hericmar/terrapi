cmake_minimum_required(VERSION 3.13)

project(terrapi VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)

include_directories(lib/wiringPi/include)
include_directories(lib/fmt/include)

include_directories(include)

add_subdirectory(lib/fmt)

################################################################################

set(TERRAPI_SOURCE
        src/config.cpp
        src/sensor.cpp
        src/switch.cpp
        src/terrapi.cpp
        src/utils.cpp)

add_executable(${PROJECT_NAME} ${TERRAPI_SOURCE} src/main.cpp)

target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-lwiringPi>)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt)

################################################################################

# Tests
add_executable(terrapi_test
        ${TERRAPI_SOURCE}
        test/lib/wiringPi.c
        test/terrapi/main.cpp
        test/terrapi/test_config.cpp)

target_compile_definitions(terrapi_test PRIVATE TERRAPI_PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
target_include_directories(terrapi_test PRIVATE src)
target_link_libraries(terrapi_test PUBLIC fmt)

################################################################################

add_executable(terrapi_test_app
        ${TERRAPI_SOURCE}
        test/lib/wiringPi.c
        test/terrapi/test_app.cpp)

target_compile_definitions(terrapi_test_app PRIVATE TERRAPI_PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
target_include_directories(terrapi_test_app PRIVATE src)
target_link_libraries(terrapi_test_app PUBLIC fmt)
