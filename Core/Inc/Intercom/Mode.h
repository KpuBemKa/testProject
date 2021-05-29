#pragma once
#include <cstdint>
#include "Context.h"

class Mode
{
protected:
    Context *context;

public:
    virtual ~Mode();

    void SetMode(Context *context);

    virtual bool CheckKey(uint32_t key) = 0;
};