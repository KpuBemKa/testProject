#pragma once
#include <cstdint>
#include "Side.h"

class Intercom;

class Mode
{
protected:
    Intercom *intercom;

public:
    virtual ~Mode();

    void SetMode(Intercom *intercom);

    virtual bool CheckKey(uint32_t key, Side side) = 0;
};

class Intercom
{
private:
    Side side;

protected:
    Mode *mode;

public:
    Intercom(Mode *mode, Side side);

    ~Intercom();

    void TransitionTo(Mode *mode);

    bool CheckKey(uint32_t key);
};