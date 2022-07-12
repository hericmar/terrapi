#ifdef WIN32
// Strange error, due to including curl library!
#include "Windows.h"
#endif

#include <algorithm>

#include "terrapi/utils.h"

#include "http.h"

#include <curl/curl.h>

namespace terra
{
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*) userp)->append((char*)contents, size * nmemb);

    return size * nmemb;
}

HttpClient::HttpClient(const std::string& server_url)
    : m_server_url(std::move(server_url))
{
    if (m_server_url[m_server_url.size() - 1] != '/') {
        m_server_url.push_back('/');
    }
}

bool HttpClient::put_config(const Context& ctx, const std::string& config_body)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    CURL* curl = curl_easy_init();
    std::string read_buffer;

    auto sanitized_config_body = string_utils::replace(config_body, "\"", "\\\"");
    sanitized_config_body = string_utils::replace(sanitized_config_body, "\n", "\\n");

    std::string request_body = fmt::format(R"({{"clientID": "{}","config": "{}"}})",
                                           ctx.m_client_id, sanitized_config_body);

    bool curl_result = true;

    const std::string url = m_server_url + "api/config";

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

        // headers
        struct curl_slist* headers = nullptr;
        // Remove a header curl would otherwise add by itself
        headers = curl_slist_append(headers, "Accept:");

        headers = curl_slist_append(headers, ("Authorization: " + ctx.m_password).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // data
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body.c_str());

        CURLcode result = curl_easy_perform(curl);
        if (result != CURLE_OK) {
            log::err("curl failed: {}", curl_easy_strerror(result));
            curl_result = false;
        }

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if ((http_code - 200) > 99) {
            log::err("HTTP request to \"{}\" failed with code {}.", url, http_code);
            curl_result = false;
        }

        curl_easy_cleanup(curl);

        curl_slist_free_all(headers);
    } else {
        log::err("Cannot initialize CURL, no data will be loaded to the server.");
        curl_result = false;
    }

    curl_global_cleanup();

    return curl_result;
}

bool HttpClient::post_measurements(const Context& ctx)
{
    return false;
}

bool HttpClient::put_config_ex() { return false; }

bool HttpClient::post_measurement_ex(const std::string& sensor_name, const std::string& value, int timestamp)
{
    return false;
}
}
