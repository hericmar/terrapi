#include "terrapi/utils.h"

#include "chrono.h"

#include <fstream>
#include <sstream>
#include <sys/stat.h>

namespace terra
{
namespace fs
{
    bool exists(const char* path)
    {
        struct stat buffer
        {};
        if (stat(path, &buffer) != 0) { return false; }

        return true;
    }

    std::string read(const char* path)
    {
        std::string result;
        std::ifstream f(path);

        if (f.good())
        {
            std::stringstream buffer;
            buffer << f.rdbuf();
            result = buffer.str();
        }

        return result;
    }
}

namespace log
{
    void print_measurement(const std::tm& tm, EPhysicalQuantity pq, const std::string& sensor_name, float val)
    {
        switch (pq)
        {
        case EPhysicalQuantity::Humidity:
            log::info("{}: {} ({}): {} %", time_to_str(tm), sensor_name, val);
            break;
        case EPhysicalQuantity::Temperature:
            log::info("{}: {} ({}): {} °C", time_to_str(tm), sensor_name, val);
            break;
        default:
            break;
        }
    }
}

namespace string_utils
{
    Lines split(std::string_view str, std::string_view delims)
    {
        std::vector<std::string_view> output;
        size_t first = 0;

        if (str.size() < 2) {
            return output;
        }

        while (first < str.size())
        {
            const auto second = str.find_first_of(delims, first);

            if (first != second) {
                output.emplace_back(str.substr(first, second - first));
            }
            else {
                // push empty lines
                size_t next = first - 1;

                if (output.size() > 2) {
                    if (output.back()[0] != '\n' && output[output.size() - 2][0] == '\n') {
                        // left empty
                    } else {
                        output.emplace_back(str.substr(next, 1));
                    }
                }
            }

            if (second == std::string_view::npos)
                break;

            first = second + 1;
        }

        return output;
    }

    Lines split_first(std::string_view str, std::string_view delims)
    {
        size_t pos = str.find(delims);
        auto s1 = str.substr(0, pos);
        auto s2 = str.substr(pos + 1, std::string::npos);

        return {s1, s2};
    }
}
}
