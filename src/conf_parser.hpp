#pragma once

#include "pch.h"
#include "terra.h"
#include "sensor.hpp"
#include "switch.hpp"

namespace Terra
{
    /*
     * Configuration file is very simple.
     * It contains sensor definitions at the top and switches definition
     * at the bottom of the file.
     */
    class ConfigurationParser
    {
    public:
        ///
        /// \param filename absolute filename location.
        inline static void ReadFile(const char* filename)
        {
            struct stat buffer;
            if (stat (filename, &buffer) != 0)
            {
                LOG_ERROR("Configuration file '%s' not found!\n", filename)
            }

            std::ifstream confFile(filename);

            std::string line;
            while (std::getline(confFile, line))
            {
                printf("%s\n", line.c_str());
            }
        }

        /*
         * Sensors are parsed first.
         */
        inline static void ReadSensor()
        {

        }

        /*
         * Switches are parsed at the end.
         */
        inline static void ReadSwitch()
        {

        }
    };
}
