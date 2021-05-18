#include "ConstantRelay.h"

ConstantRelay::ConstantRelay(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
    : PinOut(GPIOx, GPIO_Pin)
{
}