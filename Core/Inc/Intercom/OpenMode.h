#pragma once
#include "Intercom.h"

class OpenMode : public Mode
{
public:
    bool CheckKey(uint32_t key, Side side) override;
};