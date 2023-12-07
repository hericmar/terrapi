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
