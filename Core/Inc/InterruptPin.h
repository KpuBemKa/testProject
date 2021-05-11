#pragma once
#include "PinIn.h"
#include "stm32f1xx_hal.h"
#include "State.h"

class InterruptPin : public PinIn
{
private:
    bool stateChanged;

public:
    InterruptPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) : PinIn(GPIOx, GPIO_Pin)
    {
    }

    bool isStateChanged()
    {
        if (this->stateChanged == true)
        {
            this->stateChanged = false;
            return true;
        }

        return false;
    }

    void setStateChanged()
    {
        this->stateChanged = true;
    }
};