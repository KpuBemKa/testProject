/* #include "UART_Print.h"

UART_Print::UART_Print()
{
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    this->huart1.Instance = USART1;
    this->huart1.Init.BaudRate = 115200;
    this->huart1.Init.WordLength = UART_WORDLENGTH_8B;
    this->huart1.Init.StopBits = UART_STOPBITS_1;
    this->huart1.Init.Parity = UART_PARITY_NONE;
    this->huart1.Init.Mode = UART_MODE_TX_RX;
    this->huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    this->huart1.Init.OverSampling = UART_OVERSAMPLING_16;
}

void UART_Print::printf(const char *fmt, ...)
{
    char buff[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff), fmt, args);
    HAL_UART_Transmit(&this->huart1, (uint8_t *)buff, strlen(buff), HAL_MAX_DELAY);
    va_end(args);
} */