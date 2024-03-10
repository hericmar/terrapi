// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#include <algorithm>

#include "core/bus.h"
#include "config.h"
#include "http.h"
#include "logger.h"

#include <curl/curl.h>
#include "fmt/format.h"
#include "fmt/ranges.h"

namespace terra
{
static std::string string_replace(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

HttpClient::HttpClient(const BrokerConfig& config)
    : config(config)
{
}

constexpr const char* TEMPLATE_CLIENT_HELLO = R"({{ "client_id": "{}", "config": "{}" }})";

bool HttpClient::put_client_hello(const std::string& config_body)
{
    auto sanitized_config_body = string_replace(config_body, "\"", "\\\"");
    sanitized_config_body = string_replace(sanitized_config_body, "\n", "\\n");

    const auto request_body = fmt::format(TEMPLATE_CLIENT_HELLO, config.client_id, sanitized_config_body);

    return make_request("PUT", "/api/v1/hello", request_body);
}

/// Allows to resize the response read buffer.
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    auto* buffer = (std::string*) userp;
    buffer->append((char*) contents, size * nmemb);

    return size * nmemb;
}

bool HttpClient::make_request(const char* method, const std::string& path, const std::string& body)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG(ERR, "cannot initialize CURL, no data will be loaded to the server");
        return false;
    }

    const std::string url = config.address + path;
    std::string read_buffer;

    bool curl_result = true;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    // headers
    struct curl_slist* headers = nullptr;

    // Remove a header curl would otherwise add by itself
    headers = curl_slist_append(headers, "Accept:");

    headers = curl_slist_append(headers, ("Authorization: Bearer " + config.password).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // data
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        LOG(ERR, "curl failed: " + std::string(curl_easy_strerror(result)));
        curl_result = false;
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    if ((http_code - 200) > 99) {
        LOG(ERR, "HTTP request to " + url + " failed with code " + std::to_string(http_code));
        curl_result = false;
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    curl_global_cleanup();

    return curl_result;
}

//

namespace detail
{
template<class... Ts>
struct overloaded : Ts ... {
    using Ts::operator()...;
};

// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
}

//

constexpr const char* TEMPLATE_EVENT =
        R"({{"source": "{}", "state": {}, "timestamp": {}}})";

constexpr const char* TEMPLATE_MEASUREMENT =
        R"({{"source": "{}", "value": {}, "quantity": {}, "timestamp": {}}})";

constexpr const char* TEMPLATE_RECORD =
        R"({{"client_id": "{}", "events": [{}], "measurements": [{}]}})";

std::string serialize(const char* client_id, const std::vector<Record>& records)
{
    std::string records_str;

    std::vector<std::string> events_str;
    std::vector<std::string> measurements_str;

    for (const auto& record : records) {
        std::visit(detail::overloaded{
                [&](const Event& e) {
                    events_str.push_back(fmt::format(
                            TEMPLATE_EVENT,
                            e.src,
                            e.state,
                            e.timestamp / 1000));
                },
                [&](const Measurement& m) {
                    measurements_str.push_back(fmt::format(
                            TEMPLATE_MEASUREMENT,
                            m.src,
                            m.value,
                            (int) m.physical_quantity,
                            m.timestamp / 1000));
                }
        }, record);
    }

    auto result = fmt::format(
            TEMPLATE_RECORD,
            client_id,
            fmt::join(events_str, ","),
            fmt::join(measurements_str, ","));

    // throws error?
    // LOG(TRACE, "serialized: " + result);

    return result;
}
}
