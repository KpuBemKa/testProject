#include "Intercom/Intercom.h"

Mode::~Mode()
{
}

void Mode::SetMode(Intercom *intercom)
{
    this->intercom = intercom;
}