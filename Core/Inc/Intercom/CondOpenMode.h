#pragma once
#include "Intercom.h"
#include "Intercom/OpenMode.h"
#include "Intercom/NormalMode.h"

class CondOpenMode : public Mode
{
public:
    bool CheckKey(uint32_t key, Side side, RTC_TimeTypeDef currentTime, RTC_DateTypeDef currentDate) override;
};