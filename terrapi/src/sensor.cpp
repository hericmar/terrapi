#include "terrapi/sensor.h"

#include "terrapi/terrapi.h"

#include "app.h"
#include "chrono.h"

namespace terra
{
bool terra::cmp(EPhysicalQuantity q, Value lhs, Value rhs)
{
    switch (q)
    {
    case EPhysicalQuantity::Humidity:
    case EPhysicalQuantity::Temperature:
        return lhs.float_val < rhs.float_val;
    case EPhysicalQuantity::Time:
        return lhs.int_val < rhs.int_val;
    }

    return false;
}

bool cmp_within(EPhysicalQuantity q, ValueInterval interval, Value value)
{
    return cmp(q, interval[0], value) && cmp(q, value, interval[1]);
}

void SensorController::update() const
{
    if (m_switch_idx == -1) {
        log::warn("Sensor \"{}\" does not have switch assigned.");
        return;
    }

    if (m_q == EPhysicalQuantity::Time) {
        evaluate(m_day_interval);
        return;
    }

    if (static_cast<Clock*>(ctx().m_clock.get())->is_day()) {
        evaluate(m_day_interval);
    } else {
        evaluate(m_night_interval);
    }
}

void SensorController::evaluate(std::optional<ValueInterval> maybe_interval) const
{
    if (!maybe_interval.has_value()) {
        return;
    }

    if (cmp_within(m_q, *maybe_interval, m_sensor->value(m_q))) {
        ctx().m_switches[m_switch_idx].turn_on();
    } else {
        ctx().m_switches[m_switch_idx].turn_off();
    }
}

//------------------------------------------------------------------------------

void DHT11::measure()
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

        m_value[EPhysicalQuantity::Temperature].float_val = c;
        m_value[EPhysicalQuantity::Humidity].float_val = h;
    }
    else
    {
        // printf("Data not good, skip\n");
    }
}

//------------------------------------------------------------------------------

Clock::Clock()
{
    m_value[EPhysicalQuantity::Time].int_val = 0;
}

void Clock::measure()
{
    std::time_t t = std::time(nullptr);
    std::tm* now  = std::localtime(&t);

    m_value[EPhysicalQuantity::Time].int_val = tm_to_seconds(*now);
}

bool Clock::is_day() const
{
    const auto now = m_value.at(EPhysicalQuantity::Time).int_val;

    return ctx().m_daytime[0] <= now && now <= ctx().m_daytime[1];
}

//------------------------------------------------------------------------------

Value PhysicalSensor::value(EPhysicalQuantity q)
{
    if (m_value.find(q) == m_value.end()) {
        log::err("Sensor {} does not have value {}", "kunda", "q");
    }

    return m_value[q];
}
}
