#include "tests.h"

#include "app.h"
#include "config.h"

struct TestSensor : public terra::PhysicalSensor
{
    terra::EPhysicalQuantity PQ;

    TestSensor(std::string name, terra::EPhysicalQuantity pq)
        : PQ(pq)
    {
        m_name = std::move(name);
    }

    void set_value(int value)
    {
        m_value[PQ].int_val = value;
    }

    void set_value(float value)
    {
        m_value[PQ].float_val = value;
    }

    void set_value(bool value)
    {
        m_value[PQ].float_val = value;
    }

    void measure() override
    {
    }
};

void test_basic_rules()
{
    terra::test::create_test_app();

    auto& c = terra::ctx();

    c.m_sensors.push_back(std::make_unique<TestSensor>("dht11", terra::EPhysicalQuantity::Temperature));
    // c.m_sensors.push_back(std::make_unique<TestSensor>("s2", terra::EPhysicalQuantity::Humidity));

    terra::SectionBody s1_config = {
        {"sensor", {"dht11", 0}},
        {"gpio", {"1", 0}},
        {"rule", {"dht11.humidity < 0 & dht11.humidity < 60 & dht11.temperature < 15 && dht11.temperature < 25", 0}},
    };
    terra::read_switch(c, {"switch", "humidity"}, s1_config);
}

//

void test_rules()
{
    test_basic_rules();
}
