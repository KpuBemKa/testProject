#pragma once
#include "stm32f1xx_hal.h"
#include "State.h"

class PinIn
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;

public:
    PinIn(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

    State getState();
};