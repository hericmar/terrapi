#include <map>
#include <string_view>

namespace terra
{
struct Section
{
    std::string type;
    std::string name;
};

using SectionBody = std::map<std::string, std::pair<std::string, int>>;  // key, { val, line }

//------------------------------------------------------------------------------

bool is_str_literal(int c);

//------------------------------------------------------------------------------

bool read_switch(Context& ctx, const Section& section, const SectionBody& switch_config);

/// Creates new configuration.
bool parse_config(class Context& ctx, std::string_view str);
}
