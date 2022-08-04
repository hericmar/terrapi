#include "terrapi/terrapi.h"

#include <chrono>
#include <thread>

#include "terrapi/utils.h"

#include "app.h"
#include "config.h"
#include "http.h"
#include "chrono.h"

namespace terra
{
static App* g_app = nullptr;

struct EmptySensor : PhysicalSensor
{
    void measure() override {}
};

const SensorPtr& Context::get_sensor(const std::string& name)
{
    const auto idx = get_sensor_idx(name);

    if (idx == -1) {
        static SensorPtr stub = std::make_unique<EmptySensor>();

        return stub;
    }

    return m_sensors[idx];
}

int Context::get_sensor_idx(const std::string& name)
{
    for (int i = 0; i < m_sensors.size(); ++i) {
        if (m_sensors[i]->name() == name) {
            return i;
        }
    }

    return -1;
}

App* App::create(const char* path)
{
    std::string str_config;

    if (fs::exists(path)) {
        str_config = fs::read(path);
    } else {
	log::err("Config file at \"{}\" does not exist.", path);

	return nullptr;
    }

    return create_from_string(str_config.data());
}

App* App::create_from_string(const char* str_config)
{
    if (g_app == nullptr) {
        g_app = new App;
        g_app->m_ctx.m_clock = std::make_unique<Clock>();
    }

    auto& c = g_app->m_ctx;

    const auto parse_result = parse_config(c, std::string_view(str_config));
    if (!parse_result) {
        log::err("Cannot run TerraPI.");

        return nullptr;
    }

    if (!c.m_broker_addr.empty() && !c.m_client_id.empty()) {
        g_app->m_http = std::make_unique<HttpClient>(c.m_broker_addr);
    }

    if (g_app->m_http != nullptr) {
        const auto result = g_app->m_http->put_config(c, str_config);
        if (!result) {
            log::err("Cannot put config to the server!");
        }
    }

    return g_app;
}

[[noreturn]] void App::run()
{
    if (g_app == nullptr) {
        return;
    }
    auto* app = g_app;

    unsigned int measurement_delta = 0;
    unsigned int data_post_delta = 0;

    while (true) {
        auto t1 = std::chrono::high_resolution_clock::now();

        std::time_t t = std::time(nullptr);
        std::tm* now  = std::localtime(&t);

        app->m_ctx.m_clock->measure();

        if (measurement_delta > app->m_ctx.m_measure_step) {
            for (const auto& sensor : app->m_ctx.m_sensors) {
                sensor->measure();
            }
            measurement_delta = 0;
        }

        for (const auto& controller : ctx().m_controllers) {
            controller.update(*now);
        }

        if (data_post_delta > app->m_ctx.m_data_post_step) {
            app->m_http->post_measurements(app->m_ctx, *now);
            log::info("{}: Data posted to server.", time_to_str(*now));
            data_post_delta = 0;
        }

        auto t2 = std::chrono::high_resolution_clock::now();

        auto elapsed = (unsigned) std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

        float multiplier = (unsigned) std::ceil((float) elapsed / UPDATE_STEP);
        if (multiplier == 0) {
            multiplier = 1.0f;
        }

        unsigned sleep_time = (unsigned) multiplier * UPDATE_STEP - elapsed;

	measurement_delta += (unsigned) multiplier * UPDATE_STEP;
        data_post_delta += (unsigned) multiplier * UPDATE_STEP;

        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
}

std::unique_ptr<class HttpClient>& App::http()
{
    return g_app->m_http;
}

Context& ctx()
{
    return g_app->m_ctx;
}

void test::create_test_app()
{
    g_app = new App{};
}
}
