// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#pragma once

#include "expr.h"

namespace terra
{
struct DeviceConfig;

enum SwitchState
{
    SWITCH_OFF = 0,
    SWITCH_ON  = 1,
};

class Switch final
{
public:
    /// @pre \p gpio is empty
    Switch(DeviceConfig* config, const Expr& expr);

    /// called each tick
    void update(uint64_t now_ms);

    ///
    bool is_on() const;

    bool is_on_high() const;

    void switch_on();
    void switch_off();

private:
    /// Write the GPIO pin to high.
    void write_on();

    /// Write the GPIO pin to low.
    void write_off();

    DeviceConfig* config;

    Expr rule;

    SwitchState state = SWITCH_OFF;

    bool oscillate   = false;
    bool is_high     = false;
    uint64_t next_toggle_ms = -1;
};
}
