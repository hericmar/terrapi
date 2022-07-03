#pragma once

#include <ctime>
#include <vector>

#include "wiringPi.h"

#include "terrapi/sensor.h"
#include "terrapi/switch.h"

#define GPIO_ON HIGH
#define GPIO_OFF LOW

constexpr inline unsigned int MEASURE_STEP = 2000;  /// Measure step in milliseconds.

namespace terra
{

struct Context
{
    std::string m_broker_addr;
    std::string m_client_id;

    int m_daytime[2]{};

    SensorPtr m_clock;
    std::vector<SensorController> m_controllers;
    std::vector<SensorPtr>        m_sensors;
    std::vector<Switch>           m_switches;
};

class App
{
public:
    /// Create new terrapi app from configuration at \path.
    static App& create(const char* path);
    static App& create_from_string(const char* str_config);

    [[noreturn]] static void run();

    const std::vector<Switch>& switches() const { return m_ctx.m_switches; }

private:
    friend Context& ctx();

    Context m_ctx;
};
}
