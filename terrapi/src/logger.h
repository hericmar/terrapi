#include <string>
#include <string_view>

namespace terra
{
const int TRACE = 0;
const int INFO  = 1;
const int WARN  = 2;
const int ERR   = 3;
const int FATAL = 4;

void log_message(int level, const std::string& message);
void log_message(int level, const std::string_view& message);
void log_message(int level, const char* message);
}
