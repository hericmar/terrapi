// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#include "doctest/doctest.h"

#include "config.h"

TEST_CASE("Test example config")
{
    auto result = terra::Config::from_file(TERRAPI_PROJECT_ROOT "/etc/terrapi/config.toml");
    REQUIRE(result);

    const auto config = std::move(result.value());
    REQUIRE_EQ(config.sensors.size(), 2);
    REQUIRE_EQ(config.devices.size(), 3);
}
