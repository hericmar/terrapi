set -e

cd lib/paho.mqtt.c/
mkdir -p build && cd build
cmake -G "Visual Studio 17 2022" -A x64 -DPAHO_WITH_SSL=FALSE -DPAHO_BUILD_TESTS=FALSE -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=TRUE ..
cmake --build .
cmake --build . --target install
cd ../../..

cd lib/paho.mqtt.cpp/
mkdir -p build && cd build
cmake -G "Visual Studio 17 2022" -A x64 -DPAHO_MQTT_C_INCLUDE_DIRS="C:/Program Files (x86)/Eclipse Paho C/include" -DPAHO_MQTT_C_LIBRARIES="C:/Program Files (x86)/Eclipse Paho C/lib/paho-mqtt3c.lib" -DPAHO_WITH_SSL=FALSE -DPAHO_BUILD_TESTS=FALSE -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=TRUE ..
cmake --build .
cmake --build . --target install
cd ../../..
