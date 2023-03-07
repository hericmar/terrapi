#include <string>
#include <vector>

namespace terra
{
class BrokerConfig;
class EventData;
class MeasurementData;
class Sensor;

class HttpClient
{
public:
    HttpClient(const BrokerConfig& config);

    bool put_config(const std::string& config_body);
    bool post_records(
        const std::vector<MeasurementData>& measurements, const std::vector<EventData>& events);

private:
    bool make_request(const char* method, const std::string& url, const std::string& body);

    const BrokerConfig& config;
};
}
