#pragma once
#include "PinOut.h"
#include "stm32f1xx_hal.h"
#include "State.h"

class ConstantRelay : PinOut
{
public:
    ConstantRelay(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
};