#pragma once

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
    Switch(std::string  name, int gpio, int oscillation_step = -1);

    void turn_on();
    void turn_off();

    const std::string& name() const { return m_name.c_str(); }

private:
    std::string m_name;
    int m_gpio;
    int m_oscillation_step = -1;
    bool m_is_on = false;
};
}