#pragma once
#include "stm32f1xx_hal.h"

enum State
{
    Off,
    On
};

enum RelayType
{
    Constant,
    Impulse,
    ConstantSwitch,
    Valve,
    Null
};

class PinOut
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;

public:
    PinOut(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
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

    void turnOn()
    {
        setState(State::On);
    }

    void turnOff()
    {
        setState(State::Off);
    }

    void setState(State state)
    {
        if (state == On)
        {
            HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
        }
        else if (state == Off)
        {
            HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
        }
    }
};

class ConstantRelay : PinOut
{
public:
    ConstantRelay(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
        : PinOut(GPIOx, GPIO_Pin)
    {
    }
};

class ImpulseRelay : PinOut
{
public:
    ImpulseRelay(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
        : PinOut(GPIOx, GPIO_Pin)
    {
    }
};