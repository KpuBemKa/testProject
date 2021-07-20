#pragma once
#include "Intercom.h"
#include "Access.h"

class NormalMode : public Mode
{
public:
    NormalMode();

    bool CheckKey(uint32_t key, Side side, RTC_TimeTypeDef currentTime, RTC_DateTypeDef currentDate) override;
};