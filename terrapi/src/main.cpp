#include "terrapi/terrapi.h"

int main(int argc, char** argv)
{
    auto* app = terra::App::create("/etc/terrapi/config");
    if (!app) {
        return 1;
    }

    terra::App::run();
}