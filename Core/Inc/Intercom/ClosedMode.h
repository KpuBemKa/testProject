#pragma once
#include "Intercom.h"

class ClosedMode : public Mode
{
public:
    bool CheckKey(uint32_t key, Side side, RTC_TimeTypeDef currentTime, RTC_DateTypeDef currentDate) override;
};