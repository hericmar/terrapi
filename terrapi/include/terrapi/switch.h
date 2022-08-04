#pragma once

#include "rule.h"
#include "utils.h"

namespace terra
{
/// Switch is a wrapper around the GPIO pin which is "switched"
/// on with high voltage and "switched" off with low voltage.
class Switch
{
public:
    /// Switch is by default set to off mode.
    ///
    /// \param oscillation_step If you want the switch to oscillate
    ///     you must set oscillation_step as non negative value in ms.
    ///     Oscillation step should be greater than the measure step
    ///     (MEASURE_STEP)!
    Switch(std::string name, int gpio, Expr rules, int oscillation_step = -1);

    bool turn_on();
    bool turn_off();

    void update(int delta_ms);

    [[nodiscard]] const std::string& name() const { return m_name; }

private:
    friend class Controller;

    std::string m_name;
    int m_gpio;

    Expr m_rules;

    bool m_is_on = false;
    bool m_is_upper = false;

    int m_oscillation_step = -1;
    unsigned m_elapsed_ms = 0;
};
}