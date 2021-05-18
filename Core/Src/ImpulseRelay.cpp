#include "ImpulseRelay.h"

ImpulseRelay::ImpulseRelay(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
    : PinOut(GPIOx, GPIO_Pin)
{
}