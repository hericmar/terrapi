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
            if (!swithed)
            {
                digitalWrite(GPIO, GPIO_ON);
                swithed = true;
            }
        }

        void Off()
        {
            if (swithed)
            {
                digitalWrite(GPIO, GPIO_OFF);
                swithed = false;
            }
        }

    private:
        bool swithed = false;
        unsigned GPIO;
    };
}
