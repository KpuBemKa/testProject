#pragma once
#include "stm32f1xx_hal.h"
#include "State.h"

class PinOut
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;

public:
    PinOut(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

    void turnOn();

    void turnOff();

    void setState(State state);

    State getState();
};
