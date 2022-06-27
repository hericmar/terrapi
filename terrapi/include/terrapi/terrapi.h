#pragma once

#include <vector>

#include "terrapi/sensor.h"
#include "terrapi/switch.h"

#define GPIO_ON HIGH
#define GPIO_OFF LOW

constexpr inline unsigned int MEASURE_STEP = 2000;  /// Measure step in milliseconds.

namespace terra
{
using Switches = std::vector<Switch>;

struct Context
{
    Switches m_switches;
};

class App
{
public:
    /// Create new terrapi app from configuration at \path.
    static App& create(const char* path);
    static App& create_from_string(const char* str_config);

    const std::vector<Switch>& switches() const { return m_ctx.m_switches; }

private:
    Context m_ctx;
};
}
