#include "Intercom/CondOpenMode.h"

bool CondOpenMode::CheckKey(uint32_t key, Side side)
{
    this->intercom->TransitionTo(new OpenMode);
    
    return true;
}