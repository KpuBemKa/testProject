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
// #include <stdbool.h>
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

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

enum WorkMode
{
  NormalMode,
  ClosedMode,
  OpenMode,
  CondOpenMode,
  TempOpenMode,
  AlarmMode,
  NoMode
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

enum State
{
  OpenedState,
  ClosedState,
  NullState
};

enum BeepState
{
  Off,
  On
};

enum RelayType
{
  Constant,
  Impulse,
  ConstantSwitch,
  Valve,
  Null
};

enum AlarmCause
{
  OpenedDoor,
  None
};
AlarmCause alarmCause = None;
/* USER CODE BEGIN PFP */

/*----> Functions Declaration Start <----*/

/**
 * @brief Event to track time between modes
 * @retval None
*/
void timeTrackEvent();
/**
 * @brief Event to change work mode every 15 seconds
 * @retval None
*/
void changeModeEvent();
/**
 * @brief Event to check if door was [opened]/[closed] and [print]/[close relay and print]
 * @retval None
*/
void doorChangedStateEvent();
/**
 * @brief Event to check the intercom key reader
 * @retval None
*/
void intercomKeyEvent();
/**
 * @brief Event to check if buttons were pressed
 * @retval None
*/
void buttonPressedEvent();
/**
 * @brief Event to check if door [is not closed]/[closed and relay is opened] and [enable alarm]/[close relay and change work mode]
 * @retval None
*/
void doorCheckEvent();
/**
 * @brief Event to check relay state and enable/disable green light on intercoms
 * @retval None
*/
void greenLightEvent();
/**
 * @brief Event to ring the bell when work mode is TempOpenMode
 * @retval None
*/
void bellEvent();
// not implemented yet
void alarmEvent();

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

/*----> Functions Declaration End <----*/

/* USER CODE END PFP */

class Door
{
  private:
    State doorState;
    bool doorStateChanged;

  public:
    Door();

    Door(State doorState)
    {
      this->doorState = doorState;
    }

    State getDoorState()
    {
      return doorState;
    }

    void setDoorState(State doorState)
    {
      this->doorState = doorState;
      this->doorStateChanged = true;
    }

    bool isDoorStateChanged()
    {
      return this ->doorStateChanged;
    }

    void setDoorStateChanged(bool doorStateChanged)
    {
      this ->doorStateChanged = doorStateChanged;
    }
};

Door door = Door(ClosedState);

class Relay
{
  private:
    RelayType relayType;
    State relayState;
    uint32_t relayOpenTime;

  public:
    Relay()
    {

    }

    Relay(RelayType relayType, State relayState, uint32_t relayOpenTime)
    {
      this->relayType = relayType;
      this->relayState = relayState;
      this->relayOpenTime = relayOpenTime;
    }
    
    RelayType getRelayType()
    {
      return this->relayType;
    }

    void setRelayType(RelayType relayType)
    {
      this->relayType = relayType;
    }

    State getRelayState()
    {
      return this->relayState;
    }

    void setRelayState(State relayState)
    {
      if(relayState == ClosedState)
      {
        if(door.getDoorState() == ClosedState)
        {
          UART_Printf("Relay was closed.\r\n");
          // implement relay closing mechanics
          this->relayState = relayState;
        }
        else
        {
          UART_Printf("Error. Door is not closed.\r\n");
          // implement alarm mechanics
        }
      }
      else if(relayState == OpenedState)
      {
        // implement relay opening mechanics
        this->relayState = relayState;
      }
    }

    uint32_t getRelayOpenTime()
    {
      return this ->relayOpenTime;
    }

    void setRelayOpenTime(uint32_t relayOpenTime)
    {
      this->relayOpenTime = relayOpenTime;
    }
};

class Bell
{
  private:
    BeepState bellState;
    uint32_t lastBeepTime;
    uint32_t beepStartTime;

  public:
    Bell()
    {

    }

    Bell(BeepState bellState, uint32_t lastBeepTime, uint32_t beepStartTime)
    {
      this->bellState = bellState;
      this->lastBeepTime = lastBeepTime;
      this->beepStartTime = beepStartTime;
    }

