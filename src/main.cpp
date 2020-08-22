#include "terra.h"
#include "conf_parser.hpp"
#include "sensor.hpp"
#include "switch.hpp"

#include <wiringPi.h>

using namespace Terra;

int main(int argc, char** argv)
{
    printf("Raspberry Pi DHT11/DHT22 temperature/humidity test\n");

    if (wiringPiSetup() == -1)
        exit(1);

    ConfigurationParser::ReadFile("conf_test/example.terra");
    printf("Configuration parsing completed!\n");

    while (true)
    {
        for (const auto& sensor : App::Get().GetSensors())
        {
            sensor->Update();
            delay(MEASURE_STEP);
        }

        App::Get().PrintSensors();
    }

    return 0;
}
