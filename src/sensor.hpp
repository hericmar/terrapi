#pragma once

#include "pch.h"
#include "switch.hpp"
#include "terra.h"

#include <ctime>

#define DHT_MAX_TIMINGS 85

#define GPIO_TIMER 9999

namespace Terra
{
enum class EPhysicalQuantityType
{
    Temperature = 0,
    Humidity,
    Time,
    COUNT
};
constexpr std::array<const char*, (unsigned) EPhysicalQuantityType::COUNT> physicalQuantities{"temperature", "humidity",
                                                                                              "time"};

enum class ESensorType
{
    DHT11 = 0,
    COUNT
};
constexpr std::array<const char*, static_cast<unsigned>(ESensorType::COUNT)> sensors{"DHT11"};


/// Represents a interval in which a switch should be active.
struct ActiveInterval
{
    float m_from = FLT_MIN;
    float m_to   = FLT_MAX;
};

/// Each sensor must have its own physical quantity.
class PhysicalSensor
{
public:
    /// \param gpio wiring pi pin ID.
    explicit PhysicalSensor(int gpio)
    {
        for (auto& value : m_values) value = FLT_MIN; // Set value to default value.

        m_gpio = gpio;
    }

    /// Each sensor has its own way how to perform measurement.
    virtual void Measure() = 0;

    /// Get desired value.
    /// \param type physical quantity.
    float GetValue(EPhysicalQuantityType type) { return m_values[(unsigned) type]; }

    unsigned GetGPIO() const { return m_gpio; };

protected:
    /// Measured values. Each sensor has by default all values measured.
    /// If any value is not supported by the sensor, its value is FLT_MIN.
    std::array<float, (unsigned) EPhysicalQuantityType::COUNT> m_values{};

    /// Set measured value (used in measuring proccess).
    /// \param type value type.
    /// \param value measured value.
    void SetValue(EPhysicalQuantityType type, float value) { m_values[(unsigned) type] = value; }

    /// Data GPIO (WiringPi numbering).
    unsigned m_gpio{};
};

/// Logical sensor.
/// It is used for generic sensor definition.
/// It stores reference to a "physical" sensor and measure one value
/// only.
class Sensor
{
public:
    /// Create a logical representation of the one measured value.
    /// \param physicalQuantity watched value.
    /// \param sensor physical sensor.
    /// \param id unique id!
    explicit Sensor(EPhysicalQuantityType physicalQuantity, class PhysicalSensor* sensor, unsigned id)
    {
        m_physicalSensor   = sensor;
        m_physicalQuantity = physicalQuantity;
        m_id               = id;
    }

    /// Set interval in which assigned switches should be active.
    /// \param from min value.
    /// \param to max value.
    void SetActiveInterval(float from, float to)
    {
        m_activeInterval.m_from = from;
        m_activeInterval.m_to   = to;
    }

    /// Add new switch dependent to measured phys. quantity.
    /// \param aSwitch
    void SetSwitch(class Switch* aSwitch) { m_switches.push_back(aSwitch); }

    /// Perform measurement and update switches.
    void Update();

    [[nodiscard]] unsigned GetId() const { return m_id; };

    /// Sensor has only one measured value!
    float GetValue() const { return m_physicalSensor->GetValue(m_physicalQuantity); }

    EPhysicalQuantityType GetPhysQuantity() const { return m_physicalQuantity; }

private:
    friend class App;

    /// Watched physical quantity.
    EPhysicalQuantityType m_physicalQuantity;

    /// Interval for switch a switch on.
    ActiveInterval m_activeInterval;

    /// PhysicalSensor reference.
    class PhysicalSensor* m_physicalSensor;

    unsigned m_id{};

    /// Switches which depends on the sensor.
    std::vector<class Switch*> m_switches;
};

class DHT11 : public PhysicalSensor
{
public:
    explicit DHT11(int gpio) : PhysicalSensor(gpio){};

    void Measure() override
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
                h = data[0]; // for DHT11
            }
            float c = (float) (((data[2] & 0x7F) << 8) + data[3]) / 10;
            if (c > 125)
            {
                c = data[2]; // for DHT11
            }
            if (data[2] & 0x80) { c = -c; }

            SetValue(EPhysicalQuantityType::Temperature, c);
            SetValue(EPhysicalQuantityType::Humidity, h);
        }
        else
        {
            // printf("Data not good, skip\n");
        }
    };

private:
    int data[5] = {0, 0, 0, 0, 0};
};

class Timer : public PhysicalSensor
{
public:
    Timer(int gpio) : PhysicalSensor(gpio){};

    inline static float TimeToFloat(std::tm& time)
    {
        return (float) time.tm_hour / 24.0f + (1.0f / 24.0f) * ((float) time.tm_min / 60.0f);
    };

    /// Timer measure time ratio.
    void Measure() override
    {
        std::time_t t = std::time(0);
        std::tm* now  = std::localtime(&t);

        float result = TimeToFloat(*now);

        SetValue(EPhysicalQuantityType::Time, result);
    }

private:
};
} // namespace Terra