// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#include "doctest/doctest.h"

#include "core/core.h"

using namespace terra;

TEST_CASE("Device test")
{
    auto result = Config::from_file(TERRAPI_PROJECT_ROOT "/etc/terrapi/config.toml");
    REQUIRE(result);

    Core::create(result.value());
}
