#ifdef WIN32
// Strange error, due to including curl library!
#include "Windows.h"
#endif

#include <algorithm>

#include "config.h"
#include "http.h"
#include "logger.h"
#include "record.h"

#include <curl/curl.h>

namespace terra
{
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*) userp)->append((char*)contents, size * nmemb);

    return size * nmemb;
}

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

bool HttpClient::put_config(const std::string& config_body)
{
    auto sanitized_config_body = string_replace(config_body, "\"", "\\\"");
    sanitized_config_body = string_replace(sanitized_config_body, "\n", "\\n");

    std::string request_body =
        "{"
        "  \"client_id\": \"" + config.client_id + "\","
        "  \"config\": \"" + sanitized_config_body + "\""
        "}";

    return make_request("PUT", config.address + "api/v1/config", request_body);
}

bool HttpClient::post_records(
    const std::vector<MeasurementData>& measurements, const std::vector<EventData>& events)
{
    std::string request_body =
        "{"
        "  \"client_id\": \"" + config.client_id + "\", "
        "  \"measurements\": [";

    bool is_first = true;
    for (const auto& measurement : measurements) {
        if (!is_first) {
            request_body += ",";
        }

        request_body +=
            "{"
            "  \"sensor_name\": \"" + std::string(measurement.sensor_name) + "\", " +
            "  \"value\": " + std::to_string(measurement.value) + ", " +
            "  \"physical_quantity\": " + std::to_string(measurement.physical_quantity) + ", " +
            "  \"timestamp\": " + std::to_string(measurement.timestamp) +
            "}";

        is_first = false;
    }

    request_body +=
        "  ],"
        "  \"events\": [";

    is_first = true;
    for (const auto& event : events) {
        if (!is_first) {
            request_body += ",";
        }

        const auto state = event.state ? std::string("true") : std::string("false");

        request_body +=
            "{"
            "  \"switch_name\": \"" + std::string(event.switch_name) + "\", "
            "  \"state\": " + state + ", "
            "  \"timestamp\": " + std::to_string(event.timestamp) +
            "}";

        is_first = false;
    }

    request_body +=
        "  ]"
        "}";

    return make_request("POST", config.address + "api/v1/record", request_body);
}

bool HttpClient::make_request(const char* method, const std::string& url, const std::string& body)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    CURL* curl = curl_easy_init();
    std::string read_buffer;

    bool curl_result = true;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

        // headers
        struct curl_slist* headers = nullptr;
        // Remove a header curl would otherwise add by itself
        headers = curl_slist_append(headers, "Accept:");

        headers = curl_slist_append(headers, ("Authorization: Basic " + config.password).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // data
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

        CURLcode result = curl_easy_perform(curl);
        if (result != CURLE_OK) {
            log_message(ERR, "curl failed: " + std::string(curl_easy_strerror(result)));
            curl_result = false;
        }

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if ((http_code - 200) > 99) {
            log_message(ERR, "HTTP request to " + url + " failed with code " + std::to_string(http_code));
            curl_result = false;
        }

        curl_easy_cleanup(curl);

        curl_slist_free_all(headers);
    } else {
        log_message(ERR, "cannot initialize CURL, no data will be loaded to the server");
        curl_result = false;
    }

    curl_global_cleanup();

    return curl_result;
}
}
