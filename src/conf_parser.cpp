#include "conf_parser.h"

#include "sensor.hpp"
#include "time_utils.h"
#include "utils.h"

using namespace Terra;

void ReadEnvironmentProperties(ConfSection&);
void ReadSensor(ConfSection&);
void ReadSwitch(ConfSection&);
void ReadTimer(ConfSection&);

void ReadFileEx(std::ifstream& confFile)
{
    ConfigurationParser::g_currentLine = 1;
    std::string line;

    ConfSection section;
    while (!(section = GetSection(confFile)).Empty())
    {
        auto sectionName = section["section"];
        if (sectionName == "[environment]")
        {
            Log::Info("Reading environment properties");
            ReadEnvironmentProperties(section);
        }
        else if (sectionName == "[sensor]")
        {
            Log::Info("Reading a sensor settings");
            ReadSensor(section);
        }
        else if (sectionName == "[switch]")
        {
            Log::Info("Reading a switch configuration");
            ReadSwitch(section);
        }
        else if (sectionName == "[timer]")
        {
            Log::Info("Reading a timer");
            ReadTimer(section);
        }
    }
}

void ConfigurationParser::ReadFile(const char* filename)
{
    // Check path to the conf. file.
    struct stat buffer{};
    if (stat(filename, &buffer) != 0)
    {
        LOG_ERROR("Configuration file '%s' not found!\n", filename)
        return;
    }
    Log::Info("Reading configuration file '{}'", filename);

    std::ifstream confFile(filename);

    ReadFileEx(confFile);
}

/// Sensors are parsed first.
void ReadSensor(ConfSection& section)
{
    int id               = -1;
    int type             = -1;
    int gpio             = -1;
    int physicalQuantity = -1;
    float activeFrom = FLT_MIN, activeTo = FLT_MAX;

    if (!section.ContainsAll({"id", "type", "wp_gpio", "physical_quantity", "active_interval"}))
    {
        Log::Error("{}: Sensor could not be instantiated, not enough arguments.", ConfigurationParser::g_currentLine);
    }

    id = std::stoi(section["id"]);

    gpio = std::stoi(section["wp_gpio"]);

    type = GetElementIndex(sensors, section["type"].c_str());
    if (type == -1)
    {
        Log::Error("Unknown sensor type – {}.", section["type"]);
        return;
    }

    physicalQuantity = GetElementIndex(physicalQuantities, section["physical_quantity"].c_str());
    if (physicalQuantity == -1)
    {
        Log::Error("Unknown physical quantity – {}", section["physical_quantity"]);
        return;
    }

    {
        auto rawInterval = section["active_interval"];
        Sanitize(rawInterval);
        auto interval = SplitString(rawInterval, ",");

        if (interval.size() == 2)
        {
            activeFrom = std::stof(interval[0]);
            activeTo   = std::stof(interval[1]);
        }
        else
        {
            Log::Error("{}: Active interval must has two values!", ConfigurationParser::g_currentLine);
        }
    }

    // Validate values.
    if (id != -1 && gpio != -1 )
    {
        // Create PhysicalSensor, if does not exists.
        printf("Instantiating a sensor id:%d!\n", id);
        std::cout << "\t" << sensors[type] << " connected to GPIO " << gpio << " for measuring "
                  << physicalQuantities[physicalQuantity] << "\n\t associated switches will be active from "
                  << activeFrom << " to " << activeTo << ".\n";

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

            App::Get().GetPhysSensors().push_back(physSens);
        }

        // Now create a Sensor.
        auto sensor = new Sensor(EPhysicalQuantityType(physicalQuantity), physSens, id);
        sensor->SetActiveInterval(activeFrom, activeTo);

        App::Get().GetSensors().push_back(sensor);
    }
    else
    {
        Log::Error("Not enough arguments for a sensor.");
    }
}

/// Switches are parsed at the end.
void ReadSwitch(ConfSection& section)
{
    int gpio           = -1;
    int sensorId       = -1;
    int oscilationStep = -1;

    if (!section.ContainsAll({"wp_gpio", "sensor_id"}))
    {
        Log::Error("{}: section [switch] is not complete!", ConfigurationParser::g_currentLine);
        return;
    }

    gpio = std::stoi(section["wp_gpio"]);
    sensorId = std::stoi(section["sensor_id"]);

    if (section.Contains("oscillation_step"))
    {
        oscilationStep = std::stoi(section["oscillation_step"]);
    }

    if (gpio != -1 && sensorId != -1)
    {
        printf("Instantiating switch (GPIO %d) for sensor id:%d.\n", gpio, sensorId);

        auto sensor = App::Get().GetSensorById(sensorId);
        if (sensor == nullptr)
        {
            Log::Error("A sensor for switch does not exists!\n");
            return;
        }

        auto aSwitch = new Switch(gpio);
        App::Get().GetSwitches().push_back(aSwitch);

        sensor->SetSwitch(aSwitch);
        if (oscilationStep != -1) { aSwitch->Oscille((unsigned) oscilationStep); }
    }
    else
    {
        Log::Error("Not enough arguments for switch!\n");
    }
}

void ReadTimer(ConfSection& section)
{
    std::string activeFrom;
    std::string activeTo;
    int id = -1;

    if (!section.ContainsAll({"id", "active_interval"}))
    {
        Log::Error("{}: section [timer] does not contain 'id' or 'active_interval' keys.", ConfigurationParser::g_currentLine);
        Log::Error("Skipping this timer.");

        return;
    }

    id = std::stoi(section["id"]);

    auto rawInterval = section["active_interval"];
    Sanitize(rawInterval);
    auto interval = SplitString(rawInterval, ",");

    if (interval.size() == 2)
    {
        activeFrom = interval[0];
        activeTo   = interval[1];
    }
    else
    {
        Log::Error("{}: Active interval must has two values!", ConfigurationParser::g_currentLine);
        return;
    }

    auto from = StringToTime(activeFrom, "%H:%M");
    auto to = StringToTime(activeTo, "%H:%M");

    // TODO -> rename timer to clock?
    // TODO -> GlobalTimer???
    auto globalTimer = App::Get().GetSensorByGPIO(GPIO_TIMER);
    if (globalTimer == nullptr)
    {
        globalTimer = new Timer(GPIO_TIMER);
        App::Get().GetPhysSensors().push_back(globalTimer);
    }

    auto timer = new Sensor(EPhysicalQuantityType::Time, globalTimer, id);

    timer->SetActiveInterval(from, to);
    App::Get().GetSensors().push_back(timer);
}

void ReadEnvironmentProperties(ConfSection& section)
{
    auto& dayTo = App::Get().GetEnvironment().m_dayTo;
    auto& dayFrom = App::Get().GetEnvironment().m_dayFrom;
    auto& dayDuration = App::Get().GetEnvironment().m_dayDuration;
    auto& nightDuration = App::Get().GetEnvironment().m_nightDuration;

    if (!section.ContainsAll({ "day_from", "day_to" }))
    {
        Log::Error("{}: section [environment] does not contain listed properties.", ConfigurationParser::g_currentLine);
        exit(2);
    }

    dayTo = StringToTime(section["day_to"]);
    dayFrom = StringToTime(section["day_from"]);
    dayDuration = dayTo - dayFrom;
    nightDuration = 1.0f - dayDuration;
}
