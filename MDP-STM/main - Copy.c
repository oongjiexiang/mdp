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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

//int motorAngle = 77;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim8;

UART_HandleTypeDef huart3;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for theServoMotor */
osThreadId_t theServoMotorHandle;
const osThreadAttr_t theServoMotor_attributes = {
  .name = "theServoMotor",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for theMotor */
osThreadId_t theMotorHandle;
const osThreadAttr_t theMotor_attributes = {
  .name = "theMotor",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for theRightEncoder */
osThreadId_t theRightEncoderHandle;
const osThreadAttr_t theRightEncoder_attributes = {
  .name = "theRightEncoder",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for theLeftEncoder */
osThreadId_t theLeftEncoderHandle;
const osThreadAttr_t theLeftEncoder_attributes = {
  .name = "theLeftEncoder",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for theOLED */
osThreadId_t theOLEDHandle;
const osThreadAttr_t theOLED_attributes = {
  .name = "theOLED",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM3_Init(void);
void StartDefaultTask(void *argument);
void the_ServoMotor(void *argument);
void the_Motor(void *argument);
void the_RightEncoder(void *argument);
void the_LeftEncoder(void *argument);
void the_OLED(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t aRxBuffer[20] = {0};

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
  MX_TIM8_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT (&huart3, aRxBuffer, 1);
  OLED_Init();
  //HAL_UART_Receive_DMA(&huart3, (uint8_t *) aRxBuffer, 10);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of theServoMotor */
  theServoMotorHandle = osThreadNew(the_ServoMotor, NULL, &theServoMotor_attributes);

  /* creation of theMotor */
  theMotorHandle = osThreadNew(the_Motor, NULL, &theMotor_attributes);

  /* creation of theRightEncoder */
  theRightEncoderHandle = osThreadNew(the_RightEncoder, NULL, &theRightEncoder_attributes);

  /* creation of theLeftEncoder */
  theLeftEncoderHandle = osThreadNew(the_LeftEncoder, NULL, &theLeftEncoder_attributes);

  /* creation of theOLED */
  theOLEDHandle = osThreadNew(the_OLED, NULL, &theOLED_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 160;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 10;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 10;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 7199;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, OLED_SCL_Pin|OLED_SDA_Pin|OLED_RST_Pin|OLED_DC_Pin
                          |LED3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, AIN2_Pin|AIN1_Pin|BIN1_Pin|BIN2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : OLED_SCL_Pin OLED_SDA_Pin OLED_RST_Pin OLED_DC_Pin
                           LED3_Pin */
  GPIO_InitStruct.Pin = OLED_SCL_Pin|OLED_SDA_Pin|OLED_RST_Pin|OLED_DC_Pin
                          |LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : AIN2_Pin AIN1_Pin */
  GPIO_InitStruct.Pin = AIN2_Pin|AIN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BIN1_Pin BIN2_Pin */
  GPIO_InitStruct.Pin = BIN1_Pin|BIN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
int changeInitialVal_L;
int changeInitialVal_R;
int dist_to_travel_R;
int dist_to_travel_L;
int motorAngle = 72; // center
float pwmVal_L;
float pwmVal_R;
float pwmVal_L_ori;
float pwmVal_R_ori;
int motorDir;
int diff_R, diff_L;
int multiplierConstant = 65.5;
int outerTurnRadius90Deg = 62;
double turingRatio = 0.573;
int delayMotor = 0;
int withPid;

// used to test PID
void testPID()
{
	changeInitialVal_R = 1;
	changeInitialVal_L = 1;
	dist_to_travel_L = multiplierConstant * 100;
	dist_to_travel_R = multiplierConstant * 100;
	motorDir = 2;
	motorAngle = 72;
	osDelay(1000);
	delayMotor = 1;
	pwmVal_L_ori = 1000;
	pwmVal_R_ori = 1000;
	pwmVal_L = 500;
	pwmVal_R = 1000;
}

void moveCarForward(int distance,int speed)
{
	dist_to_travel_L = multiplierConstant * distance;
	dist_to_travel_R = multiplierConstant * distance;
	motorDir = 2;
	motorAngle = 72;
	delayMotor = 1;
	pwmVal_L_ori = speed;
	pwmVal_R_ori = speed;
}
void moveCarBackwards(int distance,int speed)
{
	dist_to_travel_L = multiplierConstant * distance;
	dist_to_travel_R = multiplierConstant * distance;
	motorDir = 1;
	motorAngle = 72;
	delayMotor = 1;
	pwmVal_L_ori = speed;
	pwmVal_R_ori = speed;
}
void moveCarForwardRight(double angle, int speed)
{
	int distance_L = outerTurnRadius90Deg * angle / 90;
	int distance_R = turingRatio * distance_L;
//	changeInitialVal_R = 1;
//	changeInitialVal_L = 1;
	dist_to_travel_L = multiplierConstant * distance_L;
	dist_to_travel_R = multiplierConstant * distance_R;
	motorDir = 2;
	motorAngle = 89;
//	osDelay(1000);
	delayMotor = 1;
	pwmVal_L_ori = speed;
	pwmVal_R_ori = speed*turingRatio;
//	pwmVal_L = pwmVal_L_ori;
//	pwmVal_R = pwmVal_R_ori;
}
void moveCarForwardLeft(double angle, int speed)
{
	int distance_R = outerTurnRadius90Deg * angle / 90;
	int distance_L = turingRatio * distance_R;
//	changeInitialVal_R = 1;
//	changeInitialVal_L = 1;
	dist_to_travel_L = multiplierConstant * distance_L;
	dist_to_travel_R = multiplierConstant * distance_R;
	motorDir = 2;
	motorAngle = 58;
//	osDelay(1000);
	delayMotor = 1;
	pwmVal_L_ori = speed*turingRatio;
	pwmVal_R_ori = speed;
//	pwmVal_L = pwmVal_L_ori;
//	pwmVal_R = pwmVal_R_ori;
}
void moveCarBackwardRight(double angle, int speed)
{
	int distance_L = outerTurnRadius90Deg * angle / 90;
	int distance_R = turingRatio * distance_L;
//	changeInitialVal_R = 1;
//	changeInitialVal_L = 1;
	dist_to_travel_L = multiplierConstant * distance_L;
	dist_to_travel_R = multiplierConstant * distance_R;
	motorDir = 1;
	motorAngle = 89;
//	osDelay(1000);
	delayMotor = 1;
	pwmVal_L_ori = speed;
	pwmVal_R_ori = speed*turingRatio;
//	pwmVal_L = pwmVal_L_ori;
//	pwmVal_R = pwmVal_R_ori;
}
void moveCarBackwardLeft(double angle, int speed)
{
	int distance_R = outerTurnRadius90Deg * angle / 90;
	int distance_L = turingRatio * distance_R;
//	changeInitialVal_R = 1;
//	changeInitialVal_L = 1;
	dist_to_travel_L = multiplierConstant * distance_L;
	dist_to_travel_R = multiplierConstant * distance_R;
	motorDir = 1;
	motorAngle = 58;
//	osDelay(1000);
	delayMotor = 1;
	pwmVal_L_ori = speed*turingRatio;
	pwmVal_R_ori = speed;
//	pwmVal_L = pwmVal_L_ori;
//	pwmVal_R = pwmVal_R_ori;
}

int PID_R(int current_error, int previous_error,int integration_sum)
{
	double Kp = 0.3;
	double Ki = 0.15; //= 0.2;
	double Kd = 0.12; //= 0.15;
	return (Kp * current_error) + (Ki * integration_sum * 0.1) + (Kd * (current_error - previous_error) / 0.1);
}
int PID_L(int current_error, int previous_error,int integration_sum)
{
	double Kp = 0.3;
	double Ki = 0.15; //= 0.2;
	double Kd = 0.12; //= 0.15;
	return (Kp * current_error) + (Ki * integration_sum * 0.1) + (Kd * (current_error - previous_error) / 0.1);
}
int iterationCount = 0;
uint8_t receivedChars[50];
uint8_t temp_command[50];
uint8_t temp_value[50];
int i;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/*Prevent unused argument(s) compilation warning */
	UNUSED(huart);
//	HAL_UART_Transmit(&huart3, aRxBuffer,10,0xFFFF);
//	if(aRxBuffer[0] != 'z')
//	{
//		receivedChars[iterationCount] = aRxBuffer[0];
//		iterationCount ++;
//	}
//	else
//	{
//		receivedChars[iterationCount] = aRxBuffer[0];
//		int temp_i = 0;
//		int command_done = 0;
//		while (receivedChars[i]!= 'z') {
//			if(command_done == 1)
//			{
//				temp_value[temp_i] = receivedChars[i];
//				temp_i ++;
//			}
//			else if(receivedChars[i]!= ' ')
//			{
//				temp_command[temp_i] = receivedChars[i];
//				temp_i ++;
//			}
//			else
//			{
//				temp_command[temp_i] = '\0';
//				command_done = 1;
//				temp_i = 0;
//			}
//			i++;
//		}
//		if(strcmp(temp_command ,"forward"))
//		{
//			moveCarForward(100,1500);
//		}
//		HAL_UART_Transmit(&huart3, temp_command,10,0xFFFF);
//		HAL_UART_Transmit(&huart3, temp_value,10,0xFFFF);
//		iterationCount = 0;
//		for(i = 0; i < 50; i ++)
//		{
//			receivedChars[i] = ' ';
//			temp_command[i] = ' ';
//			temp_value[i] = ' ';
//		}
//	}
	if(aRxBuffer[0] == 'a'){	// left
		moveCarForward(5,1500);
	}
	if(aRxBuffer[0] == 'b'){	// left
		moveCarForward(10,1500);
	}
	if(aRxBuffer[0] == 'c'){	// left
		moveCarForwardLeft(5,1500);
	}
	if(aRxBuffer[0] == 'd'){	// left
		moveCarForwardRight(5,1500);
	}
	if(aRxBuffer[0] == 'e'){	// left
		moveCarBackwards(5,1500);
	}
	if(aRxBuffer[0] == 'f'){	// left
		moveCarBackwards(10,1500);
	}
	if(aRxBuffer[0] == 'g'){	// left
		moveCarForwardLeft(45,1500);
	}
	if(aRxBuffer[0] == 'h'){	// left
		moveCarForwardRight(45,1500);
	}
	if(aRxBuffer[0] == 'i'){	// left
		moveCarForwardLeft(90,1500);
	}
	if(aRxBuffer[0] == 'j'){	// left
		moveCarForwardRight(90,1500);
	}
	if(aRxBuffer[0] == 'l'){	// left
		moveCarForwardRight(60,1500);
	}
	if(aRxBuffer[0] == 'm'){	// left
		moveCarForwardRight(30,1500);
	}
	if(aRxBuffer[0] == 'k'){	// left
		moveCarForward(100,1500);
	}
//	if(aRxBuffer[0] == 'k'){	// center
//		motorAngle = 72;
//	}
//	if(aRxBuffer[0] == 'l'){	// right
//		motorAngle = 86;
//	}
//	if(aRxBuffer[0] == 'w'){	// forward
//		motorDir = 2;
//		pwmVal_L = pwmVal_L_ori;
//		pwmVal_R = pwmVal_L_ori;
//	}
//	if(aRxBuffer[0] == 's'){	// stop
//		motorDir = 0;
//		pwmVal_L = 0;
//		pwmVal_R = 0;
//	}
//	if(aRxBuffer[0] == 'x'){	// reverse
//		motorDir = 1;
//		pwmVal_L = pwmVal_R_ori;
//		pwmVal_R = pwmVal_R_ori;
//	}
	HAL_UART_Receive_IT (&huart3, aRxBuffer, 1);
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
	  /* Infinite loop */
	withPid = 1;
	  for(;;)
	  {
		  osDelay(2000);
		  moveCarForward(120,1500);
		  osDelay(9000);
		  moveCarBackwards(120,1500);
//			moveCarBackwardLeft(135,1500);
		  osDelay(7000);
	  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_the_ServoMotor */
/**
* @brief Function implementing the theServoMotor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_the_ServoMotor */
void the_ServoMotor(void *argument)
{
  /* USER CODE BEGIN the_ServoMotor */
  /* Infinite loop */
  for(;;)
  {
	  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
	  htim1.Instance->CCR4 = motorAngle;
    osDelay(1);
  }
  /* USER CODE END the_ServoMotor */
}

/* USER CODE BEGIN Header_the_Motor */
/**
* @brief Function implementing the theMotor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_the_Motor */
void the_Motor(void *argument)
{
  /* USER CODE BEGIN the_Motor */
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_2);
  /* Infinite loop */
  for(;;)
  {
	  if(delayMotor == 1)
	  {
		  osDelay(1000);
		  delayMotor = 0;
		  changeInitialVal_R = 1;
		  	changeInitialVal_L = 1;
	  }
	  if(motorDir == 0){
		  __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwmVal_R);
		  __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwmVal_L);

	  }
	  else if(motorDir == 1){
			HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_RESET);
			  __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwmVal_R);
			  __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwmVal_L);
	  }
	  else if(motorDir == 2){
			HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);
			  __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwmVal_R);
			  __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwmVal_L);
	  }
    osDelay(1);
  }
  /* USER CODE END the_Motor */
}

