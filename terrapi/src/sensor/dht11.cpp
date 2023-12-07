#include "sensor/dht11.h"

#include "wiringPi.h"

#include "logger.h"

namespace terra
{
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

        values[TEMPERATURE] = c;
        values[HUMIDITY]    = h;

        log_message(TRACE, "temperature: " + std::to_string(c));
        log_message(TRACE, "humidity: " + std::to_string(h));
    }
    else
    {
        log_message(ERR, "DHT11: data not good, skip.");
    }
}
}
