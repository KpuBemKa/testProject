#pragma once
#include "stm32f1xx_hal.h"
#include "State.h"

class Timer
{
    uint32_t startTime;
    uint32_t lastTick;

public:
    Timer();

    Timer(uint32_t startTime, uint32_t lastTick);

    void start();

    uint32_t getStartTime();

    void setLastTick();

    uint32_t getLastTick();
};