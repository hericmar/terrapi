#include "doctest/doctest.h"

#include "context.h"

using namespace terra;

TEST_CASE("Device test")
{
    auto result = Config::from_file(TERRAPI_PROJECT_ROOT "/etc/terrapi/config.toml");
    REQUIRE(result);

    const auto config = std::move(result.value());

    Context::create(config);

    ctx().run();
}
