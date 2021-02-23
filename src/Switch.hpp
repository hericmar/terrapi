#pragma once

#include "Pch.h"

namespace Terra
{
class SensorController;

/// Switch is GPIO pin which is "switched" on with high voltage
/// and "switched" off with low voltage.
class Switch
{
public:
    explicit Switch(int GPIO, SensorController* sensorController, int oscillationStep);

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
    int GPIO;
    std::clock_t m_switchedAt = 0;

    /// By default switch should not be oscilating.
    /// Oscilation step should be greater than the measure step
    /// (MEASURE_STEP)!
    int m_oscilationStep = -1;

    void _SwitchOn();

    void _SwitchOff();
};
} // namespace Terra
