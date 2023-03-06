#pragma once

#include "expr.h"

namespace terra
{
class Switch
{
public:
    Switch(const Expr& expr) : rule(expr) {}

    void evaluate();

    bool is_on() const;

private:
    Expr rule;
    bool state;
};
}
