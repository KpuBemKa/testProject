#pragma once

extern void UART_Printf(const char *fmt, ...);

/* #include <stdio.h>
#include <stdarg.h>
#include "string.h"
#include "stm32f1xx_hal.h"

class UART_Print
{
    UART_HandleTypeDef huart1;

public:
    UART_Print();

    void printf(const char *fmt, ...);
}; */