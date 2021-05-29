#include "Intercom.h"

Mode::~Mode()
{
}

void Mode::SetMode(Context *context)
{
    this->context = context;
}

Context::Context(Mode *mode) : mode(nullptr)
{
    this->TransitionTo(mode);
}

Context::~Context()
{
    delete this->mode;
}

void Context::TransitionTo(Mode *mode)
{
    if (this->mode == nullptr)
    {
        delete this->mode;
    }

    this->mode = mode;
    this->mode->SetMode(this);
}

bool Context::CheckKey(uint32_t key)
{
    return this->mode->CheckKey(key);
}

bool NormalMode::CheckKey(uint32_t key)
{
    
    uint32_t codes[] =
        {12563593};

    for (unsigned int i = 0; i < sizeof(codes) / sizeof(codes[0]); i++)
    {
        if (key == codes[i])
            return true;
    }

    return false;
}

bool ClosedMode::CheckKey(uint32_t key)
{
    return false;
}

bool OpenMode::CheckKey(uint32_t key)
{
    return true;
}

bool CondOpenMode::CheckKey(uint32_t key)
{
    this->context->TransitionTo(new OpenMode);

    return true;
}