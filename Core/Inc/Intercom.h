#pragma once
#include "wiegand.h"
#include "UART_Print.h"
#include "Flags.h"
#include "IntercomMode.h"

class Context;

class Mode
{
protected:
    Context *context;

public:
    virtual ~Mode();

    void setMode(Context *context);

    virtual void KeyReadHandle() = 0;
};

class Context
{
private:
    Mode *mode;

public:
    Context(IntercomMode intercomMode);

    ~Context();

    void TransitionTo(Mode *mode);

    void KeyReadEvent();
};

class NormalMode : public Mode
{
public:
    virtual void KeyReadHandle();
};

class ClosedMode : public Mode
{
    virtual void KeyReadHandle();
};

class OpenMode : public Mode
{
    virtual void KeyReadHandle();
};

class CondOpenMode : public Mode
{
    virtual void KeyReadHandle();
};

void intercom(bool insideKeyRead, bool outsideKeyRead);