#pragma once

#include <utility>

#include "terrapi/switch.h"

namespace terra
{
class Controller
{
public:
    explicit Controller(Switch aSwitch) : m_switch(std::move(aSwitch)) {}

    void update(const std::tm& now);

private:
    void evaluate(const std::tm& now);

    Switch m_switch;
};
}
