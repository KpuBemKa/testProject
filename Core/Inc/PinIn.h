#pragma once
#include "stm32f1xx_hal.h"
#include "State.h"

class PinIn
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;

public:
    PinIn(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
    {
        this->GPIOx = GPIOx;
        this->GPIO_Pin = GPIO_Pin;

        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = GPIO_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
    }

    State getState()
    {
        if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET)
        {
            return State::On;
        }

        return State::Off;
    }
};