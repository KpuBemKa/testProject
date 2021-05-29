#include "Intercom/Mode.h"

Mode::~Mode()
{
}

void Mode::SetMode(Context *context)
{
    this->context = context;
}