    BeepState getBellState() {
      return this->bellState;
    }

    void setBellState(BeepState bellState) {
      this->bellState = bellState;

      if(bellState == On)
      {
        HAL_GPIO_WritePin(Zumer1_GPIO_Port, Zumer1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(Zumer2_GPIO_Port, Zumer2_Pin, GPIO_PIN_RESET);
      }
      else if(bellState == Off)
      {
        HAL_GPIO_WritePin(Zumer1_GPIO_Port, Zumer1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(Zumer2_GPIO_Port, Zumer2_Pin, GPIO_PIN_SET);
      }
    }

    uint32_t getLastBeepTime() {
      return this->lastBeepTime;
    }

    void setLastBeepTime(uint32_t lastBeepTime) {
      this->lastBeepTime = lastBeepTime;
    }

    uint32_t getBeepStartTime() {
      return this->beepStartTime;
    }

    void setBeepStartTime(uint32_t beepStartTime) {
      this->beepStartTime = beepStartTime;
    }
};

bool
  insideButtonPressed = false,
  outsideButtonPressed = false,
  insideKeyRead = false,
  outsideKeyRead = false;

uint32_t
  timmeTrack = 0,
  timme = 0;

/* OBJECTS START */

Relay relay = Relay(Constant, ClosedState, 0);

Bell bell = Bell(Off, 0, 0);

/* OBJECTS END */

/* USER CODE END 0 */

/* DEFAULT INITS START */

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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Zumer1_Pin | Zumer2_Pin | GreenLed_1_Pin | GreenLed_2_Pin, GPIO_PIN_SET);

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
  GPIO_InitStruct.Pin = Zumer1_Pin | Zumer2_Pin | GreenLed_1_Pin | GreenLed_2_Pin;
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

/* DEFAULT INITS END */

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
  
