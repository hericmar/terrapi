#include "tests.h"

#include <cassert>

#include "terrapi/terrapi.h"

//------------------------------------------------------------------------------

static constexpr const char* empty_config = R"(
)";

void test_empty_config()
{
    auto* app = terra::App::create_from_string(empty_config);
    TEST_ASSERT(app);
}

//------------------------------------------------------------------------------

void test_basic_config()
{
    auto* app = terra::App::create(TERRAPI_PROJECT_ROOT "/example/config");
    TEST_ASSERT(app != nullptr);

    TEST_ASSERT(app->switches().size() == 2);
    // TEST_ASSERT(app->switches()[0].name() == "heater");
    // TEST_ASSERT(app->switches()[1].name() == "humidifier");

    // No duplicates.
    TEST_ASSERT(app->sensors().size() == 1);
}

//------------------------------------------------------------------------------

void test_config()
{
    test_empty_config();
    test_basic_config();
}
