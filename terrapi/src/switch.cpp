#include "switch.h"

namespace terra
{
void Switch::evaluate()
{
    state = rule->evaluate();
}

bool Switch::is_on() const
{
    return state;
}
}
