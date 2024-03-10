// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#include "sensor.h"

#include <ctime>

#include "logger.h"
#include "utils.h"

namespace terra
{
Value Sensor::value(ValueType type)
{
    if (values.count(type) == 0) {
        LOG(ERR, "invalid value");

        return 0;
    }

    return values.at(type);
}

void Sensor::force_value(ValueType type, float value)
{
    LOG(INFO, "forcing value to " + std::to_string(value));
    values[type] = value;
}

//----------------------------------------------------------------------------//

void Clock::measure()
{
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);

    m_value = to_seconds(*now);
    values[TIME] = (float) m_value;
}
}
