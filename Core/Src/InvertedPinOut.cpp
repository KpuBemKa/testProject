#include "InvertedPinOut.h"

InvertedPinOut::InvertedPinOut(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) : PinOut(GPIOx, GPIO_Pin)
{
}

void InvertedPinOut::turnOn()
{
    setState(State::Off);
}

void InvertedPinOut::turnOff()
{
    setState(State::On);
}