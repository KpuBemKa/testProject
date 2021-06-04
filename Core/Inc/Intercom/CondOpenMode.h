#pragma once
#include "Intercom.h"
#include "Intercom/OpenMode.h"

class CondOpenMode : public Mode
{
public:
    bool CheckKey(uint32_t key, Side side) override;
};