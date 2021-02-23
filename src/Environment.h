#pragma once

namespace Terra
{
/// Environment properties, such as night, day duration.
/// All values are in the relative manner. 12:00 is represented
/// as 12/24 = 0.5f.
struct Environment
{
    float m_dayDuration = 10.0f / 24.0f;
    float m_nightDuration = 14.0f / 24.0f;
    float m_dayFrom = 8.0f / 24.0f;
    float m_dayTo = 18.0f / 24.0f;
};

bool IsDay();
}
