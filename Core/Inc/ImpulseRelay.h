#pragma once
#include "PinOut.h"
#include "stm32f1xx_hal.h"
#include "State.h"

class ImpulseRelay : PinOut
{
public:
    ImpulseRelay(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
        : PinOut(GPIOx, GPIO_Pin)
    {
    }
};