#pragma once
#include <cstdint>
#include "Mode.h"

class Context
{
private:
    Mode *mode;

public:
    Context(Mode *mode);

    ~Context();

    void TransitionTo(Mode *mode);

    bool CheckKey(uint32_t key);

};