#include <string>
#include <string_view>

namespace terra
{
enum LogLevel
{
    TRACE,
    INFO,
    WARN,
    ERR,
    FATAL,
};

void log_message(int level, const std::string& message);
void log_message(int level, const std::string_view& message);
void log_message(int level, const char* message);
}
