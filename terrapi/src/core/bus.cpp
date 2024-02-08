#include "core/bus.h"

#include "core/core.h"
#include "http.h"
#include "logger.h"

namespace terra
{
void Bus::publish()
{
    if (!broker_config.enabled) {
        return;
    }

    HttpClient client(broker_config);

    const auto request_body = serialize(broker_config.client_id.c_str(), records);

    if (client.make_request("POST", "/api/v1/records", request_body)) {
        records.clear();
    }
}

void Bus::record(Record record)
{
    if (!broker_config.enabled) {
        return;
    }

    if (records.size() < MAX_PENDING_RECORDS) {
        records.push_back(record);
    } else {
        LOG(ERR, "too many pending records");
    }
}
}