/* USER CODE BEGIN Header_the_RightEncoder */
/**
* @brief Function implementing the theRightEncoder thread.
* @param argument: Not used
* @retval None
*/
int dist_curr_R;
uint16_t dir_R;
uint8_t uartTest[20];
int distTravelled_R;
int distTravelled_L;
int motorRunning_R = 0;
int motorRunning_L = 0;
/* USER CODE END Header_the_RightEncoder */
void the_RightEncoder(void *argument)
{
  /* USER CODE BEGIN the_RightEncoder */
	HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_ALL);
	int cnt1, cnt2;
	uint32_t tick, tick2;
	cnt1 = __HAL_TIM_GET_COUNTER(&htim2);
	tick = tick2 = HAL_GetTick();
	int tempCounterVal;
	int initialDistTempVal;
	int current_error = 0;
	int integration_sum  = 0;
	int previous_error = 0;
	int speedPerSec = 0;
	float pidAdjustment = 0;
  /* Infinite loop */
  for(;;)
  {
	  tempCounterVal = __HAL_TIM_GET_COUNTER(&htim2);
		if(changeInitialVal_R == 1)
		{
			//osDelay(1000);
			initialDistTempVal = tempCounterVal;
			integration_sum = 0;
			previous_error = 0;
			distTravelled_R = 0;
			changeInitialVal_R = 0;
			if(withPid == 0)
				pwmVal_R = pwmVal_R_ori;
			else
				pwmVal_R = 0;
			motorRunning_R = 1;

			distTravelled_R = distTravelled_L = 0;
		}
		/*
		if(HAL_GetTick()-tick2 > 100L){	// unable to display properly below 1000L
//			if(motorRunning == 1 && pwmVal_R != pwmVal_R_ori)
//			{
//				pwmVal_R += 0.1 * pwmVal_R_ori;
//				pwmVal_L += 0.1 * pwmVal_L_ori;
//			}
			if(motorRunning == 1 &&
					((((diff_L * 0.1 + distTravelled_L) + (distTravelled_L - distTravelled_R)) - distTravelled_R) / 0.1) > 0.3 * pwmVal_R &&
					((((diff_L * 0.1 + distTravelled_L) + (distTravelled_L - distTravelled_R)) - distTravelled_R) / 0.1) < 1.7 * pwmVal_R)
				pwmVal_R = (((diff_L * 0.1 + distTravelled_L) + (distTravelled_L - distTravelled_R)) - distTravelled_R) / 0.1;
				//pwmVal_R += (5 * (distTravelled_L - distTravelled_R ));
			tick2 = HAL_GetTick();
		}
		*/
		if(HAL_GetTick()-tick > 100L){	// unable to display properly below 1000L
			cnt2 = tempCounterVal;
			if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2)){
				if(cnt2<cnt1)
					diff_R = cnt1 - cnt2;
				else
					diff_R = (65535 - cnt2)+cnt1;
			}
			else{
				if(cnt2>cnt1)
					diff_R = cnt2 - cnt1;
				else
					diff_R = (65535 - cnt1)+cnt2;
			}
			dir_R = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2);
			cnt1 = tempCounterVal;
			tick = HAL_GetTick();
			// PID algo, currently still causes robot to deviate
			speedPerSec = diff_R * 10;
			if(speedPerSec > 10000) speedPerSec = 0;
			if(withPid == 1)
			if(motorRunning_R == 1){
			//if(1 == 0){
			//if(speedPerSec > pwmVal_R_ori - (pwmVal_R_ori * 0.8) && speedPerSec < pwmVal_R_ori + (pwmVal_R_ori * 0.8) && delayMotor == 0){
				current_error = pwmVal_R_ori - speedPerSec;
				integration_sum = integration_sum + current_error ;
				pidAdjustment = PID_R(current_error,previous_error,integration_sum);
				sprintf(uartTest, "pid:%5d ",(int)(pidAdjustment));
				OLED_ShowString(0,50,uartTest);
				pwmVal_R = pwmVal_R + pidAdjustment;
				previous_error = current_error;
			}
			// sends the ready state to algo
			  if(motorRunning_R == 0 && motorRunning_L == 0)
			  {
				sprintf(uartTest, "ALGO|a\n");
				  HAL_UART_Transmit(&huart3, uartTest,10,0xFFFF);
			  }
		}
		// count the distance travelled and stops the car when the dist is req to required
		if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2)){
			if(initialDistTempVal > tempCounterVal){
				if(initialDistTempVal - tempCounterVal  > dist_to_travel_R  && initialDistTempVal - tempCounterVal < 20000){
					pwmVal_R = 0;
					motorRunning_R = 0;
				}
				distTravelled_R = initialDistTempVal - tempCounterVal;
			}
			else
			{
				if(65535 - tempCounterVal + initialDistTempVal > dist_to_travel_R   && 65535 - tempCounterVal + initialDistTempVal < 20000){
					pwmVal_R = 0;
					motorRunning_R = 0;
				}
				distTravelled_R = 65535 - tempCounterVal + initialDistTempVal;
			}
		}else{
			if(initialDistTempVal < tempCounterVal){
				if(tempCounterVal - initialDistTempVal > dist_to_travel_R  && tempCounterVal - initialDistTempVal - tempCounterVal < 20000){
					pwmVal_R = 0;
					motorRunning_R = 0;
				}
				distTravelled_R = tempCounterVal - initialDistTempVal;
			}
			else
			{
				if(65535 - initialDistTempVal + tempCounterVal> dist_to_travel_R   && 65535 - initialDistTempVal + tempCounterVal < 20000){
					pwmVal_R = 0;
					motorRunning_R = 0;
				}
				distTravelled_R = 65535 - initialDistTempVal + tempCounterVal;
			}
		}
	    //osDelay(1);
  }
  /* USER CODE END the_RightEncoder */
}

