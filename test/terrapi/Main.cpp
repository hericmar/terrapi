#include "fmt/format.h"

#include "Terra.h"

using namespace Terra;

int main()
{
    auto& app = App::Get();
    app.Initialize(fmt::format("{}/example/configuration.ini", TERRAPI_PROJECT_ROOT));
    app.Run();
}
