#include "terrapi/terrapi.h"

int main(int argc, char** argv)
{
	if (wiringPiSetup() != 0) {
		terra::log::err("Cannot setup wiring PI");
		
		return 1;
	}

	auto* app = terra::App::create("/etc/terrapi/config");
	if (!app) {
		return 1;
	}

	terra::App::run();
}
