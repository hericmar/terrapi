#include <string>
#include <string_view>

namespace terra
{
void log_message(const char* level, const std::string& message);
void log_message(const char* level, const std::string_view& message);
void log_message(const char* level, const char* message);
}
