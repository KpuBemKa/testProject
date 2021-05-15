/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include <stdio.h>
#include "wiegand.h"
#include <stdarg.h>
#include "PinIn.h"
#include "PinOut.h"
#include "ConstantRelay.h"
#include "ImpulseRelay.h"
#include "InterruptPin.h"
#include "Timer.h"
#include "WorkMode.h"
#include "IntercomMode.h"
#include "InvertedPinOut.h"

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
volatile uint8_t wig_flag_inrt = 1;

IntercomMode intercomMode = IntercomMode::NormalMode;
WorkMode workMode = WorkMode::NoMode, previousWorkMode = WorkMode::NoMode;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

/*----> Functions Protoypes <----*/

/**
 * @brief Event to track time between modes
*/
void TimeTrackEvent();
/**
 * @brief Event to change work mode every 15 seconds
*/
void ChangeModeEvent();
/**
 * @brief Event to listen for door button to avoid rattling contacts
*/
void DoorSensorEvent();
/**
 * @brief Event to listen for intercoms
*/
void KeyReadEvent();
/**
 * @brief Event to listen for pressed buttons
*/
void ButtonPressedEvent();
/**
 * @brief Event to listen for switching to temporarily opened mode, giving access to enter
*/
void TempOpenModeEvent();
/**
 * @brief Event to handle temporarily alarm, activated when exceeded time to enter
*/
void TempAlarmEvent();
/**
 * @brief Event to handle deny work mode, activated when user is not permitted to enter
*/
void DenyKeyEvent();
/**
 * @brief Event to handle alarm work mode, activated when something is wrong, like unauthorized door opening
*/
void AlarmEvent();

/**
 * @brief Check if given code is allowed to pass
 * @retval bool
*/
bool verifyCode(uint32_t code);
/**
 * @brief printf
 * @retval None
*/
void UART_Printf(const char *fmt, ...);
/**
 * @brief Switch mode depending on the current one
 * @retval None
*/
void switchMode();

/*----------> Objects <----------*/

/**
 * @brief Door sensor
*/
InterruptPin door(Door_GPIO_Port, Door_Pin);

/**
 * @brief Door relay
*/
PinOut lock(Lock_GPIO_Port, Lock_Pin);
/**
 * @brief Alarm relay
*/
PinOut siren(Siren_GPIO_Port, Siren_Pin);

/**
 * @brief Inside intercom zumer (#1, right side)
*/
InvertedPinOut insideZumer(Zumer1_GPIO_Port, Zumer1_Pin);
/**
 * @brief Outside intercom zumer (#2, left side)
*/
InvertedPinOut outsideZumer(Zumer2_GPIO_Port, Zumer2_Pin);
/**
 * @brief Inside intercom green led (#1, right side)
*/
InvertedPinOut insideGreenLed(GreenLed_1_GPIO_Port, GreenLed_1_Pin);
/**
 * @brief Outside intercom green led (#2, left side)
*/
InvertedPinOut outsideGreenLed(GreenLed_2_GPIO_Port, GreenLed_2_Pin);

Timer doorSensorTimer(0, 0);
/*----------> Flags <----------*/

bool
    insideButtonPressed = false,
    outsideButtonPressed = false,
    insideKeyRead = false,
    outsideKeyRead = false,
    doorSensorFirstTime = true;

uint32_t
    timmeTrack = 0,
    timme = 0;

/*-------> Init Functions <-------*/

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pins : D_01_Pin D_11_Pin D_02_Pin D_12_Pin */
  GPIO_InitStruct.Pin = D_01_Pin | D_11_Pin | D_02_Pin | D_12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : BT_0_Pin BT_1_Pin */
  GPIO_InitStruct.Pin = BT_0_Pin | BT_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Zumer1_Pin Zumer2_Pin GreenLed_1_Pin GreenLed_2_Pin */
  /*  GPIO_InitStruct.Pin = Zumer1_Pin | Zumer2_Pin | GreenLed_1_Pin | GreenLed_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); */

  /*Configure GPIO pin : Door_Pin */
  GPIO_InitStruct.Pin = Door_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Door_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

int main(void)
{
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART1_UART_Init();

  UART_Printf("\nSwitched to Normal mode.\r\n");
  insideGreenLed.turnOff();
  outsideGreenLed.turnOff();
  insideZumer.turnOff();
  outsideZumer.turnOff();

  while (1)
  {
    TimeTrackEvent();

    ChangeModeEvent();

    DoorSensorEvent();

    KeyReadEvent();

    ButtonPressedEvent();

    TempOpenModeEvent();

    TempAlarmEvent();

    DenyKeyEvent();

    AlarmEvent();
  }
}

/* Events */

