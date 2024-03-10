// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#include <functional>
#include <memory>
#include <unordered_map>

#include "core/sensor.h"

#include "hardware/dht11.h"
#include "hardware/htu21d.h"
#include "hardware/signal.h"

namespace terra
{
template <typename T>
using UPtr = std::unique_ptr<T>;

namespace detail
{
template <typename T>
UPtr<T> create_sensor(int gpio)
{
    return std::make_unique<T>(gpio);
}

template <typename T>
UPtr<T> create_i2c_sensor()
{
    return std::make_unique<T>();
}
}

static inline const std::unordered_map<std::string, std::function<UPtr<Sensor>(int)>> KNOWN_GPIO_SENSORS = {
        { "DHT11", detail::create_sensor<DHT11> },
        { "Signal", detail::create_sensor<Signal> },
};

static inline const std::unordered_map<std::string, std::function<UPtr<Sensor>()>> KNOWN_I2C_SENSORS = {
        { "HTU21D", detail::create_i2c_sensor<HTU21D> },
};
}
