#include "sensor.h"

#include <ctime>

#include "wiringPi.h"

#include "datetime.h"
#include "logger.h"

namespace terra
{
Value Sensor::value(ValueType type)
{
    if (values.count(type) == 0) {
        log_message("error", "invalid value");

        return 0;
    }

    return values.at(type);
}

void Sensor::force_value(ValueType type, float value)
{
    log_message("info", "forcing value");
    values[type] = value;
}

//----------------------------------------------------------------------------//

#define DHT_MAX_TIMINGS 85

void DHT11::measure()
{
    int data[5]{};
    
    uint8_t laststate = HIGH;
    uint8_t counter   = 0;
    uint8_t j         = 0, i;

    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    /* pull pin down for 18 milliseconds */
    pinMode(gpio, OUTPUT);
    digitalWrite(gpio, LOW);
    delay(18);

    /* prepare to read the pin */
    pinMode(gpio, INPUT);

    /* detect change and read data */
    for (i = 0; i < DHT_MAX_TIMINGS; i++)
    {
        counter = 0;
        while (digitalRead(gpio) == laststate)
        {
            counter++;
            delayMicroseconds(1);
            if (counter == 255) { break; }
        }
        laststate = digitalRead(gpio);

        if (counter == 255) break;

        /* ignore first 3 transitions */
        if ((i >= 4) && (i % 2 == 0))
        {
            /* shove each bit into the storage bytes */
            data[j / 8] <<= 1;
            if (counter > 16) data[j / 8] |= 1;
            j++;
        }
    }

    /*
     * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
     * print it out if data is good
     */
    if ((j >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)))
    {
        float h = (float) ((data[0] << 8) + data[1]) / 10;
        if (h > 100)
        {
            h = (float) data[0]; // for DHT11
        }
        float c = (float) (((data[2] & 0x7F) << 8) + data[3]) / 10;
        if (c > 125)
        {
            c = (float) data[2]; // for DHT11
        }
        if (data[2] & 0x80) { c = -c; }

        values[ValueType_Temperature] = c;
        values[ValueType_Humidity]    = h;

        log_message("trace", "temperature: " + std::to_string(c));
        log_message("trace", "humidity: " + std::to_string(h));
    }
    else
    {
        log_message("error", "DHT11: data not good, skip.");
    }
}

//----------------------------------------------------------------------------//

void Signal::measure()
{
    const auto liquid_level = digitalRead(gpio);
    // printf("GPIO#%d %d\n", gpio, liquid_level);
    values[ValueType_Signal] = liquid_level;

    log_message("trace", "signal: " + std::to_string(liquid_level));
}

void Clock::measure()
{
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);

    m_value = to_seconds(*now);
}
}
