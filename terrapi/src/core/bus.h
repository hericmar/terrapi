#pragma once

#include <vector>

#include "core/record.h"

// #define MAX_PENDING_RECORDS (8 * 1024)
#define MAX_PENDING_RECORDS (1 * 1024)

namespace terra
{
class BrokerConfig;

/// Bus is a message bus.
class Bus
{
public:
    Bus(const BrokerConfig& config) : broker_config(config) {}

    /// Sends a hello message to the server.
    ///
    /// If server does not respond, the message will be sent again
    /// on the next call to publish().
    void hello();

    /// Send all events to the server.
    void publish();

    /// Push an event to the bus.
    void record(Record record);

private:
    const BrokerConfig& broker_config;
    bool                hello_pending = true;
    std::vector<Record> records;
};
}
