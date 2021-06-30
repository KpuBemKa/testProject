#pragma once
#include "Intercom.h"
#include "Access.h"
#include "User.h"
#include <vector>

class NormalMode : public Mode
{
public:
    NormalMode();

    bool CheckKey(uint32_t key, Side side) override;
};