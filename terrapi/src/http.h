// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#pragma once

#include <string>
#include <vector>

#include "core/record.h"

namespace terra
{
class BrokerConfig;

class HttpClient
{
public:
    HttpClient(const BrokerConfig& config);

    bool put_client_hello(const std::string& config_body);

    bool make_request(const char* method, const std::string& path, const std::string& body);

private:
    const BrokerConfig& config;
};

std::string serialize(const char* client_id, const std::vector<Record>& records);
}
