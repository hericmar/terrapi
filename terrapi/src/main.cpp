#include "config.h"
#include "context.h"
#include "logger.h"

using namespace terra;

int main(int argc, char** argv)
{
    std::string config_path = "/etc/terrapi/config.toml";
    if (argc >= 2) {
        config_path = argv[1];
    }

    const auto maybe_config = Config::from_file(config_path.c_str());
    if (!maybe_config) {
        log_message("fatal", "unable to load config from" + config_path);
        return 1;
    }

    Context::create(*maybe_config);

    return 0;
}
