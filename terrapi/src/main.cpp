#include "terrapi/terrapi.h"

int main(int argc, char** argv)
{
	if (wiringPiSetupGpio() != 0) {
		terra::log::err("Cannot setup GPIOs");
		
		return 1;
	}

	auto* app = terra::App::create("/etc/terrapi/config");
	if (!app) {
		return 1;
	}

	terra::App::run();
}
