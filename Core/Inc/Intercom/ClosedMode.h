#pragma once
#include "Mode.h"

class ClosedMode : public Mode
{
    bool CheckKey(uint32_t key) override;
};