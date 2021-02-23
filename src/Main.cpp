#include <wiringPi.h>

#include "Terra.h"

using namespace Terra;

int main(int argc, char** argv)
{
    if (wiringPiSetup() == -1) exit(1);

    App::Get().Initialize("/etc/terrapi/configuration.ini");
    App::Get().Run();

    return 0;
}
