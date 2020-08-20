#pragma once

#include "pch.h"
#include "terra.h"
#include "switch.hpp"

#define DHT_MAX_TIMINGS    85

namespace Terra
{
    enum class EPhysicalQuantityType
    {
        Temperature = 0,
        Humidity,
        COUNT
    };
    constexpr std::array<const char*, (unsigned) EPhysicalQuantityType::COUNT> physicalQuantities {
            "temperature",
            "humidity"
    };

    enum class ESensorType
    {
        DHT11 = 0,
        COUNT
    };
    constexpr std::array<const char*, (unsigned) ESensorType::COUNT> sensors {
        "DHT11"
    };


    /// Represents a interval in which a switch should be active.
    struct ActiveInterval
    {
        float m_from = FLT_MIN;
        float m_to = FLT_MAX;
    };

    /// Each sensor must have its own physical quantity.
    class PhysicalSensor {
    public:
        ///
        /// \param id nonzero id.
        explicit PhysicalSensor(int id = -1)
        {
            for (auto& value : m_values)
                value = FLT_MIN; // Set value to default value.

            m_gpio = id;
        }

        virtual void Measure() = 0;

        float GetValue(EPhysicalQuantityType type)
        {
            return m_values[(unsigned) type];
        }

        unsigned GetGPIO() { return m_gpio; };

    protected:
        std::array<float, (unsigned) EPhysicalQuantityType::COUNT> m_values{};

        void SetValue(EPhysicalQuantityType type, float value)
        {
            m_values[(unsigned) type] = value;
        }

    private:
        unsigned m_gpio{};
    };

    /*
     * Wrapper for a sensor.
     * It is used for generic sensor definition.
     * It stores reference to a "physical" sensor and measure one value
     * only.
     */
    class Sensor {
    public:
        explicit Sensor(EPhysicalQuantityType physicalQuantity, class PhysicalSensor* sensor, unsigned id)
        {
            m_physicalSensor = sensor;
            m_physicalQuantity = physicalQuantity;
            m_id = id;
        }

        void SetActiveInterval(float from, float to)
        {
            m_activeInterval.m_from = from;
            m_activeInterval.m_to = to;
        }

        void SetSwitch(class Switch* aSwitch)
        {
            m_switches.push_back(aSwitch);
        }

        /// Perform measurement and update switches.
        void Update()
        {
            if (m_physicalSensor == nullptr)
                printf("Debil\n");

            m_physicalSensor->Measure();

            float measuredValue = m_physicalSensor->GetValue(m_physicalQuantity);

            if (m_activeInterval.m_from < measuredValue && measuredValue < m_activeInterval.m_to)
            {
                for (auto aSwitch : m_switches)
                {
                    LOG_DEBUG("Switching on.");
                    aSwitch->On();
                }
            }
            else
            {
                for (auto aSwitch : m_switches)
                {
                    aSwitch->Off();
                }
            }
        }

        unsigned GetId() const { return m_id; };

        float GetValue()
        {
            return m_physicalSensor->GetValue(m_physicalQuantity);
        }

        EPhysicalQuantityType GetPhysQuantity() { return m_physicalQuantity; }

    private:
        // Watched physical quantity.
        EPhysicalQuantityType m_physicalQuantity;

        // Interval for switch a switch on.
        ActiveInterval m_activeInterval;

        // PhysicalSensor reference.
        class PhysicalSensor* m_physicalSensor;

        unsigned m_id{};

        // Switches which depends on the sensor.
        std::vector<class Switch*> m_switches;
    };

    class DHT11 : public PhysicalSensor
    {
    public:
        explicit DHT11(unsigned GPIOPin)
        {
            this->GPIOPin = GPIOPin;
        }

        void Measure() override
        {
            uint8_t laststate = HIGH;
            uint8_t counter = 0;
            uint8_t j = 0, i;

            data[0] = data[1] = data[2] = data[3] = data[4] = 0;

            /* pull pin down for 18 milliseconds */
            pinMode(GPIOPin, OUTPUT);
            digitalWrite(GPIOPin, LOW);
            delay(18);

            /* prepare to read the pin */
            pinMode(GPIOPin, INPUT);

            /* detect change and read data */
            for (i = 0; i < DHT_MAX_TIMINGS; i++)
            {
                counter = 0;
                while (digitalRead(GPIOPin) == laststate) {
                    counter++;
                    delayMicroseconds(1);
                    if (counter == 255)
                    {
                        break;
                    }
                }
                laststate = digitalRead(GPIOPin);

                if (counter == 255)
                    break;

                /* ignore first 3 transitions */
                if ((i >= 4) && (i % 2 == 0))
                {
                    /* shove each bit into the storage bytes */
                    data[j / 8] <<= 1;
                    if (counter > 16)
                        data[j / 8] |= 1;
                    j++;
                }
            }

            /*
             * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
             * print it out if data is good
             */
            if ((j >= 40) &&
                (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)))
            {
                float h = (float) ((data[0] << 8) + data[1]) / 10;
                if (h > 100)
                {
                    h = data[0];    // for DHT11
                }
                float c = (float) (((data[2] & 0x7F) << 8) + data[3]) / 10;
                if (c > 125)
                {
                    c = data[2];    // for DHT11
                }
                if (data[2] & 0x80)
                {
                    c = -c;
                }

                SetValue(EPhysicalQuantityType::Temperature, c);
                SetValue(EPhysicalQuantityType::Humidity, h);
            }
            else
            {
                printf("Data not good, skip\n");
            }
        };

    private:
        unsigned GPIOPin;
        int data[5] = {0, 0, 0, 0, 0};
    };
}