#include "Timer.h"

Timer::Timer()
{
    start();
    setLastTick();
}

Timer::Timer(uint32_t startTime, uint32_t lastTick)
{
    this->startTime = startTime;
    this->lastTick = lastTick;
}

void Timer::start()
{
    this->startTime = HAL_GetTick();
}

uint32_t Timer::getStartTime()
{
    return this->startTime;
}

void Timer::setLastTick()
{
    this->lastTick = HAL_GetTick();
}

uint32_t Timer::getLastTick()
{
    return this->lastTick;
}