#include "PinOut.h"

PinOut::PinOut(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    this->GPIOx = GPIOx;
    this->GPIO_Pin = GPIO_Pin;

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PinOut::turnOn()
{
    setState(State::On);
}

void PinOut::turnOff()
{
    setState(State::Off);
}

void PinOut::setState(State state)
{
    if (state == State::On)
    {
        HAL_GPIO_WritePin(this->GPIOx, this->GPIO_Pin, GPIO_PIN_SET);
    }
    else if (state == State::Off)
    {
        HAL_GPIO_WritePin(this->GPIOx, this->GPIO_Pin, GPIO_PIN_RESET);
    }
}

State PinOut::getState()
{
    if (HAL_GPIO_ReadPin(this->GPIOx, this->GPIO_Pin) == GPIO_PIN_RESET)
    {
        return State::Off;
    }

    return State::On;
}
