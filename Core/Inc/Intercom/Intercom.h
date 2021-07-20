#pragma once
#include <cstdint>
#include "Side.h"
#include "User.h"
#include "stm32f1xx_hal.h"

class Intercom;

class Mode
{
protected:
    Intercom *intercom;

public:
    virtual ~Mode();

    void SetMode(Intercom *intercom);

    virtual bool CheckKey(uint32_t key, Side side, RTC_TimeTypeDef currentTime, RTC_DateTypeDef currentDate) = 0;
};

class Intercom
{
private:
    Side side;
    RTC_HandleTypeDef *hrtc;

protected:
    Mode *mode;

public:
    Intercom(Mode *mode, Side side, RTC_HandleTypeDef *hrtc);

    ~Intercom();

    void TransitionTo(Mode *mode);

    bool CheckKey(uint32_t key);
};