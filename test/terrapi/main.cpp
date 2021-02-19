#include "fmt/format.h"

#include "conf_parser.h"

using namespace Terra;

int main()
{
    ConfigurationParser::ReadFile(fmt::format("{}/example/configuration.ini", TERRAPI_PROJECT_ROOT).c_str());
    auto app = App::Get();
    int a = 1;
}
