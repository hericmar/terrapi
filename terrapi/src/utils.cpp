#include "terrapi/utils.h"

#include <fstream>
#include <sstream>

namespace terra
{
namespace fs
{
    bool exists(const char* path)
    {
        struct stat buffer{};
        if (stat(path, &buffer) != 0) {
            return false;
        }

        return true;
    }

    std::string read(const char* path)
    {
        std::string result;
        std::ifstream f(path);

        if (f.good()) {
            std::stringstream buffer;
            buffer << f.rdbuf();
            result = buffer.str();
        }

        return result;
    }
}
}
