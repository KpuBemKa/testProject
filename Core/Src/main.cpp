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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

volatile uint8_t wig_flag_inrt = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

  enum WorkMode
  {
    NormalMode,
    ClosedMode,
    OpenMode,
    CondOpenMode,
    TempOpenMode,
    AlarmMode
  };
  /**
   * NormalMode,
   * ClosedMode,
   * OpenMode,
   * CondOpenMode,
   * TempOpenMode,
   * AlarmMode
  */
  WorkMode workMode = NormalMode, previousWorkMode = NormalMode;

  enum States
  {
    OpenedState,
    ClosedState,
    NullState
  };
  /**
   * OpenedState,
   * ClosedState,
   * NullState
  */
  States doorState = ClosedState, relayState = ClosedState;

  enum RelayMode
  {
    Constant,
    Impulse,
    ConstantSwitch,
    Valve,
    Null
  };
  enum RelayMode relayMode = Constant;
  
  enum AlarmCause
  {
    OpenedDoor,
    None
  };
  enum AlarmCause alarmCause = None;

  // time ticker for verifying if 15 seconds passed to switch mode
  uint32_t timme = 0;

  // was button to open the door pressed?
  bool
    insideButtonPressed = false,
    outsideButtonPressed = false,
    insideKeyRead = false,
    outsideKeyRead = false,
    firstTime = false,
    doorRecentlyClosed = false,
    alarmTrigger = false,
    toBlink = false;

  uint32_t
    relayUnlockingTime = 0,
    timmeTrack = 0,
    blinkTime = 0,
    blinkStart = 0;


  // enum Direction
  // {
  //   IN,
  //   OUT,
  //   BOTH
  // };

  // struct Key
  // {
  //   uint32_t code;
  //   enum Direction direction;
  //   uint32_t accessTime;
  // };

  /*----> Functions Declaration <----*/

  void timeTrackEvent();
  void changeModeEvent();
  void blinkEvent();
  void intercomKeyEvent();
  void buttonPressedEvent();
  void doorUnlockEvent();
  void doorClosedEvent();
  void greenLightEvent();
  void bellEvent();
  void alarmEvent();

  bool verifyCode(uint32_t code);
  void UART_Printf(const char *fmt, ...);
  void switchMode();
  void unlockDoor();
  void unlockRelay();
  void lockDoor();
  void doorChangedState(States currentState);

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    timeTrackEvent();

    changeModeEvent();

    blinkEvent();

    intercomKeyEvent();

    buttonPressedEvent();

    doorUnlockEvent();

    doorClosedEvent();

    greenLightEvent();

    bellEvent();

    alarmEvent();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Zumer1_Pin|Zumer2_Pin|GreenLed_1_Pin|GreenLed_2_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : D_01_Pin D_11_Pin D_02_Pin D_12_Pin */
  GPIO_InitStruct.Pin = D_01_Pin|D_11_Pin|D_02_Pin|D_12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : BT_0_Pin BT_1_Pin */
  GPIO_InitStruct.Pin = BT_0_Pin|BT_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Zumer1_Pin Zumer2_Pin GreenLed_1_Pin GreenLed_2_Pin */
  GPIO_InitStruct.Pin = Zumer1_Pin|Zumer2_Pin|GreenLed_1_Pin|GreenLed_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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

/* USER CODE BEGIN 4 */

/* Events */

void timeTrackEvent()
{
  if((HAL_GetTick() - timmeTrack) > 1000)
  {
    UART_Printf("Time passed: %d\r\n", (HAL_GetTick() - timme));
    timmeTrack = HAL_GetTick();
  }
}

/**
  * @brief Event for acting when it is time to change modes
  * @retval None
*/
void changeModeEvent()
{
  if( (HAL_GetTick() - timme) > 15000 )
  {
    switchMode();
    timme = HAL_GetTick();
  }
}

/**
 * @brief Event for acting when intercom key was inserted
 * @retval None
*/
void intercomKeyEvent()
{
  if(wig_available())
  {
    if(insideKeyRead)
      UART_Printf("Inside reader was used.\r\n");

    if(outsideKeyRead)
      UART_Printf("Outside reader was used.\r\n");

    if(verifyCode(getCode()))
    switch(workMode)
    {
      case NormalMode:
        unlockRelay();
        break;

      case ClosedMode:
        break;

      case OpenMode:
      case TempOpenMode:
        break;

      case CondOpenMode:
        workMode = OpenMode;
        unlockRelay();
        break;

      default:
        break;
    }
  }
}

/**
 * @brief Event for acting when a button to open the door was pressed
 * @retval None
*/
void buttonPressedEvent()
{ 
  if(insideButtonPressed)
    UART_Printf("Inside button was pressed.\r\n");

  if(outsideButtonPressed)
    UART_Printf("Outside button was pressed.\r\n");
 
  if(outsideButtonPressed || insideButtonPressed)
  {
    switch(workMode)
    {
      case NormalMode:
      case ClosedMode:
      {
        unlockRelay();
        previousWorkMode = workMode;
        workMode = TempOpenMode;
        break;
      }
      case OpenMode:
      case TempOpenMode:
      {
        break;
      }
      case CondOpenMode:
      {
        workMode = OpenMode;
        break;
      }
      default:
      {
        break;  
      }
    }
    outsideButtonPressed = false;
    insideButtonPressed = false;
  }
}

