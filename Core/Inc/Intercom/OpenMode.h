#pragma once
#include "Mode.h"

class OpenMode : public Mode
{
   bool CheckKey(uint32_t key) override;
};