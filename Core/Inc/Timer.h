#pragma once
#include "stm32f1xx_hal.h"
#include "State.h"

class Timer
{
    uint32_t startTime;
    uint32_t lastTick;

public:
    Timer()
    {
        start();
        setLastTick();
    }

    Timer(uint32_t startTime, uint32_t lastTick)
    {
        this->startTime = startTime;
        this->lastTick = lastTick;
    }

    void start()
    {
        this->startTime = HAL_GetTick();
    }

    uint32_t getStartTime()
    {
        return this->startTime;
    }

    void setLastTick()
    {
        this->lastTick = HAL_GetTick();
    }

    uint32_t getLastTick()
    {
        return this->lastTick;
    }
};