#pragma once

#include "pch.h"
#include "terra.h"
#include "sensor.hpp"
#include "switch.hpp"

#include <cctype>

namespace Terra
{
    /// Split string.
    /// \param s
    /// \param delimiter
    /// \return
    inline std::vector<std::string> SplitString(std::string s, std::string delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
            token = s.substr (pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back (token);
        }
        res.push_back (s.substr (pos_start));

        return res;
    }

    inline std::string Sanitize(std::string str)
    {
        str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

        std::string result;
        for (const char c : str)
        {
            if (c == '#')
                break;

            result += c;
        }

        return result;
    }

    /// Get another line.
    /// \param stream conf file.
    /// \return configuration line without whitespaces.
    inline std::string GetLine(std::ifstream& stream)
    {
        std::string line;
        if (std::getline(stream, line))
        {
            line = Sanitize(line);
        }

        return line;
    }

    /// Configuration file is very simple.
    /// It contains sensor definitions at the top and switches definition
    /// at the bottom of the file.
    class ConfigurationParser
    {
    public:
        ///
        /// \param filename absolute filename location.
        static void ReadFile(const char* filename);

        /// Sensors are parsed first.
        static void ReadSensor(std::ifstream& stream);

        /// Switches are parsed at the end.
        static void ReadSwitch(std::ifstream& stream);

        static void ReadTimer(std::ifstream& stream);
    };
}