void TimeTrackEvent()
{
  if ((HAL_GetTick() - timmeTrack) > 5000)
  {
    UART_Printf("Time passed: %d%s\r\n", (HAL_GetTick() - timme) / 1000, " sec.");
    timmeTrack = HAL_GetTick();
  }
}

void ChangeModeEvent()
{
  if ((HAL_GetTick() - timme) > 15000)
  {
    timme = HAL_GetTick();
    switchMode();
  }
}

void DoorSensorEvent()
{
  if (HAL_GetTick() - doorSensorTimer.getStartTime() > 100 && doorSensorFirstTime == false)
  {
    door.setStateChanged();

    if (door.getState() == State::Off)
    {
      UART_Printf("Door was closed.\r\n");
    }
    else if (door.getState() == State::On)
    {
      UART_Printf("Door was opened.\r\n");
    }

    if (workMode != WorkMode::TempOpenMode && door.getState() == State::On)
    {
      workMode = WorkMode::AlarmMode;
    }

    doorSensorFirstTime = true;
  }
}

void KeyReadEvent()
{
  if (wig_available() && workMode == WorkMode::NoMode)
  {
    if (intercomMode != IntercomMode::ClosedMode && verifyCode(getCode()))
    {
      UART_Printf("%s intercom was used. Key id: %d.\r\n", insideKeyRead ? "Inside" : "Outside", getCode());

      workMode = WorkMode::TempOpenMode;

      if (intercomMode == IntercomMode::CondOpenMode)
      {
        UART_Printf("Current mode is Conditionally Open. Switching to Open.\r\n");
        intercomMode = IntercomMode::OpenMode;
      }
    }
    else
    {
      (intercomMode == IntercomMode::ClosedMode) ? UART_Printf("Acces denied, door is closed. Key id: %d.\r\n", getCode())
                                                 : UART_Printf("Acces denied. Key is not authorized to enter. Key id: %d.\r\n", getCode());

      previousWorkMode = workMode;
      workMode = WorkMode::DenyMode;
    }

    insideKeyRead = false;
    outsideKeyRead = false;
  }
}

void ButtonPressedEvent()
{
  if (insideButtonPressed || outsideButtonPressed)
  {
    UART_Printf("%s button was used.\r\n", insideButtonPressed ? "Inside" : "Outside");

    workMode = WorkMode::TempOpenMode;

    insideButtonPressed = false;
    outsideButtonPressed = false;
  }
}

void TempOpenModeEvent()
{
  static Timer lockTimer(0, 0);
  static Timer zumerTimer(0, 0);

  if (workMode == WorkMode::TempOpenMode)
  {
    static bool firstTime = true;
    if (firstTime)
    {
      firstTime = false;
      lock.turnOn();
      lockTimer.start();
    }

    if (HAL_GetTick() - lockTimer.getStartTime() < 5000 && !(door.isStateChanged() && door.getState() == State::Off))
    {
      if (HAL_GetTick() - zumerTimer.getLastTick() > 250)
      {
        zumerTimer.setLastTick();
        insideZumer.turnOn();
        outsideZumer.turnOn();
        insideGreenLed.turnOn();
        outsideGreenLed.turnOn();
      }

      if (HAL_GetTick() - zumerTimer.getLastTick() > 75)
      {
        insideZumer.turnOff();
        outsideZumer.turnOff();
        insideGreenLed.turnOff();
        outsideGreenLed.turnOff();
      }
    }
    else if (door.getState() == State::On)
    {
      UART_Printf("Door is still opened. Turning on temporarily alarm.\r\n");
      workMode = WorkMode::TempAlarmMode;
    }
    else
    {
      if (HAL_GetTick() - lockTimer.getStartTime() >= 5000)
      {
        UART_Printf("Door was not opened. Closing the relay.\r\n");
      }

      workMode = WorkMode::NoMode;
      firstTime = true;
      lock.turnOff();
      insideZumer.turnOff();
      outsideZumer.turnOff();
      insideGreenLed.turnOff();
      outsideGreenLed.turnOff();
    }
  }
}

void TempAlarmEvent()
{
  static Timer sirenTimer(0, 0);

  if (workMode == WorkMode::TempAlarmMode)
  {
    if (HAL_GetTick() - sirenTimer.getLastTick() > 250)
    {
      sirenTimer.setLastTick();
      siren.turnOn();
    }
    else if (HAL_GetTick() - sirenTimer.getLastTick() < 250 && HAL_GetTick() - sirenTimer.getLastTick() > 150)
    {
      siren.turnOff();
    }

    if (door.isStateChanged() && door.getState() == State::Off)
    {
      UART_Printf("Door was closed.\r\n");
      workMode = WorkMode::NoMode;
      siren.turnOff();
    }
  }
}

