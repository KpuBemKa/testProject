#pragma once
#include "stm32f1xx_hal.h"
#include "PinOut.h"

class InvertedPinOut : public PinOut
{
public:
    InvertedPinOut(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) : PinOut(GPIOx, GPIO_Pin)
    {
    }

    void turnOn()
    {
        setState(State::Off);
    }

    void turnOff()
    {
        setState(State::On);
    }
};