/* USER CODE BEGIN Header_the_LeftEncoder */
/**
* @brief Function implementing the theLeftEncoder thread.
* @param argument: Not used
* @retval None
*/
int dist_curr_L;
uint16_t dir_L;
/* USER CODE END Header_the_LeftEncoder */
void the_LeftEncoder(void *argument)
{
  /* USER CODE BEGIN the_LeftEncoder */
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
	int cnt1, cnt2;
	uint32_t tick;
	cnt1 = __HAL_TIM_GET_COUNTER(&htim3);
	tick = HAL_GetTick();
	int tempCounterVal = 0;
	int initialDistTempVal = 0;
	int current_error = 0;
	int integration_sum  = 0;
	int previous_error = 0;
	uint8_t tempUart[20];
	int speedPerSec = 0;
	distTravelled_L = 0;
	float pidAdjustment;
  /* Infinite loop */
  for(;;)
  {
	  tempCounterVal = __HAL_TIM_GET_COUNTER(&htim3);
		if(changeInitialVal_L == 1)
		{
			//osDelay(1000);
			initialDistTempVal = tempCounterVal;
			integration_sum = 0;
			previous_error = 0;
			changeInitialVal_L = 0;
			if(withPid == 0)
				pwmVal_L = pwmVal_L_ori;
			else
				pwmVal_L = 0;
			motorRunning_L = 1;
			distTravelled_L = 0;
		}
		if(HAL_GetTick()-tick > 100L){	// unable to display properly below 1000L
			cnt2 = tempCounterVal;
			if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3)){
				if(cnt2<cnt1)
					diff_L = cnt1 - cnt2;
				else
					diff_L = (65535 - cnt2)+cnt1;
			}
			else{
				if(cnt2>cnt1)
					diff_L = cnt2 - cnt1;
				else
					diff_L = (65535 - cnt1)+cnt2;
			}
			dir_L = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3);
			cnt1 = tempCounterVal;
			tick = HAL_GetTick();
			// PID algo, currently still causes robot to deviate
			speedPerSec = diff_L * 10;
			if(speedPerSec > 10000) speedPerSec = 0;
			if(withPid == 1)
			if(motorRunning_L == 1){
			//if(1 == 0){
			//if(speedPerSec > pwmVal_L_ori - (pwmVal_L_ori * 0.8) && speedPerSec < pwmVal_L_ori + (pwmVal_L_ori * 0.8) && delayMotor == 0){
				current_error = pwmVal_L_ori - speedPerSec ;
				integration_sum = integration_sum + current_error ;
				pidAdjustment = PID_L(current_error,previous_error,integration_sum);
				pwmVal_L = pwmVal_L + pidAdjustment;
				previous_error = current_error;
			}

		}
		// count the distance travelled and stops the car when the dist is req to required
		if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3)){
			if(initialDistTempVal > tempCounterVal){
				if(initialDistTempVal - tempCounterVal  > dist_to_travel_L && initialDistTempVal - tempCounterVal < 20000){
					pwmVal_L = 0;
					motorRunning_L = 0;
				}
				distTravelled_L = initialDistTempVal - tempCounterVal;
			}
			else
			{
				if(65535 - tempCounterVal + initialDistTempVal> dist_to_travel_L  && 65535 - tempCounterVal + initialDistTempVal < 20000){
					pwmVal_L = 0;
					motorRunning_L = 0;
				}
				distTravelled_L = 65535 - tempCounterVal + initialDistTempVal;
			}
		}else
		{
			if(initialDistTempVal < tempCounterVal){
				if( tempCounterVal  - initialDistTempVal > dist_to_travel_L  && tempCounterVal  - initialDistTempVal < 20000){
					pwmVal_L = 0;
					motorRunning_L = 0;
				}
				distTravelled_L = tempCounterVal  - initialDistTempVal;
			}
			else
			{
				if(65535 - initialDistTempVal + tempCounterVal > dist_to_travel_L  && 65535 - initialDistTempVal + tempCounterVal < 20000){
					pwmVal_L = 0;
					motorRunning_L = 0;
				}
				distTravelled_L = 65535 - initialDistTempVal + tempCounterVal;
			}
		}
  }
  /* USER CODE END the_LeftEncoder */
}

