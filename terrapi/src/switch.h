#pragma once

#include "expr.h"

namespace terra
{
class Switch
{
public:
    Switch(const Expr& expr) : rule(expr) {}

private:
    Expr rule;
};
}