void DenyKeyEvent()
{
  static Timer denyTimer(0, 0);
  static bool firstTime = true;

  if (workMode == WorkMode::DenyMode)
  {
    if (firstTime)
    {
      denyTimer.start();
      firstTime = false;
    }

    if (HAL_GetTick() - denyTimer.getStartTime() < 300)
    {
      if (HAL_GetTick() - denyTimer.getLastTick() > 150)
      {
        denyTimer.setLastTick();
        insideZumer.turnOn();
        outsideZumer.turnOn();
      }
      else if (HAL_GetTick() - denyTimer.getLastTick() < 150 && HAL_GetTick() - denyTimer.getLastTick() > 75)
      {
        insideZumer.turnOff();
        outsideZumer.turnOff();
      }
    }
    else
    {
      workMode = previousWorkMode;
      insideZumer.turnOff();
      outsideZumer.turnOff();
      firstTime = true;
    }
  }
}

void AlarmEvent()
{
  if (workMode == WorkMode::AlarmMode)
  {
    static Timer alarmTimer(0, 0);
    static bool firstTime = true;

    if (firstTime)
    {
      UART_Printf("Unauthorized access. Triggered alarm.\r\n");
      alarmTimer.start();
      firstTime = false;
    }

    if (HAL_GetTick() - alarmTimer.getLastTick() > 1000)
    {
      alarmTimer.setLastTick();
      siren.turnOn();
    }
    else if (HAL_GetTick() - alarmTimer.getLastTick() < 1000 && HAL_GetTick() - alarmTimer.getLastTick() > 750)
    {
      siren.turnOff();
    }

    if (HAL_GetTick() - alarmTimer.getStartTime() > 2000 && ((wig_available() && verifyCode(getCode())) /* || cancelled from server */))
    {
      workMode = WorkMode::TempOpenMode;
      siren.turnOff();
      firstTime = true;
    }
  }
}

/* Aux functions */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
  case BT_0_Pin:
  {
    insideButtonPressed = true;
    break;
  }
  case BT_1_Pin:
  {
    outsideButtonPressed = true;
    break;
  }
  case Door_Pin:
  {
    if (doorSensorFirstTime)
    {
      doorSensorTimer.start();
      doorSensorFirstTime = false;
    }
    break;
  }
  case D_01_Pin:
  {
    if (wig_flag_inrt)
    {
      ReadD0();
      insideKeyRead = true;
    }
    break;
  }
  case D_11_Pin:
  {
    if (wig_flag_inrt)
    {
      ReadD1();
      insideKeyRead = true;
    }
    break;
  }
  case D_02_Pin:
  {
    if (wig_flag_inrt)
    {
      ReadD0();
      outsideKeyRead = true;
    }
    break;
  }
  case D_12_Pin:
  {
    if (wig_flag_inrt)
    {
      ReadD1();
      outsideKeyRead = true;
    }
    break;
  }
  default:
  {
    break;
  }
  }
}

void UART_Printf(const char *fmt, ...)
{
  char buff[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buff, sizeof(buff), fmt, args);
  HAL_UART_Transmit(&huart1, (uint8_t *)buff, strlen(buff), HAL_MAX_DELAY);
  va_end(args);
}

bool verifyCode(uint32_t code)
{
  uint32_t codes[] =
      {12563593};

  for (unsigned int i = 0; i < sizeof(codes) / sizeof(codes[0]); i++)
  {
    if (code == codes[i])
      return true;
  }

  return false;
}

void switchMode()
{
  switch (intercomMode)
  {
  case IntercomMode::NormalMode:
  {
    UART_Printf("\nSwitched to Closed mode.\r\n");
    intercomMode = IntercomMode::ClosedMode;
    lock.turnOff();
    break;
  }
  case IntercomMode::ClosedMode:
  {
    UART_Printf("\nSwitched to Open mode.\r\n");
    intercomMode = IntercomMode::OpenMode;
    lock.turnOn();
    break;
  }
  case IntercomMode::OpenMode:
  {
    UART_Printf("\nSwitched to CondOpen mode.\r\n");
    intercomMode = IntercomMode::CondOpenMode;
    lock.turnOff();
    break;
  }
  case IntercomMode::CondOpenMode:
  {
    UART_Printf("\nSwitched to Normal mode.\r\n");
    intercomMode = IntercomMode::NormalMode;
    lock.turnOff();
    break;
  }
  default:
  {
    UART_Printf("Unknown mode, switched to normal.\r\n");
    intercomMode = IntercomMode::NormalMode;
    lock.turnOff();
    break;
  }
  }

  return;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */