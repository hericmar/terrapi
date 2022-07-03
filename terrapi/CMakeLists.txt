cmake_minimum_required(VERSION 3.13)

project(terrapi VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)

include_directories(lib/wiringPi/include)
include_directories(lib/fmt/include)

include_directories(include)

add_subdirectory(lib/fmt)

if (WIN32)
    set(PAHO_MQTT_C_INCLUDE_DIRS "C:/Program Files (x86)/Eclipse Paho C/include")
    set(PAHO_MQTT_C_LIBRARIES "C:/Program Files (x86)/Eclipse Paho C/lib/paho-mqtt3c.lib")
    set(PAHO_MQTT_CPP_INCLUDE_DIRS "C:/Program Files (x86)/paho-mqtt-cpp/include")
    set(PAHO_MQTT_CPP_LIBRARIES "C:/Program Files (x86)/paho-mqtt-cpp/lib/paho-mqttpp3-static.lib")
endif()

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

if (WIN32)
    target_include_directories(terrapi_test PRIVATE ${PAHO_MQTT_C_INCLUDE_DIRS} ${PAHO_MQTT_CPP_INCLUDE_DIRS})
    target_link_libraries(terrapi_test PUBLIC "${PAHO_MQTT_C_LIBRARIES}" "${PAHO_MQTT_CPP_LIBRARIES}")

    target_include_directories(terrapi_test_app PRIVATE ${PAHO_MQTT_C_INCLUDE_DIRS} ${PAHO_MQTT_CPP_INCLUDE_DIRS})
    target_link_libraries(terrapi_test_app PUBLIC "${PAHO_MQTT_C_LIBRARIES}" "${PAHO_MQTT_CPP_LIBRARIES}")
endif()
