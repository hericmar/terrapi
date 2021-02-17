#pragma once

#include "pch.h"
#include "terra.h"

namespace Terra
{
/// Switch is GPIO pin which is "switched" on with high voltage
/// and "switched" off with low voltage.
class Switch
{
public:
    Switch(unsigned GPIO)
    {
        this->GPIO = GPIO;

        pinMode(GPIO, OUTPUT);
        Off();
    }

    void On()
    {

        if (m_oscilationStep == -1)
        {
            // No oscilation.
            if (!switched) { _SwitchOn(); }
        }
        else
        {
            // Oscilate.
            // Check time.
            auto elapsed = float(std::clock() - m_switchedAt);
            if (elapsed > (float) m_oscilationStep)
            {
                if (switched) { _SwitchOff(); }
                else
                {
                    _SwitchOn();
                }
            }
        }
    }

    void Off()
    {
        if (switched) { _SwitchOff(); }
    }

    ///
    /// \param oscallitionStep in ms.
    void Oscille(unsigned oscillationStep) { m_oscilationStep = (int) oscillationStep; }

    void StopOscillation() { m_oscilationStep = -1; }

    [[nodiscard]] unsigned GetGPIO() const { return GPIO; };

    [[nodiscard]] bool IsSwitched() const { return switched; };

private:
    bool switched = false;
    unsigned GPIO;
    std::clock_t m_switchedAt = 0;

    /// By default switch should not be oscilating.
    /// Oscilation step should be greater than the measure step
    /// (MEASURE_STEP)!
    int m_oscilationStep = -1;

    void _SwitchOn()
    {
        m_switchedAt = std::clock();

        LOG_DEBUG("Switching ON GPIO %d.\n", GPIO);
        digitalWrite(GPIO, GPIO_ON);
        switched = true;
    }

    void _SwitchOff()
    {
        LOG_DEBUG("Switching OFF GPIO %d.\n", GPIO);
        digitalWrite(GPIO, GPIO_OFF);
        switched = false;
    }
};
} // namespace Terra
