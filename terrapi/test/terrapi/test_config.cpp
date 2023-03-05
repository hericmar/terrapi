#include "tests.h"

#include "doctest/doctest.h"

#include "terrapi/terrapi.h"

#include "config.h"

//------------------------------------------------------------------------------

static constexpr const char* empty_config = R"(
)";

void test_empty_config()
{
    // auto* app = terra::App::create_from_string(empty_config);
    // TEST_ASSERT(app);
}

TEST_CASE("Test example config")
{
    auto result = terra::Config::from_file(TERRAPI_PROJECT_ROOT "/etc/terrapi/config.toml");
    REQUIRE(result);

    const auto config = std::move(result.value());
    REQUIRE_EQ(config.sensors.size(), 2);
    REQUIRE_EQ(config.switches.size(), 3);
}

//------------------------------------------------------------------------------

void test_config()
{
    test_empty_config();
    // test_basic_config();
}
