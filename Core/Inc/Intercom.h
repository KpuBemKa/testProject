#pragma once
#include <cstdint>
#include "UART_Print.h"

class Context;

class Mode
{
protected:
    Context *context;

public:
    virtual ~Mode();

    void SetMode(Context *context);

    virtual bool CheckKey(uint32_t key) = 0;
};

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

class NormalMode : public Mode
{
public:
    bool CheckKey(uint32_t key) override;
};

class ClosedMode : public Mode
{
    bool CheckKey(uint32_t key) override;
};

class OpenMode : public Mode
{
   bool CheckKey(uint32_t key) override;
};

class CondOpenMode : public Mode
{
    bool CheckKey(uint32_t key) override;
};