  UART_Printf("\nSwitched to Normal mode.\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    timeTrackEvent();

    changeModeEvent();

    doorChangedStateEvent();

    intercomKeyEvent();

    buttonPressedEvent();

    doorCheckEvent();

    greenLightEvent();

    bellEvent();

    alarmEvent();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */

/* Events */

void timeTrackEvent()
{
  if ((HAL_GetTick() - timmeTrack) > 5000)
  {
    UART_Printf("Time passed: %d%s\r\n", (HAL_GetTick() - timme) / 1000, " sec.");
    timmeTrack = HAL_GetTick();
  }
}

void changeModeEvent()
{
  if ((HAL_GetTick() - timme) > 15000)
  {
    timme = HAL_GetTick();
    switchMode();
    UART_Printf("Work mode: %d\r\n", workMode);
  }
}

void blinkEvent()
{
  // if (toBlink && (HAL_GetTick() - blinkStart) > blinkTime)
  // {
  //   HAL_GPIO_WritePin(GreenLed_1_GPIO_Port, GreenLed_1_Pin, GPIO_PIN_SET);
  //   HAL_GPIO_WritePin(GreenLed_2_GPIO_Port, GreenLed_2_Pin, GPIO_PIN_SET);
  //   toBlink = false;
  // }
}

void doorChangedStateEvent()
{
  if(door.isDoorStateChanged())
  {
    switch(door.getDoorState())
    {
      case OpenedState:
      {
        UART_Printf("Door was opened.\r\n");
        break;
      }
      case ClosedState:
      {
        if(workMode == TempOpenMode)
        {
          workMode = previousWorkMode;
          relay.setRelayState(ClosedState);
        }
        UART_Printf("Door was closed.\r\n");
        break;
      }
      default:
      {
        break;
      }
    }
      
    door.setDoorStateChanged(false);
  }
}

void intercomKeyEvent()
{
  if(wig_available() && verifyCode(getCode()))
  {
    if (insideKeyRead)
      UART_Printf("Inside key reader has been used.\r\n");

    if (outsideKeyRead)
      UART_Printf("Outside key reader has been used.\r\n");

    switch (workMode)
    {
      case NormalMode:
      {
        UART_Printf("Key id: %d\r\n", getCode());
        relay.setRelayState(OpenedState);
        relay.setRelayOpenTime(HAL_GetTick());
        previousWorkMode = workMode;
        workMode = TempOpenMode;
        break;
      }
      case ClosedMode:  
      {
        UART_Printf("Door is locked.\r\n");
        // blink mechanics to implement
        break;
      }
      case OpenMode:
      case TempOpenMode:
      {
        break;
      }
      case CondOpenMode:
      {
        UART_Printf("Switched to Open mode.\r\n");
        workMode = OpenMode;
        relay.setRelayState(OpenedState);
        break;
      }
      default:
      {
        break;  
      }
    }
  insideKeyRead = false;
  outsideKeyRead = false;
  }
}

void buttonPressedEvent()
{
  if (insideButtonPressed)
    UART_Printf("Inside button was pressed.\r\n");

  if (outsideButtonPressed)
    UART_Printf("Outside button was pressed.\r\n");

  if (outsideButtonPressed || insideButtonPressed)
  {
    switch (workMode)
    {
      case NormalMode:
      case ClosedMode:
      {
        UART_Printf("Door has been unlocked\r\n");
        relay.setRelayState(OpenedState);
        relay.setRelayOpenTime(HAL_GetTick());
        bell.setLastBeepTime(HAL_GetTick());
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
        UART_Printf("Switched to Open mode.\r\n");
        workMode = OpenMode;
        relay.setRelayState(OpenedState);
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

void doorCheckEvent()
{
  if( (HAL_GetTick() - relay.getRelayOpenTime()) > 5000 && door.getDoorState() == OpenedState)
  {
    // trigger alarm
    UART_Printf("Alarm triggered.\r\n");
    // workMode = AlarmMode;
  }
  else if((HAL_GetTick() - relay.getRelayOpenTime()) > 5000 && door.getDoorState() == ClosedState && workMode == TempOpenMode)
  {
    relay.setRelayState(ClosedState);
    workMode = previousWorkMode;
  }
}

void greenLightEvent()
{
  switch (relay.getRelayState())
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
  if(HAL_GetTick()-relay.getRelayOpenTime() < 5000 && workMode == TempOpenMode)
  {
    if(HAL_GetTick()-bell.getLastBeepTime() > 250)
    {
      bell.setLastBeepTime(HAL_GetTick());
      bell.setBellState(On);
    }
    
    if(HAL_GetTick()-bell.getLastBeepTime() < 250 && HAL_GetTick()-bell.getLastBeepTime() > 75)
    {
      bell.setBellState(Off);
    }
  }
}

void alarmEvent()
{
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
      if (HAL_GPIO_ReadPin(Door_GPIO_Port, Door_Pin) == 0)
      {
        door.setDoorState(ClosedState);
      }
      else if (HAL_GPIO_ReadPin(Door_GPIO_Port, Door_Pin) == 1)
      {
        door.setDoorState(OpenedState);
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

  for (int i = 0; i < sizeof(codes) / sizeof(codes[0]); i++)
  {
    if (code == codes[i])
      return true;
  }

  return false;
}

void switchMode()
{
  switch(workMode)
  {
    case NormalMode:
    {
      UART_Printf("\nSwitched to Closed mode.\r\n");
      workMode = ClosedMode;
      relay.setRelayState(ClosedState);
      break;
    }
    case ClosedMode:
    {
      UART_Printf("\nSwitched to Open mode.\r\n");
      workMode = OpenMode;
      relay.setRelayState(OpenedState);
      break;
    }
    case OpenMode:
    {
      UART_Printf("\nSwitched to CondOpen mode.\r\n");
      workMode = CondOpenMode;
      relay.setRelayState(ClosedState);
      break;
    }
    case CondOpenMode:
    {
      UART_Printf("\nSwitched to Normal mode.\r\n");
      workMode = NormalMode;
      relay.setRelayState(ClosedState);
      break;
    }
    default:
    {
      UART_Printf("Unknown mode, switched to normal.\r\n");
      workMode = NormalMode;
      relay.setRelayState(ClosedState);
      break;
    }
  }

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
