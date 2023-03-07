#include <stdlib.h>

namespace terra
{
struct MeasurementData
{
    const char* sensor_name;
    float       value;
    /// value of ValueType enum
    unsigned    physical_quantity;
    uint64_t    timestamp;
};

struct EventData
{
    const char* switch_name;
    bool        state;
    uint64_t    timestamp;
};
}
