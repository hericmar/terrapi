// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#include <csignal>
#include "core/core.h"
#include "config.h"
#include "logger.h"

namespace terra
{
void handle_termination(int signal)
{
    LOG(INFO, "received termination signal {}", signal);

    core().shutdown();

    exit(0);
}

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

    signal(SIGABRT, handle_termination);
    signal(SIGINT, handle_termination);
    signal(SIGTERM, handle_termination);

    core->run();
    delete core;

    return 0;
}
}
