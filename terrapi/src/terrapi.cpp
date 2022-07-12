#include "terrapi/terrapi.h"

#include <chrono>
#include <thread>

#include "terrapi/utils.h"

#include "app.h"
#include "config.h"
#include "http.h"

namespace terra
{
static App* g_app = nullptr;

App* App::create(const char* path)
{
    std::string str_config;

    if (fs::exists(path)) {
        str_config = fs::read(path);
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