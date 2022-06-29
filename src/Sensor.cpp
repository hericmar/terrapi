#include "Sensor.hpp"

#include "Environment.h"
#include "terrapi/sensor.h"


using namespace Terra;

static std::function<bool(float)> g_dayStrategy;
static std::function<bool(float)> g_nightStrategy;

std::function<bool(float)> SensorController::m_strategy;

SensorController::SensorController(EPhysicalQuantityType physicalQuantity, PhysicalSensor* sensor,
                                   int id)
{
    m_physicalSensor   = sensor;
    m_physicalQuantity = physicalQuantity;
    m_id               = id;

    g_dayStrategy = [&](float measuredValue)
    {
      return m_activeInterval.m_from < measuredValue && measuredValue < m_activeInterval.m_to;
    };

    g_nightStrategy = [&](float measuredValue)
    {
      return m_activeNightInterval.m_from < measuredValue && measuredValue < m_activeNightInterval.m_to;
    };

    if (Terra::IsDay())
        m_strategy = g_dayStrategy;
    else
        m_strategy = g_nightStrategy;
}

void SensorController::Update()
{
    float measuredValue = m_physicalSensor->GetValue(m_physicalQuantity);

    if (m_strategy(measuredValue))
    {
        for (auto& aSwitch : m_switches) { aSwitch->On(); }
    }
    else
    {
        for (auto& aSwitch : m_switches) { aSwitch->Off(); }
    }
}

void DHT11::Measure()
{
    uint8_t laststate = HIGH;
    uint8_t counter   = 0;
    uint8_t j         = 0, i;

    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    /* pull pin down for 18 milliseconds */
    pinMode(m_gpio, OUTPUT);
    digitalWrite(m_gpio, LOW);
    delay(18);

    /* prepare to read the pin */
    pinMode(m_gpio, INPUT);

    /* detect change and read data */
    for (i = 0; i < DHT_MAX_TIMINGS; i++)
    {
        counter = 0;
        while (digitalRead(m_gpio) == laststate)
        {
            counter++;
            delayMicroseconds(1);
            if (counter == 255) { break; }
        }
        laststate = digitalRead(m_gpio);

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

        SetValue(EPhysicalQuantityType::Temperature, c);
        SetValue(EPhysicalQuantityType::Humidity, h);
    }
    else
    {
        // printf("Data not good, skip\n");
    }
}

void Clock::Measure()
{
    std::time_t t = std::time(nullptr);
    std::tm* now  = std::localtime(&t);

    float result = TimeToFloat(*now);

    SetValue(EPhysicalQuantityType::Time, result);
}
