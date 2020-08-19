#pragma once

#include <stdio.h>
#include <wiringPi.h>

#define GPIO_ON     HIGH
#define GPIO_OFF    LOW

#define LOG_ERROR(...) fprintf(stderr, __VA_ARGS__);
