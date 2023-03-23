#include "doctest/doctest.h"

#include "context.h"

using namespace terra;

TEST_CASE("Device test")
{
    auto result = Config::from_file(TERRAPI_PROJECT_ROOT "/etc/terrapi/config.toml");
    REQUIRE(result);

    Context::create(result.value());
}
