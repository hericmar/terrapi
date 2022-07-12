#include <string>

#include "terrapi/terrapi.h"

namespace terra
{
class HttpClient
{
public:
    explicit HttpClient(const std::string& server_url);

    bool put_config(const Context& ctx, const std::string& config_body);
    bool post_measurements(const Context& ctx);

private:
    bool put_config_ex();
    bool post_measurement_ex(const std::string& sensor_name, const std::string& value, int timestamp);

    std::string m_server_url;
};
}
