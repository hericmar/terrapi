#pragma once

#include <ctime>
#include <vector>

#include "wiringPi.h"

#include "terrapi/sensor.h"
#include "terrapi/switch.h"
#include "controller.h"

#define GPIO_ON HIGH
#define GPIO_OFF LOW

constexpr inline unsigned int UPDATE_STEP  = 2 * 1000;  /// Measure step in milliseconds.
constexpr inline unsigned int MEASURE_STEP = 10 * 1000;  /// Measure step in milliseconds.
constexpr inline unsigned int DATA_STEP    = 20 * 60 * 1000;

namespace terra
{
struct Context
{
    std::string m_broker_addr;
    std::string m_client_id;
    std::string m_password;

    int m_measure_step   = MEASURE_STEP;
    int m_data_post_step = DATA_STEP;

    int m_daytime[2]{};

    SensorPtr m_clock;
    std::vector<Controller>       m_switch_controllers;
    std::vector<SensorPtr>        m_sensors;
    std::vector<Switch>           m_switches;

    const Clock* get_clock();

    const SensorPtr& get_sensor(const std::string& name);
    int get_sensor_idx(const std::string& name);
};

class App
{
public:
    /// Create new terrapi app from configuration at \path.
    static App* create(const char* path);
    static App* create_from_string(const char* str_config);

    static std::unique_ptr<class HttpClient>& http();

    [[noreturn]] static void run();

    const std::vector<SensorPtr>& sensors() const { return m_ctx.m_sensors; }
    const std::vector<Switch>& switches() const { return m_ctx.m_switches; }

private:
    friend Context& ctx();

    Context                           m_ctx;
    std::unique_ptr<class HttpClient> m_http;
};
}
