#include "terrapi/terrapi.h"

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
    }

    parse_config(g_app->m_ctx, std::string_view(str_config));

    return *g_app;
}
}