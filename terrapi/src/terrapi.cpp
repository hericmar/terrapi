#include "terrapi/terrapi.h"

#include <chrono>
#include <thread>

#include "app.h"
#include "config.h"
#include "terrapi/utils.h"

namespace terra
{
static App* g_app = nullptr;

App& App::create(const char* path)
{
    std::string str_config;

    if (fs::exists(path)) {
        str_config = fs::read(path);
    }

    return create_from_string(str_config.data());
}

App& App::create_from_string(const char* str_config)
{
    if (g_app == nullptr) {
        g_app = new App;
        g_app->m_ctx.m_clock = std::make_unique<Clock>();
    }

    parse_config(g_app->m_ctx, std::string_view(str_config));

    return *g_app;
}

void App::run()
{
    if (g_app == nullptr) {
        return;
    }

    auto* app = g_app;

    while (true) {
        app->m_ctx.m_clock->measure();
        for (const auto& sensor : app->m_ctx.m_sensors) {
            sensor->measure();
        }

        for (const auto& controller : ctx().m_controllers) {
            controller.update();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

Context& ctx()
{
    return g_app->m_ctx;
}
}