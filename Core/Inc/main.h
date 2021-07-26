
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

  void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
#define D_01_Pin GPIO_PIN_0
#define D_01_GPIO_Port GPIOC
#define D_01_EXTI_IRQn EXTI0_IRQn
#define D_11_Pin GPIO_PIN_1
#define D_11_GPIO_Port GPIOC
#define D_11_EXTI_IRQn EXTI1_IRQn
#define BT_0_Pin GPIO_PIN_2
#define BT_0_GPIO_Port GPIOC
#define BT_0_EXTI_IRQn EXTI2_IRQn
#define BT_1_Pin GPIO_PIN_3
#define BT_1_GPIO_Port GPIOC
#define BT_1_EXTI_IRQn EXTI3_IRQn
#define Zumer1_Pin GPIO_PIN_10
#define Zumer1_GPIO_Port GPIOB
#define Zumer2_Pin GPIO_PIN_11
#define Zumer2_GPIO_Port GPIOB
#define GreenLed_1_Pin GPIO_PIN_14
#define GreenLed_1_GPIO_Port GPIOB
#define GreenLed_2_Pin GPIO_PIN_15
#define GreenLed_2_GPIO_Port GPIOB
#define D_02_Pin GPIO_PIN_6
#define D_02_GPIO_Port GPIOC
#define D_02_EXTI_IRQn EXTI9_5_IRQn
#define D_12_Pin GPIO_PIN_7
#define D_12_GPIO_Port GPIOC
#define D_12_EXTI_IRQn EXTI9_5_IRQn
#define Door_Pin GPIO_PIN_8
#define Door_GPIO_Port GPIOC
#define Door_EXTI_IRQn EXTI9_5_IRQn
#define Lock_Pin GPIO_PIN_8
#define Lock_GPIO_Port GPIOB
#define Siren_Pin GPIO_PIN_9
#define Siren_GPIO_Port GPIOB

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
