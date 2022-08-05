#include "terrapi/sensor.h"

#include "terrapi/terrapi.h"

#include "app.h"
#include "chrono.h"
#include "http.h"

namespace terra
{
static const std::string physical_quantity_names[] = {
    "humidity", "temperature", "time"
};

const std::string& to_string(EPhysicalQuantity q)
{
    return physical_quantity_names[(int) q];
}

std::optional<EPhysicalQuantity> from_string(const std::string& str)
{
    int i = 0;
    bool has_value = false;

    for (const auto& name : physical_quantity_names) {
        if (name == str) {
            has_value = true;
            break;
        }
        ++i;
    }

    if (!has_value) {
        return std::nullopt;
    }

    return EPhysicalQuantity(i);
}

//------------------------------------------------------------------------------

float PhysicalSensor::value(EPhysicalQuantity q) const
{
    if (m_value.find(q) == m_value.end()) {
        log::err("Sensor {} does not have value {}", m_name, to_string(q));
    }

    return m_value.at(q);
}

//------------------------------------------------------------------------------

void DHT11::measure(const std::tm& now)
{
    uint8_t laststate = HIGH;
    uint8_t counter   = 0;
    uint8_t j         = 0, i;

    m_data[0] = m_data[1] = m_data[2] = m_data[3] = m_data[4] = 0;

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
            m_data[j / 8] <<= 1;
            if (counter > 16) m_data[j / 8] |= 1;
            j++;
        }
    }

    /*
     * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
     * print it out if data is good
     */
    if ((j >= 40) && (m_data[4] == ((m_data[0] + m_data[1] + m_data[2] + m_data[3]) & 0xFF)))
    {
        float h = (float) ((m_data[0] << 8) + m_data[1]) / 10;
        if (h > 100)
        {
            h = (float) m_data[0]; // for DHT11
        }
        float c = (float) (((m_data[2] & 0x7F) << 8) + m_data[3]) / 10;
        if (c > 125)
        {
            c = (float) m_data[2]; // for DHT11
        }
        if (m_data[2] & 0x80) { c = -c; }

        m_value[EPhysicalQuantity::Temperature] = c;
        m_value[EPhysicalQuantity::Humidity]    = h;

        log::print_measurement(now, EPhysicalQuantity::Temperature, name(), c);
        log::print_measurement(now, EPhysicalQuantity::Humidity, name(), h);
    }
    else
    {
		log::err("DHT11: data not good, skip.");
    }
}

//------------------------------------------------------------------------------

Clock::Clock()
{
    m_value[EPhysicalQuantity::Time] = 0;
}

void Clock::measure(const std::tm& now)
{
    m_value[EPhysicalQuantity::Time] = (float) tm_to_seconds(now);
}

bool Clock::is_day() const
{
    const auto now = (unsigned) m_value.at(EPhysicalQuantity::Time);

    return ctx().m_daytime[0] <= now && now <= ctx().m_daytime[1];
}
}
