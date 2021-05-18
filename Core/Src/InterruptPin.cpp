#include "InterruptPin.h"

InterruptPin::InterruptPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) : PinIn(GPIOx, GPIO_Pin)
{
}

bool InterruptPin::isStateChanged()
{
    if (this->stateChanged == true)
    {
        this->stateChanged = false;
        return true;
    }

    return false;
}

void InterruptPin::setStateChanged()
{
    this->stateChanged = true;
}