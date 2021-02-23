#include "ConfParser.h"

#include <optional>

#include "Sensor.hpp"
#include "Terra.h"
#include "TimeUtils.h"
#include "Utils.h"

using namespace Terra;

void ReadEnvironmentProperties(ConfSection&);
void ReadSensor(ConfSection&);
void ReadSwitch(ConfSection&);
void ReadTimer(ConfSection&);

static constexpr int g_lowestWPGPIO = 0;
static constexpr int g_highestWPGPIO = 16;

std::optional<ActiveInterval> GetIntervalFromString(const std::string& rawInterval)
{
    auto interval = SplitString(rawInterval, ",");

    if (rawInterval == "none")
    {
        return {};
    }

    if (interval.size() != 2)
    {
        Log::Error("{}: Active interval must has two values!", ConfigurationParser::g_currentLine);
        return {};
    }

    return ActiveInterval{std::stof(interval[0]), std::stof(interval[1])};
}

std::optional<int> GetInt(const std::string& str)
{
    try
    {
        auto result = std::stoi(str);
        return result;
    }
    catch (std::invalid_argument& e)
    {
        return {};
    }
}

std::optional<int> GetInt(const std::string& str, int from, int to)
{
    auto result = std::stoi(str);
    if (from <= result && result <= to)
        return result;
    return {};
}

template <std::size_t Size>
std::optional<int> GetInt(const std::string& str, const std::array<int, Size>& validValues)
{
    auto result = std::stoi(str);
    for (const int i : validValues)
        if (result == i)
            return result;

    return {};
}

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
    ActiveInterval activeInterval;
    ActiveInterval activeNightInterval;

    if (!section.ContainsAll({"id", "type", "wp_gpio", "physical_quantity", "active_interval"}))
    {
        Log::Error("{}: Sensor cannot not be instantiated, not enough arguments.", ConfigurationParser::g_currentLine);
    }

    if (auto parsedID = GetInt(section["id"]))
        id = parsedID.value();
    else
    {
        Log::Error("Sensor ID must have numeric value!");
    }

    if (auto parsedGPIO = GetInt(section["wp_gpio"], g_lowestWPGPIO, g_highestWPGPIO))
        gpio = parsedGPIO.value();
    else
    {
        Log::Error("Wrong WiringPI GPIO number!");
        return;
    }

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

    if (auto interval = GetIntervalFromString(section["active_interval"]))
        activeInterval = interval.value();
    else
        Log::Warning("No active value interval specified.");

    if (auto nightInterval = GetIntervalFromString(section["night_active_interval"]))
        activeNightInterval = nightInterval.value();

    // Create PhysicalSensor, if does not exists.
    Log::Info("Instantiating a sensor controller id: {}!", id);
    Log::Info("\t{} connected to GPIO {} for measuring {}", sensors[type], gpio, physicalQuantities[physicalQuantity]);
    Log::Info("\t associated switches will be active from {} to {}", activeInterval.m_from * 24.0f, activeInterval.m_to * 24.0f);

    // Check if sensor is already parsed.
    auto physSens = App::Get().GetSensorByGPIO(gpio);
    if (physSens == nullptr) // Phys. sensor is not assigned.
        physSens = App::CreateSensor(ESensorType(type), gpio);

    // Now create a Sensor controller.
    App::CreateSensorController(id, EPhysicalQuantityType(physicalQuantity), physSens)
        ->SetActiveInterval(activeInterval)
        ->SetActiveNightInterval(activeNightInterval);
}

/// Switches are parsed at the end.
void ReadSwitch(ConfSection& section)
{
    int gpio           = -1;
    int sensorId       = -1;
    int oscillationStep = -1;
    SensorController* sensor;

    if (!section.ContainsAll({"wp_gpio", "sensor_id"}))
    {
        Log::Error("{}: section [switch] is not complete!", ConfigurationParser::g_currentLine);
        return;
    }

    if (auto parsedGPIO = GetInt(section["wp_gpio"], g_lowestWPGPIO, g_highestWPGPIO))
        gpio = parsedGPIO.value();
    else
    {
        Log::Error("Invalid GPIO number!");
        return;
    }

    if (section.Contains("oscillation_step"))
    {
        if (auto parsedOscillationStep = GetInt(section["oscillation_step"]))
            oscillationStep = parsedOscillationStep.value();
        else
        {
            Log::Error("Oscillation step must be integer value!");
        }
    }

    if (auto parsedSensorId = GetInt(section["sensor_id"]))
    {
        sensorId = parsedSensorId.value();
        sensor = App::Get().GetSensorById(sensorId);
        if (sensor == nullptr)
        {
            Log::Error("A sensor for switch does not exists!\n");
            return;
        }
    }
    else
    {
        Log::Error("Sensor ID must be integer value!");
        return;
    }

    Log::Info("Instantiating switch (GPIO {}) for sensor id: {}.", gpio, sensorId);

    App::CreateSwitch(gpio, sensor, oscillationStep);
}

void ReadTimer(ConfSection& section)
{
    int id = -1;
    ActiveInterval activeInterval;

    if (!section.ContainsAll({"id", "active_interval"}))
    {
        Log::Error("{}: section [timer] does not contain 'id' or 'active_interval' keys.", ConfigurationParser::g_currentLine);
        Log::Error("Skipping this timer.");
        return;
    }

    if (auto parsedId = GetInt(section["id"]))
        id = parsedId.value();
    else
    {
        Log::Error("Timer id is not numeric value!");
        return;
    }

    if (auto interval = GetIntervalFromString(section["active_interval"]))
        activeInterval = interval.value();

    App::CreateTimer(id, activeInterval);
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
