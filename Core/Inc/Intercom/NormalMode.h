#pragma once
#include "Mode.h"

class NormalMode : public Mode
{
public:
    bool CheckKey(uint32_t key) override;
};