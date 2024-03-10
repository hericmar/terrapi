// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#include "core/timer.h"

namespace terra
{
void Timer::tick(uint64_t now_ms)
{
    if (now_ms - last_tick_ms > interval_ms) {
        callback(now_ms);
        last_tick_ms = now_ms;
    }
}
}
