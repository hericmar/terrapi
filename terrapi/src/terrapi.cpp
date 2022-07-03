#include "terrapi/terrapi.h"

#include <chrono>
#include <thread>

#include "mqtt/async_client.h"

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

[[noreturn]] void App::run()
{
    if (g_app == nullptr) {
        return;
    }

    auto* app = g_app;

    mqtt::async_client* client = nullptr;

    //
    if (!app->m_ctx.m_broker_addr.empty()) {
        client = new mqtt::async_client(app->m_ctx.m_broker_addr, app->m_ctx.m_client_id);
    }
    //

    while (true) {
        if (client) {
            auto conn_opts = mqtt::connect_options_builder()
                .clean_session(false)
                .finalize();

            try {
                // Start consumer before connecting to make sure to not miss messages
                client->start_consuming();

                // Connect to the server
                log::info("Connecting to the MQTT server ({}).", app->m_ctx.m_broker_addr);
                auto tok = client->connect(conn_opts);

                // Getting the connect response will block waiting for the
                // connection to complete.
                auto rsp = tok->get_connect_response();

                // If there is no session present, then we need to subscribe, but if
                // there is a session, then the server remembers us and our
                // subscriptions.
                const int QOS = 1;
                const auto topic = fmt::format("{}/config", app->m_ctx.m_client_id);

                if (!rsp.is_session_present())
                    client->subscribe(topic, QOS)->wait();

                // Consume messages
                // This just exits if the client is disconnected.
                // (See some other examples for auto or manual reconnect)
                log::info("Waiting for messages on topic: {}.", topic);

                while (true) {
                    auto msg = client->consume_message();
                    if (!msg) {
                        break;
                    }
                    log::info("{}: {}", topic, msg->to_string());
                }

                // If we're here, the client was almost certainly disconnected.
                // But we check, just to make sure.
                if (client->is_connected()) {
                    log::err("Shutting down and disconnecting from the MQTT server...");
                    client->unsubscribe(topic)->wait();
                    client->stop_consuming();
                    client->disconnect()->wait();
                    log::err("OK" );
                }
                else {
                    log::info("Client was disconnected");
                }
            } catch (const mqtt::exception& exc) {
                log::err(exc.to_string());
                return;
            }
        }

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