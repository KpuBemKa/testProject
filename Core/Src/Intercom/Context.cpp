#include "Intercom/Context.h"

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