void doorUnlockEvent()
{
  if(doorRecentlyClosed)
  {
    lockDoor();
    doorRecentlyClosed = false;
  }
  
  if((HAL_GetTick() - relayUnlockingTime) > 10000 && !doorRecentlyClosed)
  {
    alarmCause = OpenedDoor;
  }
  else if(doorState == ClosedState)
  {
    alarmCause = None;
  }
}

void doorClosedEvent()
{
  if(doorState == ClosedState)
  {
    //relay closing mechanics
    relayState = ClosedState;
  }
}

void greenLightEvent()
{
  switch(relayState)
  {
    case ClosedState:
    {
      HAL_GPIO_WritePin(GreenLed_1_GPIO_Port, GreenLed_1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GreenLed_2_GPIO_Port, GreenLed_2_Pin, GPIO_PIN_SET);
      break;
    }
    case OpenedState:
    {
      HAL_GPIO_WritePin(GreenLed_1_GPIO_Port, GreenLed_1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GreenLed_2_GPIO_Port, GreenLed_2_Pin, GPIO_PIN_RESET);
      break;
    }
    default:
    {
      HAL_GPIO_WritePin(GreenLed_1_GPIO_Port, GreenLed_1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GreenLed_2_GPIO_Port, GreenLed_2_Pin, GPIO_PIN_SET);
      break;
    }
  }
}

void bellEvent()
{
  
}

void alarmEvent()
{

}

/* Aux functions */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch(GPIO_Pin)
  {
    case BT_0_Pin:
      insideButtonPressed = true;
      break;

    case BT_1_Pin:
      outsideButtonPressed = true;
      break;

    case Door_Pin:
      if(HAL_GPIO_ReadPin(Door_GPIO_Port, Door_Pin) == 0)
      {
        doorState = OpenedState;
      }
      else if(HAL_GPIO_ReadPin(Door_GPIO_Port, Door_Pin) == 1)
      {
        doorState = ClosedState;
        doorRecentlyClosed = true;
      }
      break;

    case D_01_Pin:
      if(wig_flag_inrt)
      {
        ReadD0();
        insideKeyRead = true;
      }
      break;
    
    case D_11_Pin:
      if(wig_flag_inrt)
      {
        ReadD1();
        insideKeyRead = true;
      }
      break;

    case D_02_Pin:
      if(wig_flag_inrt)
      {
        ReadD0();
        outsideKeyRead = true;
      }
      break;

    case D_12_Pin:
      if(wig_flag_inrt)
      {
        ReadD1();
        outsideKeyRead = true;
      }
      break;
    
    default:
      break;
  }
}

/**
  @brief Verify if intercom code is valid
  @param code Intercom code to verify
  @retval bool value
*/
bool verifyCode(uint32_t code)
{
  uint32_t codes[] = {
    12563593
  };

  for(int i = 0; i < sizeof(codes)/sizeof(codes[0]); i++){
    if(code == codes[i])
      return true;
  }

  return false;
}

/**
  @brief Print a string to the terminal
*/
void UART_Printf(const char *fmt, ...)
{
  char buff[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buff, sizeof(buff), fmt, args);
  HAL_UART_Transmit(&huart1, (uint8_t *)buff, strlen(buff), HAL_MAX_DELAY);
  va_end(args);
}

/**
  * @brief If mode is not 3, increment it. If it is, set mode to 0
*/
void switchMode()
{
  switch(workMode)
  {
    case NormalMode:
    {
      workMode = ClosedMode;
      break;
    }
    case ClosedMode:
    {
      workMode = OpenMode;
      break;
    }
    case OpenMode:
    {
      workMode = CondOpenMode;
      break;
    }
    case CondOpenMode:
    {
      workMode = NormalMode;
      break;
    }
    default:
    {
      UART_Printf("Unknown mode switched to normal.\r\n");
      workMode = NormalMode;
      break;
    }
  }

  UART_Printf("Switched to mode %d\r\n", workMode);

  return;
}

// void blink(uint16_t blinkDurration)
// {
//   toBlink = true;
//   blinkTime = blinkDurration;
//   blinkStart = HAL_GetTick();
//   HAL_GPIO_WritePin(GreenLed_1_GPIO_Port, GreenLed_1_Pin, RESET);
//   HAL_GPIO_WritePin(GreenLed_2_GPIO_Port, GreenLed_2_Pin, RESET);
// }

void unlockRelay()
{
  UART_Printf("Unlocked the door.\r\n");
  // relay opening mechanics
  relayUnlockingTime = HAL_GetTick();
  relayState = OpenedState;
}

void lockDoor()
{
  if(doorState == ClosedState)
  {
    //relay closing mechanics
    relayState = ClosedState;
  }
  else
  {
    UART_Printf("Error. Door is not closed\r\n");
  }
}
/* USER CODE END 4 */

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

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
