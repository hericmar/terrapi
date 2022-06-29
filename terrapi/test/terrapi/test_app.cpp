#include "terrapi/terrapi.h"

int main()
{
    auto& app = terra::App::create(TERRAPI_PROJECT_ROOT "/example/config");
    terra::App::run();
}
