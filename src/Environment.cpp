#include "Environment.h"

#include "Terra.h"

bool Terra::IsDay()
{
    return App::Get().GetTime();
}
