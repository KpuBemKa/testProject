#include "Intercom/Intercom.h"

Intercom::Intercom(Mode *mode, Side side, RTC_HandleTypeDef *hrtc) : mode(nullptr)
{
    this->TransitionTo(mode);
    this->side = side;
    this->hrtc = hrtc;
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
    RTC_TimeTypeDef currentTime;
    RTC_DateTypeDef currentDate;

    HAL_RTC_GetTime(hrtc, &currentTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(hrtc, &currentDate, RTC_FORMAT_BIN);    
    
    return this->mode->CheckKey(key, this->side, currentTime, currentDate);
}

Mode::~Mode()
{
}

void Mode::SetMode(Intercom *intercom)
{
    this->intercom = intercom;
}