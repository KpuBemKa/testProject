#include "Intercom/CondOpenMode.h"

bool CondOpenMode::CheckKey(uint32_t key, Side side, RTC_TimeTypeDef currentTime, RTC_DateTypeDef currentDate)
{
    NormalMode normalMode;

    if(normalMode.CheckKey(key, side, currentTime, currentDate))
    {
        this->intercom->TransitionTo(new OpenMode);

        return true;
    }

    return false;
}