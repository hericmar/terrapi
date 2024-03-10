#include "core/bus.h"

#include "core/core.h"
#include "http.h"
#include "logger.h"

namespace terra
{
void Bus::hello()
{
    if (!broker_config.enabled) {
        hello_pending = false;
        return;
    }

    HttpClient client(broker_config);

    if (client.put_client_hello(core().config.raw)) {
        hello_pending = false;
    } else {
        LOG(ERR, "cannot send hello message");
    }
}

void Bus::publish()
{
    if (!broker_config.enabled) {
        return;
    }

    if (hello_pending) {
        hello();
    }

    HttpClient client(broker_config);

    const auto request_body = serialize(broker_config.client_id.c_str(), records);

    if (client.make_request("POST", "/api/v1/records", request_body)) {
        records.clear();
    } else {
        LOG(ERR, "cannot send records");
    }
}

void Bus::record(Record record)
{
    if (!broker_config.enabled) {
        return;
    }

    if (records.size() > MAX_PENDING_RECORDS) {
        LOG(ERR, "too many pending records");
        records.erase(records.begin(), records.begin() + (MAX_PENDING_RECORDS / 2));
    }

    records.push_back(record);
}
}
