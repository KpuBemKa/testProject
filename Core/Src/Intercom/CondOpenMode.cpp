#include "Intercom/CondOpenMode.h"

bool CondOpenMode::CheckKey(uint32_t key)
{
    this->context->TransitionTo(new OpenMode);

    return true;
}