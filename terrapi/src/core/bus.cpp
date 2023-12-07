#include "core/bus.h"

namespace terra
{
void Bus::publish()
{
    /// @todo

    events.clear();
}

void Bus::record(EventType event)
{
    events.push_back(event);
}
}
