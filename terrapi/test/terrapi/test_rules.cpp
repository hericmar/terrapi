#include <string>

#include "doctest/doctest.h"

#include "context.h"
#include "expr.h"

using namespace terra;

TEST_CASE("Can parse given expression")
{
    Config config{};

    std::vector<SensorConfig> sensors = {
        SensorConfig{"dht11", 1, "Dummy"},
    };
    config.sensors = sensors;

    Context::create(config);
    {
        std::string str = "dht11.temperature < 22";
        Expr::from(str);
    }
    {
        std::string str = "(10:59 < time) & (time < 21:57)";
        Expr::from(str);
    }
}

/*
#include "app.h"
#include "chrono.h"
#include "config.h"
#include "rule.h"

struct TestSensor : public terra::PhysicalSensor
{
    explicit TestSensor(std::string name) : PhysicalSensor("DHT11")
    {
        m_name = std::move(name);

        m_value[terra::EPhysicalQuantity::Humidity];
        m_value[terra::EPhysicalQuantity::Temperature];
    }

    void set_value(terra::EPhysicalQuantity pq, int value)
    {
        m_value[pq] = (float) value;
    }

    void set_value(terra::EPhysicalQuantity pq, float value)
    {
        m_value[pq] = value;
    }

    void set_value(terra::EPhysicalQuantity pq, bool value)
    {
        m_value[pq] = value;
    }

    void measure(const std::tm& now) override
    {
    }
};

struct TestSignalSensor : public terra::PhysicalSensor
{
    explicit TestSignalSensor(std::string name, int gpio) : PhysicalSensor("WaterLevel")
    {
        m_name = std::move(name);

        m_value[terra::EPhysicalQuantity::Signal];
    }

    void set_value(bool value)
    {
        m_value[terra::EPhysicalQuantity::Signal] = value;
    }

    void measure(const std::tm &now) override
    {

    }
};

//

void test_tokenizer()
{
    {
        terra::Tokenizer t("dht11.humidity < 10");
        const auto result = t.parse();

        TEST_ASSERT(result.size() == 3 + 1)
    }

    {
        terra::Tokenizer t("dht11.humidity < 10 & dht11.temperature < 20");
        const auto result = t.parse();

        TEST_ASSERT(result.size() == 7 + 1)
    }

    {
        terra::Tokenizer t("dht11.humidity < 0 & dht11.humidity < 60 & dht11.temperature < 15 & dht11.temperature < 25");
        const auto result = t.parse();

        TEST_ASSERT(result.size() == 15 + 1)
    }
}

void test_basic_rules()
{
    terra::test::create_test_app();

    auto& c = terra::ctx();

    c.m_sensors.push_back(std::make_unique<TestSignalSensor>("water", 7));
    const auto& test_water = (TestSignalSensor*) c.m_sensors.back().get();

    c.m_sensors.push_back(std::make_unique<TestSensor>("dht11"));
    const auto& test_dht11 = (TestSensor*) c.m_sensors.back().get();

    terra::SectionBody s1_config = {
        {"gpio", {"1", 0}},
        {"rule", {"dht11.humidity < 60 & dht11.temperature < 25 & water.signal = 1", 0}},
    };
    TEST_ASSERT(terra::read_switch(c, {"switch", "humidifier"}, s1_config));

    terra::SectionBody s2_config = {
        {"gpio", {"1", 0}},
        {"rule", {"08:00 < time & time < 22:00", 0}},
    };
    TEST_ASSERT(terra::read_switch(c, {"switch", "lights"}, s2_config));

    TEST_ASSERT(terra::ctx().m_switch_controllers.size() == 2);

    auto& c1 = terra::ctx().m_switch_controllers[0];
    auto& s1 = c1.get_switch();

    auto& c2 = terra::ctx().m_switch_controllers[1];
    auto& s2 = c2.get_switch();

    TEST_ASSERT(!s1.is_on());
    TEST_ASSERT(!s2.is_on());

    {
        // only one rule is valid

        test_dht11->set_value(terra::EPhysicalQuantity::Humidity, 50.0f);
        test_dht11->set_value(terra::EPhysicalQuantity::Temperature, 30.0f);

        c1.update(terra::get_now());
        TEST_ASSERT(!s1.is_on());
    }
    {
        // only one rule is valid

        test_dht11->set_value(terra::EPhysicalQuantity::Humidity, 90.0f);
        test_dht11->set_value(terra::EPhysicalQuantity::Temperature, 20.0f);

        c1.update(terra::get_now());
        TEST_ASSERT(!s1.is_on());
    }
    {
        // both rules are valid

        test_dht11->set_value(terra::EPhysicalQuantity::Humidity, 50.0f);
        test_dht11->set_value(terra::EPhysicalQuantity::Temperature, 20.0f);
        test_water->set_value(true);

        c1.update(terra::get_now());
        TEST_ASSERT(s1.is_on());
    }
    {
        std::tm tm{};

        // inactive time
        TEST_ASSERT(terra::str_to_time("04:00", tm));

        c.m_clock->measure(tm);

        c2.update(tm);
        TEST_ASSERT(!s2.is_on());

        // active time
        TEST_ASSERT(terra::str_to_time("14:00", tm));

        c.m_clock->measure(tm);

        c2.update(tm);
        TEST_ASSERT(s2.is_on());

        // inactive time
        TEST_ASSERT(terra::str_to_time("23:30", tm));

        c.m_clock->measure(tm);

        c2.update(tm);
        TEST_ASSERT(!s2.is_on());
    }
}

//

void test_rules()
{
    test_tokenizer();
    test_basic_rules();
}
 */
