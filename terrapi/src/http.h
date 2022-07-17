#include <string>
#include <vector>

#include "terrapi/terrapi.h"

namespace terra
{
class HttpClient
{
    struct MeasurementData
    {
        std::string client_id;
        std::string sensor_name;
        std::string value;
        std::string timestamp;
    };

public:
    explicit HttpClient(const std::string& server_url);

    bool put_config(const Context& ctx, const std::string& config_body);
    bool post_measurement(const Context& ctx, const SensorController& controller, const std::tm& now);
    bool post_measurements(const Context& ctx, const std::tm& now);

private:
    void append_data(const Context& ctx, std::vector<MeasurementData>& data, const SensorController& sensorController, const std::tm& now);

    bool post_measurement_ex(const Context& ctx, const std::vector<MeasurementData>& request);

    std::string m_server_url;
};
}
