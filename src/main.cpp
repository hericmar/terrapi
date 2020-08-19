#include "sensor.hpp"
#include "switch.hpp"

#include <wiringPi.h>

using namespace Terra;

int main(int argc, char** argv)
{
    printf("Raspberry Pi DHT11/DHT22 temperature/humidity test\n");

    if (wiringPiSetup() == -1)
        exit(1);

    SensorWrapper dht11(EPhysicalQuantityType::Humidity, new DHT11(7));
    Switch aSwitch(0);
    dht11.SetSwitch(&aSwitch);
    dht11.SetActiveInterval(75.0, 95.0);

    while (true)
    {
        /*
        dht11.Measure();

        if (dht11.humidity < 80.0f)
            aSwitch.On();
        else
            aSwitch.Off();
        */
        dht11.Update();

        printf("%.1f percent.\n", dht11.GetValue());

        delay(2000);
    }

    return 0;
}
