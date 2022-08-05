#include "controller.h"

#include "terrapi/terrapi.h"

namespace terra
{
void Controller::update(const std::tm& now)
{
    evaluate(now);
}

void Controller::evaluate(const std::tm& now)
{
    const bool should_turn_on = m_switch.m_rules->evaluate();

    if (should_turn_on) {
        m_switch.update(UPDATE_STEP);

        if (m_switch.turn_on()) {
            // App::http()->post_measurement(ctx(), *this, tm);
        }
    } else {
        if (m_switch.turn_off()) {
            // App::http()->post_measurement(ctx(), *this, tm);
        }
    }
    // m_switch.turn_on();
}
}
