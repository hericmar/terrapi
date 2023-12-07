#include "core/core.h"
#include "config.h"
#include "logger.h"

namespace terra
{
int init(int argc, char** argv)
{
    const char* config_path = DEFAULT_CONFIG_PATH;
    if (argc >= 2) {
        config_path = argv[1];
    }

    auto maybe_config = Config::from_file(config_path);
    if (!maybe_config) {
        LOG(FATAL, "unable to load config from {}", config_path);
        return 1;
    }

    auto* core = Core::create(std::move(*maybe_config));
    if (!core) {
        LOG(FATAL, "unable to create core");
        return 1;
    }

    core->run();
    delete core;

    return 0;
}
}
