// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#pragma once

#include <stdint.h>
#include <functional>

namespace terra
{
/// Now is a parameter to callback functions.
using CallbackFn = std::function<void(uint64_t)>;

class Timer
{
public:
    Timer() = default;
    Timer(uint64_t interval_ms, CallbackFn callback)
        : interval_ms(interval_ms), callback(callback) {}

    void tick(uint64_t now_ms);

private:
    uint64_t   interval_ms  = 0;
    uint64_t   last_tick_ms = 0;
    CallbackFn callback;
};
}
