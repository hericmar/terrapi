#pragma once

#include "pch.h"
#include "terra.h"

namespace Terra
{
    class Switch
    {
    public:
        Switch(unsigned GPIO)
        {
            this->GPIO = GPIO;

            pinMode(GPIO, OUTPUT);
        }

        void On()
        {
            if (!switched)
            {
                digitalWrite(GPIO, GPIO_ON);
                switched = true;
            }
        }

        void Off()
        {
            if (switched)
            {
                digitalWrite(GPIO, GPIO_OFF);
                switched = false;
            }
        }

        [[nodiscard]] unsigned GetGPIO() const { return GPIO; };

        [[nodiscard]] bool IsSwitched() const { return switched; };

    private:
        bool switched = false;
        unsigned GPIO;
    };
}
