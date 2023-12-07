#include "core/bus.h"

#include "core/core.h"
#include "http.h"
#include "logger.h"

namespace terra
{
void Bus::publish()
{
    const auto& config = core().config;
    HttpClient client(config.broker);

    const auto request_body = serialize(config.broker.client_id.c_str(), events);

    if (client.make_request("POST", "api/v1/record", request_body)) {
        events.clear();
    }
}

void Bus::record(EventType event)
{
    if (events.size() < MAX_PENDING_RECORDS) {
        events.push_back(event);
    } else {
        LOG(WARN, "too many pending records");
    }
}
}
