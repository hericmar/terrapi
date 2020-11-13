#include "conf_parser.hpp"

#include <stdlib.h>
#include <string.h>
#include <time.h>

using namespace Terra;

void ConfigurationParser::ReadFile(const char *filename)
{
    struct stat buffer;
    if (stat(filename, &buffer) != 0)
    {
        LOG_ERROR("Configuration file '%s' not found!\n", filename)
        return;
    }

    std::ifstream confFile(filename);

    std::string line;

    while (!confFile.eof())
    {
        line = GetLine(confFile);

        if (line == "[sensor]")
        {
            ReadSensor(confFile);
        }
        else if (line == "[switch]")
        {
            ReadSwitch(confFile);
        }
        else if (line == "[timer]")
        {
            ReadTimer(confFile);
        }
    }
}

void ConfigurationParser::ReadSensor(std::ifstream &stream)
{
    int id = -1;
    int type = -1;
    int gpio = -1;
    int physicalQuantity = -1;
    float activeFrom = FLT_MIN, activeTo = FLT_MAX;

    auto line = GetLine(stream);

    while (!line.empty())
    {
        auto splittedLine = SplitString(line, "=");

        if (splittedLine.size() != 2)
            continue;

        if (splittedLine[0] == "id")
        {
            id = std::stoi(splittedLine[1]);
        }
        else if (splittedLine[0] == "type")
        {
            bool found = false;
            int i = 0;
            for (const auto& sensorType : sensors)
            {
                if (sensorType == splittedLine[1])
                {
                    type = i;
                    break;
                }

                i++;
            }
        }
        else if (splittedLine[0] == "wp_gpio")
        {
            gpio = std::stoi(splittedLine[1]);
        }
        else if (splittedLine[0] == "physical_quantity")
        {
            bool found = false;
            int i = 0;
            for (const auto& pq : physicalQuantities)
            {
                if (pq == splittedLine[1])
                {
                    physicalQuantity = i;
                    break;
                }

                i++;
            }
        }
        else if (splittedLine[0] == "active_interval") {
            auto rawInterval = splittedLine[1];
            Sanitize(rawInterval);
            auto interval = SplitString(rawInterval, ",");

            if (interval.size() == 2)
            {
                activeFrom = std::stof(interval[0]);
                activeTo = std::stof(interval[1]);
            }
            else
            {
                LOG_ERROR("Active interval must has two values!");
            }
        }
        else
        {
            LOG_ERROR("Unknown symbol %s.\n", line.c_str());
        }

        line = GetLine(stream);
    }

    if (id != -1 && type != -1 && gpio != -1 && physicalQuantity != -1)
    {
        // Create PhysicalSensor, if does not exists
        printf("Instantiating a sensor id:%d!\n", id);
        std::cout << "\t" << sensors[type] << " connected to GPIO " << gpio << " for measuring " << physicalQuantities[physicalQuantity] <<
                  "\n\t associated switches will be active from " << activeFrom << " to " << activeTo << ".\n";

        // Check if sensor is already parsed.
        auto physSens = App::Get().GetSensorByGPIO(gpio);
        if (physSens == nullptr) // Phys. sensor is not assigned.
        {
            // Get type and create one.
            switch (ESensorType(type))
            {
                case ESensorType::DHT11:
                {
                    physSens = new DHT11(gpio);
                };
                default:
                {
                    break;
                }
            }

            App::Get().m_physicalSensors.push_back(physSens);
        }

        // Now create a Sensor.
        auto sensor = new Sensor(EPhysicalQuantityType(physicalQuantity), physSens, id);
        sensor->SetActiveInterval(activeFrom, activeTo);

        App::Get().m_sensors.push_back(sensor);
    }
    else
    {
        LOG_ERROR("Not enough arguments for a sensor.\n");
    }
}

void ConfigurationParser::ReadSwitch(std::ifstream &stream)
{
    int gpio = -1;
    int sensorId = -1;
    int oscilationStep = -1;

    auto rawLine = GetLine(stream);
    while (!rawLine.empty())
    {
        auto line = SplitString(rawLine, "=");

        if (line.size() != 2)
        {
            LOG_ERROR("Conf. entry must has a 'key = value' pair.\n");
            continue;
        }

        if (line[0] == "wp_gpio")
        {
            gpio = std::stoi(line[1]);
        }
        else if (line[0] == "sensor_id")
        {
            sensorId = std::stoi(line[1]);
        }
        else if (line[0] == "oscillation_step")
        {
            oscilationStep = std::stoi(line[1]);
        }
        else
        {
            LOG_ERROR("Unknown argument %s.\n", line[0].c_str());
        }

        rawLine = GetLine(stream);
    }

    if (gpio != -1 && sensorId != -1)
    {
        printf("Instantiating switch (GPIO %d) for sensor id:%d.\n", gpio, sensorId);

        auto sensor = App::Get().GetSensorById(sensorId);
        if (sensor == nullptr)
        {
            LOG_ERROR("A sensor for switch does not exists!\n");
            return;
        }

        auto aSwitch = new Switch(gpio);
        App::Get().m_switches.push_back(
                aSwitch
        );

        sensor->SetSwitch(aSwitch);
        if (oscilationStep != -1)
        {
            aSwitch->Oscille((unsigned) oscilationStep);
        }
    }
    else
    {
        LOG_ERROR("Not enough arguments for switch!\n");
    }
}

void ConfigurationParser::ReadTimer(std::ifstream& stream)
{
    std::string activeFrom;
    std::string activeTo;
    int id = -1;

    auto rawLine = GetLine(stream);
    while (!rawLine.empty())
    {
        auto line = SplitString(rawLine, "=");

        if (line.size() != 2)
        {
            LOG_ERROR("Conf. entry must has a 'key = value' pair.\n");
            continue;
        }

        if (line[0] == "id")
        {
            id = std::stoi(line[1]);
        }
        else if (line[0] == "active_interval")
        {
            auto rawInterval = line[1];
            Sanitize(rawInterval);
            auto interval = SplitString(rawInterval, ",");

            if (interval.size() == 2)
            {
                activeFrom = interval[0];
                activeTo = interval[1];
            }
            else
            {
                LOG_ERROR("Active interval must has two values!");
            }
        }
        else
        {
            LOG_ERROR("Unknown argument %s.\n", line[0].c_str());
        }

        rawLine = GetLine(stream);
    }

    // Check values.
    if (id != -1 && !activeFrom.empty() && !activeTo.empty())
    {
        struct tm from;
        struct tm to;

        memset(&from, 0, sizeof(struct tm));
        memset(&to, 0, sizeof(struct tm));

        strptime(activeFrom.c_str(), "%H:%M", &from);
        strptime(activeTo.c_str(), "%H:%M", &to);

        // TODO -> rename timer to clock?
        auto globalTimer = App::Get().GetSensorByGPIO(GPIO_TIMER);
        if (globalTimer == nullptr)
        {
            globalTimer = new Timer(GPIO_TIMER);
            App::Get().m_physicalSensors.push_back(globalTimer);
        }

        auto timer = new Sensor(EPhysicalQuantityType::Time, globalTimer, id);

        timer->SetActiveInterval(Timer::TimeToFloat(from), Timer::TimeToFloat(to));
        App::Get().m_sensors.push_back(timer);
    }
}
