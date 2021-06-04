#pragma once
#include "Intercom.h"
#include "Access.h"

class NormalMode : public Mode
{
public:
    bool CheckKey(uint32_t key, Side side) override;
};