#pragma once

#include "Pch.h"

#include <cctype>
#include <map>
#include <utility>

#include "Log.h"

namespace Terra
{
class ConfSection
{
    std::map<std::string, std::string> m_data;

public:
    const std::string& operator[](const std::string&& keyVal)
    {
        return m_data[keyVal];
    }

    void Insert(std::pair<std::string, std::string>&& keyVal)
    {
        m_data.insert(keyVal);
    }

    bool Contains(const std::string& key)
    {
        return m_data.count(key) != 0;
    }

    bool ContainsAll(const std::vector<std::string>& keys)
    {
        bool result = true;

        for (const auto& key : keys)
        {
            if (!Contains(key))
            {
                Log::Error("Expected record '{}'.", key);
                result = false;
            }
        }

        return result;
    }

    [[nodiscard]] bool Empty() const noexcept
    {
        return m_data.empty();
    }
};

inline std::string Sanitize(std::string str)
{
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

    std::string result;
    for (const char c : str)
    {
        if (c == '#') break;

        result += c;
    }

    return result;
}

/// Split string.
/// \param s
/// \param delimiter
/// \return
inline std::vector<std::string> SplitString(const std::string& s, const std::string& delimiter)
{
    Sanitize(s);

    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
    {
        token     = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }
    res.push_back(s.substr(pos_start));

    return res;
}

/// Configuration file is very simple.
/// It contains sensor definitions at the top and switches definition
/// at the bottom of the file.
namespace ConfigurationParser
{
static int g_currentLine = -1;
static const std::array<std::string, 4> g_sectionTags = {
    "[environment]",
    "[sensor]",
    "[switch]",
    "[timer]"
};

/// \param filename absolute filename location.
void ReadFile(const char* filename);
}


inline bool HasBeginOfSection(const std::string& sanitizedLine)
{
    for (const auto& section : ConfigurationParser::g_sectionTags)
    {
        if (section == sanitizedLine) return true;
    }

    return false;
}

inline ConfSection GetSection(std::ifstream& stream)
{
    ConfSection result;

    std::string line;

    // If we are at the end of the stream, return empty result.
    if (stream.eof())
        return result;

    // Try to get title.
    if (std::getline(stream, line))
    {
        line = Sanitize(line);
    }

    if (!HasBeginOfSection(line))
    {
        Log::Error("{}: Expected begin of section tag (such as [sensor], ...)", ConfigurationParser::g_currentLine);
        return result;
    }

    result.Insert(std::make_pair("section", line));

    while (!stream.eof())
    {
        auto pos = stream.tellg();

        if (std::getline(stream, line))
        {
            line = Sanitize(line);
            ConfigurationParser::g_currentLine++;

            if (HasBeginOfSection(line))
            {
                // Rollback to the beginning of a line.
                stream.seekg(pos);
                break;
            }
            else
            {
                if (!line.empty() && line[0] != '#')
                {
                    auto parsedLine = SplitString(line, "=");

                    if (parsedLine.size() != 2)
                    {
                        Log::Error("{}: Expected key–value pair, got '{}'.", ConfigurationParser::g_currentLine, line);
                    }
                    else
                    {
                        result.Insert(std::make_pair(parsedLine[0], parsedLine[1]));
                    }
                }
            }
        }
    }

    return result;
}
} // namespace Terra
