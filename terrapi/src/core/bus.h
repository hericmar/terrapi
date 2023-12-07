#pragma once

#include <vector>

#include "core/record.h"

#define MAX_PENDING_RECORDS (8 * 1024)

namespace terra
{
/// Bus is a message bus.
class Bus
{
public:
    /// Send all events to the server.
    void publish();

    /// Push an event to the bus.
    void record(EventType event);

private:
    std::vector<EventType> events;
};
}