/* USER CODE BEGIN Header_the_OLED */
/**
* @brief Function implementing the theOLED thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_the_OLED */
void the_OLED(void *argument)
{
  /* USER CODE BEGIN the_OLED */
  /* Infinite loop */
	uint8_t display_Rx[30];
	uint8_t display_RMotor[30];
	uint8_t display_LMotor[30];
	uint8_t display_Dist[15];
  for(;;)
  {
//	  sprintf(display_Rx,"Rx: %s\0",aRxBuffer);
//	  OLED_ShowString(0,0,display_Rx);
	  sprintf(display_RMotor, "M_R:%5d..%1d\0",diff_R,dir_R);
	  OLED_ShowString(0,10,display_RMotor);
	  sprintf(display_RMotor, "M_L:%5d..%1d\0",diff_L,dir_L);
	  OLED_ShowString(0,20,display_RMotor);
//		if(dir_L == 0)	dir_L == 1;
//		else dir_L == 0;
//	  sprintf(display_LMotor, "%1d : %1d\0",(int)dist_to_travel_L,(int)dist_to_travel_R);
//	  OLED_ShowString(0,30,display_LMotor);
//	  int tempInt1 = pwmVal_R_ori;
//	  float tmpFrac = pwmVal_R_ori - tempInt1;      // Get fraction (0.0123).
//	  int tempInt2 = trunc(tmpFrac * 10000);
//	  sprintf (display_Dist, "%d.%04d\n", tempInt1, tempInt2);
	  //sprintf(display_Dist, "%10f\0",pwmVal_R);
//	  OLED_ShowString(0,30,display_Dist);
	  OLED_Refresh_Gram();
    osDelay(1);
  }
  /* USER CODE END the_OLED */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  /* USER CODE END Callback 1 */
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
