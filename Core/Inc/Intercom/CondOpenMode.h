#pragma once
#include "Mode.h"
#include "Intercom/OpenMode.h"

class CondOpenMode : public Mode
{
    bool CheckKey(uint32_t key) override;
};