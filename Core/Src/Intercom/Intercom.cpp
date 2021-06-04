#include "Intercom/Intercom.h"
#include "UART_Print.h"

Intercom::Intercom(Mode *mode, Side side) : mode(nullptr)
{
    this->TransitionTo(mode);
    this->side = side;
}

Intercom::~Intercom()
{
    delete this->mode;
}

void Intercom::TransitionTo(Mode *mode)
{
    if (this->mode == nullptr)
    {
        delete this->mode;
    }

    this->mode = mode;
    this->mode->SetMode(this);
}

bool Intercom::CheckKey(uint32_t key)
{
    return this->mode->CheckKey(key, this->side);
}