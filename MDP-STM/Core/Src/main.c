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
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
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
/* Definitions for theOLED */
osThreadId_t theOLEDHandle;
const osThreadAttr_t theOLED_attributes = {
  .name = "theOLED",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ultrasonicSenso */
osThreadId_t ultrasonicSensoHandle;
const osThreadAttr_t ultrasonicSenso_attributes = {
  .name = "ultrasonicSenso",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
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
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
void StartDefaultTask(void *argument);
void the_ServoMotor(void *argument);
void the_Motor(void *argument);
void the_RightEncoder(void *argument);
void the_OLED(void *argument);
void StartTask08(void *argument);

/* USER CODE BEGIN PFP */
void enqueue(char*);
char* dequeue(void);
void show(void);
void HCSR04_Read (void);
void motor_stop(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t aRxBuffer[20] = {0};
char action_queue[8000][4];
int Rear = - 1;
int Front = - 1;
// motor running state
int motorRunning_R = 0;
int motorRunning_L = 0;
int changeInitialVal_L = 0;
int changeInitialVal_R = 0;
// used to control the distance the motor travels before it stops
int dist_to_travel_R;
int dist_to_travel_L;
// used to adjust steering
double motorAngle = 72; // center
// current pwmVal
float pwmVal_L;
float pwmVal_R;
// target pwmVal
float pwmVal_L_ori;
float pwmVal_R_ori;
// controls the front or back direction, 2 for front, 1 for back
int motorDir;
// the speed of the motors
int diff_R, diff_L;
// multiplier for cm to encoder value
int multiplierConstant = 66.5;
// the distance the outer wheel travels when turning right

// values for smooth floor
//int outerTurnRadius90Deg_R = 59.9;
//int outerTurnRadius90Deg_R_back = 59.1;
//int outerTurnRadius90Deg_L = 59.3;
//int outerTurnRadius90Deg_L_backLeft = 62.4;
// end of values for smooth floor

// values for outside lab
int outerTurnRadius90Deg_R = 56;
int outerTurnRadius90Deg_R_back = 58.6;
int outerTurnRadius90Deg_L = 60.2;
int outerTurnRadius90Deg_L_backLeft = 62.9;
// end of values for outside lab

// values for smooth floor for week9 ?!
//int outerTurnRadius90Deg_R = 58.8;
//int outerTurnRadius90Deg_R_back = 58.6;
//int outerTurnRadius90Deg_L = 59.8;
//int outerTurnRadius90Deg_L_backLeft = 62.9;
// end of values for smooth floor

// the ratio of distance and speed for the inner wheel to outer wheel
double turingRatio_R = 0.50;//0.573;
double turingRatio_L = 0.50;//0.573;
// to delay the motor as immediately moving the motor after setting the variables will cause problems
int delayMotor = 0;
// set to 1 to enable motor control with PID, 0 to disable it
int withPid = 0;
// temp variable to test a new program flow
int motorReady = 1;
int sensorState;  // state of IR sensors if distance is equal or different from each other
int flag_Calibration = 0; // interest flag
int flag_NextSide = 0; // NextSide flag
uint8_t sendAlgo_Ready[7] = "ALG|R\n\0";
uint8_t sendAlgo_ready[7] = "ALG|r\n\0";
uint8_t sendAlgo_found[7] = "ALG|F\n\0";
uint8_t sendAND_End[11] = "AND|END\n\0";
// variable to enable user defined distance to clear checklist
char prevCommand = ' ';
uint16_t dir_R;
// save distance traveled : was meant for correcting misaligned direction
int distTravelled_R;
int distTravelled_L;
int dist_curr_L;
uint16_t dir_L;
int reAlignWheel = 0;
int reAlignWheelForward = 0;
int reAlignWheelBackward = 0;
// ultra sonic variables
int ultraSonicReady = 1;
uint8_t ultraLed1[20];
uint32_t IC_Val1 = 0;
uint32_t IC_Val2 = 0;
uint32_t Difference = 0;
uint32_t Distance  = 0;
uint32_t ultraSensorCount = 0;
int Is_First_Captured = 0;
// IR Sensor distances
int32_t distance_IR_R, distance_IR_L = -1;
int32_t distance_Acquired = 0;
// temporary alternating ready char flag
int readyFlag = 1;
// temp test ready sent flag
int readySent = 0;
int waitingForFirstCommand = 1;
// week 9 initial travel distance
int week9InitialTravel = 0;
int week9InitialTravelFlag = 0;
int week9InitialTravelMovementFlag = 0;
int week9RightTurnTravel = 0;
int week9RightTurnTravelFlag = 0;
int week9DetectLeftWall = 0;
int week9returningFlag = 0;

int turningLeft = 0;

	int initialDistTempVal_R = 0;
	int initialDistTempVal_L = 0;
	int tempCounterVal_R;
	int tempCounterVal_L;
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
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
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

  /* creation of theOLED */
  theOLEDHandle = osThreadNew(the_OLED, NULL, &theOLED_attributes);

  /* creation of ultrasonicSenso */
  ultrasonicSensoHandle = osThreadNew(StartTask08, NULL, &ultrasonicSenso_attributes);

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */
  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

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
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 8-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 0xffff;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 8-1;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 65535;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, OLED_SCL_Pin|OLED_SDA_Pin|OLED_RST_Pin|OLED_DC_Pin
                          |LED3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, AIN2_Pin|AIN1_Pin|BIN1_Pin|BIN2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);

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

  /*Configure GPIO pin : PD13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PA14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void moveCarForward(int distance,int speed)
{
	dist_to_travel_L = multiplierConstant * distance;
	dist_to_travel_R = multiplierConstant * distance;
	motorDir = 2;
	motorAngle = 72;
	reAlignWheelForward = 1;
	turningLeft = 0;
	delayMotor = 1;
	pwmVal_L_ori = speed;
	// for inside lab floor
//	pwmVal_R_ori = speed * 0.98;
	// end of inside lab floor
	// for outside lab floor
	pwmVal_R_ori = speed * 1.05;
	// end of outside lab floor

}
void moveCarBackwards(int distance,int speed)
{
	dist_to_travel_L = multiplierConstant * distance;
	dist_to_travel_R = multiplierConstant * distance;
	motorDir = 1;
	motorAngle = 72;
	reAlignWheelBackward = 1;
	turningLeft = 0;
	delayMotor = 1;
	// for smooth floor
	pwmVal_L_ori = speed * 0.88;
	// end of smooth floor
	// for outside lab floor
//	pwmVal_L_ori = speed * 0.84;
	// end of outside lab floor
	pwmVal_R_ori = speed;
}
void moveCarForwardRight(double angle, int speed)
{
	int distance_L = outerTurnRadius90Deg_R * angle / 90;
	int distance_R = distance_L * turingRatio_R;
	dist_to_travel_L = multiplierConstant * distance_L;
	dist_to_travel_R = multiplierConstant * distance_R;
	motorDir = 2;
	motorAngle = 89;
	reAlignWheel = 0;
	turningLeft = 1;
	delayMotor = 1;
	pwmVal_L_ori = speed;
	pwmVal_R_ori = speed*turingRatio_R;
}
void moveCarForwardLeft(double angle, int speed)
{
	int distance_R = outerTurnRadius90Deg_L * angle / 90;
	int distance_L = turingRatio_L * distance_R;
	dist_to_travel_L = multiplierConstant * distance_L;
	dist_to_travel_R = multiplierConstant * distance_R;
	motorDir = 2;
	motorAngle = 59;
	reAlignWheel = 0;
	turningLeft = 0;
	delayMotor = 1;
	pwmVal_L_ori = speed*turingRatio_L;
	pwmVal_R_ori = speed;
}
void moveCarBackwardRight(double angle, int speed)
{
	int distance_L = outerTurnRadius90Deg_R_back * angle / 90;
	int distance_R = turingRatio_R * distance_L;
	dist_to_travel_L = multiplierConstant * distance_L;
	dist_to_travel_R = multiplierConstant * distance_R;
	motorDir = 1;
	motorAngle = 89;
	reAlignWheel = 0;
	turningLeft = 1;
	delayMotor = 1;
	pwmVal_L_ori = speed;
	pwmVal_R_ori = speed*turingRatio_R;
}
void moveCarBackwardLeft(double angle, int speed)
{
	int distance_R = outerTurnRadius90Deg_L_backLeft * angle / 90;
	int distance_L = turingRatio_L * distance_R;
	dist_to_travel_L = multiplierConstant * distance_L;
	dist_to_travel_R = multiplierConstant * distance_R;
	motorDir = 1;
	motorAngle = 59;
	reAlignWheel = 1;
	turningLeft = 0;
	delayMotor = 1;
	pwmVal_L_ori = speed*turingRatio_L;
	pwmVal_R_ori = speed;
}

void moveCarForwardRight_s(double angle, int speed)
{
	int distance_L = outerTurnRadius90Deg_R * angle / 90;
	int distance_R = distance_L * turingRatio_R;
	dist_to_travel_L = multiplierConstant * distance_L;
	dist_to_travel_R = multiplierConstant * distance_R;
	motorDir = 2;
	motorAngle = 80;
	reAlignWheel = 1;
	turningLeft = 1;
	delayMotor = 1;
	pwmVal_L_ori = speed;
	//pwmVal_R_ori = speed*turingRatio_R;
	pwmVal_R_ori = 0;
}
void moveCarForwardLeft_s(double angle, int speed)
{
	int distance_R = outerTurnRadius90Deg_L * angle / 90;
	int distance_L = turingRatio_L * distance_R;
	dist_to_travel_L = multiplierConstant * distance_L;
	dist_to_travel_R = multiplierConstant * distance_R;
	motorDir = 2;
	motorAngle = 68;
	reAlignWheel = 1;
	turningLeft = 0;
	delayMotor = 1;
	//pwmVal_L_ori = speed*turingRatio_L;
	pwmVal_L_ori = 0;
	pwmVal_R_ori = speed;
}
void moveCarForwardRight_u(double angle, int speed)
{
	int distance_L = outerTurnRadius90Deg_R * angle / 90;
	int distance_R = distance_L * turingRatio_R;
	dist_to_travel_L = multiplierConstant * distance_L;
	dist_to_travel_R = multiplierConstant * distance_R;
	motorDir = 2;
	motorAngle = 85;
	reAlignWheel = 1;
	turningLeft = 1;
	delayMotor = 1;
	//pwmVal_L_ori = speed;
	pwmVal_R_ori = speed*turingRatio_R;
	pwmVal_R_ori = 0;
}
void moveCarForwardLeft_u(double angle, int speed)
{
	int distance_R = outerTurnRadius90Deg_L * angle / 90;
	int distance_L = turingRatio_L * distance_R;
	dist_to_travel_L = multiplierConstant * distance_L;
	dist_to_travel_R = multiplierConstant * distance_R;
	motorDir = 2;
	motorAngle = 66;
	reAlignWheel = 1;
	turningLeft = 0;
	delayMotor = 1;
	pwmVal_L_ori = speed*turingRatio_L;
	//pwmVal_L_ori = 0;
	pwmVal_R_ori = speed;
}
void moveCarBackwardRight_s(double angle, int speed)
{
	int distance_L = outerTurnRadius90Deg_R * angle / 90;
	int distance_R = turingRatio_R * distance_L;
	dist_to_travel_L = multiplierConstant * distance_L;
	dist_to_travel_R = multiplierConstant * distance_R;
	motorDir = 1;
	motorAngle = 82;
	reAlignWheel = 1;
	turningLeft = 1;
	delayMotor = 1;
	pwmVal_L_ori = speed;
	pwmVal_R_ori = speed*turingRatio_L;
	//pwmVal_R_ori = 0;
}
void moveCarBackwardLeft_s(double angle, int speed)
{
	int distance_R = outerTurnRadius90Deg_L * angle / 90;
	int distance_L = turingRatio_L * distance_R;
	dist_to_travel_L = multiplierConstant * distance_L;
	dist_to_travel_R = multiplierConstant * distance_R;
	motorDir = 1;
	motorAngle = 69;
	reAlignWheel = 0;
	turningLeft = 0;
	delayMotor = 1;
	pwmVal_L_ori = speed*turingRatio_L;
	//pwmVal_L_ori = 0;
	pwmVal_R_ori = speed;
}
// the pid formula, still needs fine tuning
int PID_R(int current_error, int previous_error,int integration_sum)
{
	double Kp = 0.175;
	double Ki = 0.07; //= 0.2;
	double Kd = 0.07; //= 0.15;
	return (Kp * current_error) + (Ki * integration_sum * 0.1) + (Kd * (current_error - previous_error) / 0.1);
}
int PID_L(int current_error, int previous_error,int integration_sum)
{
	double Kp = 0.18;
	double Ki = 0.07; //= 0.2;
	double Kd = 0.07; //= 0.15;
	return (Kp * current_error) + (Ki * integration_sum * 0.1) + (Kd * (current_error - previous_error) / 0.1);
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	/*Prevent unused argument(s) compilation warning */
//	//uint8_t uartTest[6] = "RPI|R\n\0";
//	uint8_t uartTest[6] = "ALG|R\n\0";
//	UNUSED(huart);
//	if(prevCommand == ' '){
//		motorReady = 0;
//		if(aRxBuffer[0] == 'a'){
//			moveCarForward(5,1500);
//		}
//		if(aRxBuffer[0] == 'b'){
//			moveCarForward(10,1500);
//			//uint8_t uartTest[6] = "ALG|R\n\0";
//			//sprintf(uartTest, "ALG|R\n\0");
//			//memset()
//			 //HAL_UART_Transmit(&huart3, uartTest,sizeof('a')*7,0xFFFF);
//		}
//		if(aRxBuffer[0] == 'c'){
//			moveCarForwardLeft(5,1500);
//		}
//		if(aRxBuffer[0] == 'd'){
//			moveCarForwardRight(5,1500);
//		}
//		if(aRxBuffer[0] == 'e'){
//			moveCarBackwards(5,1500);
//		}
//		if(aRxBuffer[0] == 'f'){
//			moveCarBackwards(10,1500);
//		}
//		if(aRxBuffer[0] == 'g'){
//			moveCarForwardLeft(45,1500);
//		}
//		if(aRxBuffer[0] == 'h'){
//			moveCarForwardRight(45,1500);
//		}
//		if(aRxBuffer[0] == 'i'){
//			moveCarForwardLeft(90,1500);
//			motorReady = 0;
//			enqueue('P');
//		}
//		if(aRxBuffer[0] == 'j'){
//			moveCarForwardRight(90,1500);
//			motorReady = 0;
//			enqueue('P');
//	//		while(motorReady == 0);
//	//		moveCarForward(5,1500);
//		}
//		if(aRxBuffer[0] == 'k'){
//			moveCarForward(100,1500);
//		}
//		if(aRxBuffer[0] == 'l'){
//			moveCarForwardRight(60,1500);
//		}
//		if(aRxBuffer[0] == 'm'){
//			moveCarForwardRight(30,1500);
//		}
//		if(aRxBuffer[0] == 'n'){
//		  flag_Calibration = 1;
//		}
//		if(aRxBuffer[0] == 'o'){	// NextSide Routine
//		  flag_NextSide = 1;
//	//	  for(int i = 0; i<2; i++){
//	//		  while(motorRunning_L == 1 || motorRunning_R == 1);
//	//		  if(i == 0){
//	//			  moveCarForwardRight(90,1500);
//	//		  }
//	//		  if(i == 1){
//	//			  moveCarForwardLeft(90,1500);
//	//		  }
//	//	  }
//		}
//		if(aRxBuffer[0] == 'v'){
//		  //flag_Calibration = 1;
//			imageFound = 0;
//		}
//		if(aRxBuffer[0] == 'w'){
//		  //flag_Calibration = 1;
//		}
//
//		if(aRxBuffer[0] == 'x'){
//			startA5 = 1;
//		  //flag_Calibration = 1;
//		}
//		if(aRxBuffer[0] == 'y'){
//			moveCarForwardRight(45,1500);
//			motorReady = 0;
//			enqueue('Y');
//		  //flag_Calibration = 1;
//		}
//		if(aRxBuffer[0] == 'z'){
//			moveCarForwardLeft(45,1500);
//			motorReady = 0;
//			enqueue('Z');
//		  //flag_Calibration = 1;
//		}
//		if(aRxBuffer[0] == 'A'){
//			moveCarBackwardRight(90,1500);
//		  //flag_Calibration = 1;
//		}
//		if(aRxBuffer[0] == 'B'){
//			moveCarBackwardLeft(90,1500);
//		  //flag_Calibration = 1;
//		}
//		readyFlag = readyFlag == 0;
//		// if any of these characters are received, uart will receive 3 more characters to set the distance/angle to travel
//		if(aRxBuffer[0] == 'p' || aRxBuffer[0] == 'q' || aRxBuffer[0] == 'r')
//		{
//			prevCommand = aRxBuffer[0];
//			HAL_UART_Receive_IT (&huart3, aRxBuffer, 3);
//
//		}
//	}
//	else{
//	  if(prevCommand == 'p'){
//		  int tempDis;
//		  tempDis = 100 * (aRxBuffer[0] - '0') + 10 *  (aRxBuffer[1] - '0') +  (aRxBuffer[2] - '0');
//		moveCarForward(tempDis,2000);
//	  }
//	  if(prevCommand == 'q'){
//		  int tempDis;
//		  tempDis = 100 * (aRxBuffer[0] - '0') + 10 *  (aRxBuffer[1] - '0') +  (aRxBuffer[2] - '0');
//		moveCarForwardLeft(tempDis,2000);
//	  }
//	  if(prevCommand == 'r'){
//		  int tempDis;
//		  tempDis = 100 * (aRxBuffer[0] - '0') + 10 *  (aRxBuffer[1] - '0') +  (aRxBuffer[2] - '0');
//		moveCarForwardRight(tempDis,2000);
//	  }
//	  prevCommand = ' ';
//	}
//	// check if the code is receiving the distance or angle
//	if(prevCommand == ' ')
//		HAL_UART_Receive_IT (&huart3, aRxBuffer, 1);
//	readySent = 0;
//	//HAL_UART_Transmit(&huart3, uartTest,sizeof('a')*7,0xFFFF);
//}
char tempBuffer[4];
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/*Prevent unused argument(s) compilation warning */
	//uint8_t uartTest[6] = "RPI|R\n\0";
	UNUSED(huart);
	HAL_UART_Transmit(&huart3, aRxBuffer,20,0xFFFF);
	waitingForFirstCommand = 0;
	if(tempBuffer[0] == 'p' || tempBuffer[0] == 'q' || tempBuffer[0] == 'r' || tempBuffer[0] == 's')
	{
		tempBuffer[1] = aRxBuffer[0];
		tempBuffer[2] = aRxBuffer[1];
		tempBuffer[3] = aRxBuffer[2];
		enqueue(tempBuffer);
		tempBuffer[0] = tempBuffer[1] = tempBuffer[2] = tempBuffer[3] = "\0";
		//aRxBuffer[0] = aRxBuffer[1] = aRxBuffer[2] = aRxBuffer[3] = "\0";
		HAL_UART_Receive_IT (&huart3, aRxBuffer, 1);
	}
	else
	{
		if(aRxBuffer[0] == 'p' || aRxBuffer[0] == 'q' || aRxBuffer[0] == 'r' || aRxBuffer[0] == 's')
		{
			tempBuffer[0] = aRxBuffer[0];
			HAL_UART_Receive_IT (&huart3, aRxBuffer, 3);
		}
		else
		{

			if(aRxBuffer[0] == 'j' || aRxBuffer[0] == 'i' )
			{
				enqueue(aRxBuffer);
				enqueue("a");
			}else
			if(aRxBuffer[0] == 'A' )
			{
				enqueue("e");
				enqueue(aRxBuffer);
				enqueue("H");
			}
			else if(aRxBuffer[0] == 'B' )
			{
				enqueue("I");
				enqueue(aRxBuffer);
			}
			else if(aRxBuffer[0] == 'C' )
			{
				enqueue("p002");
				enqueue(aRxBuffer);
				enqueue("D");
				enqueue("C");
				enqueue("D");
				enqueue("C");
				enqueue("s010");
			}
			else if(aRxBuffer[0] == 'E' )
			{
				enqueue("p001");
				enqueue(aRxBuffer);
				enqueue("F");
				enqueue("E");
				enqueue("F");
				enqueue("E");
				enqueue("s010");
			}else
			{

				enqueue(aRxBuffer);
			}
			HAL_UART_Receive_IT (&huart3, aRxBuffer, 1);
		}
	}
	readySent = 0;
	//HAL_UART_Transmit(&huart3, uartTest,sizeof('a')*7,0xFFFF);
}

float frequency = 0;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1 && htim->Instance == htim4.Instance)  // if the interrupt source is channel1
	{
		if (Is_First_Captured==0) // if the first value is not captured
		{
			IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
			Is_First_Captured = 1;  // set the first captured as true
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else   // if the first is already captured
		{
			IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value
			if (IC_Val2 > IC_Val1)
			{
				Difference = IC_Val2-IC_Val1;
			}

			else if (IC_Val1 > IC_Val2)
			{
				Difference = (65534 - IC_Val1) + IC_Val2;
			}

			Distance = Difference * 0.034 / 2.0;
			ultraSensorCount += 1;
			Is_First_Captured = 0;

			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
//			HAL_TIM_IC_Stop_IT(&htim4, TIM_IT_CC1);
			ultraSonicReady = 1;
//			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter
//			Is_First_Captured = 0; // set it back to false
		}
	}
}
void week9()
{
	int ninetyDegreeTurnDelay = 3000;
	moveCarForward(200,2000);
	week9InitialTravelFlag = 1;
	// refers to ultrasonic distance
	while(Distance > 30)osDelay(1);
	moveCarForwardRight(90,2100);
	osDelay(ninetyDegreeTurnDelay);
	moveCarForward(60,2000);
	// while IR can detect a wall on th left
	week9RightTurnTravelFlag = 1;
	while(distance_IR_L > 350)osDelay(1);
	moveCarForwardLeft(90,2000);
	osDelay(ninetyDegreeTurnDelay);
	moveCarForwardLeft(90,2000);
	osDelay(ninetyDegreeTurnDelay);
	moveCarForward(60,2000);
	osDelay(5000);
	moveCarForwardLeft(90,2000);
	osDelay(ninetyDegreeTurnDelay);
	moveCarForwardLeft(90,2000);
	osDelay(ninetyDegreeTurnDelay);
	moveCarForward(week9RightTurnTravel - 40,2000);
	osDelay(4000);
	moveCarForwardRight(90,2000);
	osDelay(ninetyDegreeTurnDelay);
	moveCarForward(week9InitialTravel-20,2000);
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
	uint8_t uartChars[20];
	char dequeuedVal[4];
//	int constSpeedStraight = 2300;
//	int constSpeed = 2000;
//	int constSpeedTurn = 2100;
	int constSpeedStraight = 3200;
	int constSpeed = 2000;
	int constSpeedTurn = 3000;
	  //HCSR04_Read();
	  for(;;)
	  {
		  while(waitingForFirstCommand == 1) osDelay(1);
		  if(motorRunning_R == 0 && motorRunning_L == 0
			  && changeInitialVal_R == 0 && changeInitialVal_L == 0
			  && motorReady == 0 && delayMotor == 0)
		  {
			  motorReady = 1;
			  osDelay(1);
		  }
		  if(motorReady == 1 )
		  {
			  if(Front != - 1 && Front <= Rear){
				 motorReady = 0	;
				  //dequeuedVal = dequeue();
				  strncpy(dequeuedVal,dequeue(), 4);
//				  //sprintf(display_RMotor, "M_R:%5d..%1d\0",diff_R,dir_R);
//				  //OLED_ShowString(0,10,dequeuedVal);
//				  //HAL_UART_Transmit(&huart3, &dequeuedVal,1,0xFFFF);
//				  if(dequeuedVal[0] == '1')
//				  {
//					  //moveCarForwardLeft_s(5, 800);
//					  moveCarForwardRight_s(4, 1800);
//				  }
//				  if(dequeuedVal[0] == 'L')
//				  {
//					  //moveCarForwardRight_s(5, 800);
//					  moveCarForwardLeft_s(4, 1800);
//				  }
//				  if(dequeuedVal[0] == '3')
//				  {
//					  //moveCarForwardLeft_s(5, 800);
//					  //moveCarBackwardRight_s(12, 2300);
//					  moveCarBackwards(8, 2150);
//					  //moveCarBackwardRight_s(9, 2300);
//				  }
//				  if(dequeuedVal[0] == '4')
//				  {
//					  //moveCarForwardRight_s(5, 800);
//					  //moveCarBackwardLeft_s(12, 2300);
//					  moveCarBackwards(8, 2150);
//					  //moveCarBackwardL_s(9, 2300);
//				  }
//				  if(dequeuedVal[0] == '5')
//				  {
//					  moveCarForward(2, 550);
//				  }
//				  if(dequeuedVal[0] == '6')	//for endstate
//				  {
//					  //moveCarBackwards(5, 600);
//					  moveCarBackwards(20 - distance_Acquired/10, 1000);
//				  }
//				  if(dequeuedVal[0] == '7')	//for endstate
//				  {
//					  moveCarForward(distance_Acquired/10 - 20, 1000);
//				  }
//				  if(dequeuedVal[0] == '8')
//				  {
//					  moveCarForwardRight_u(9, 1800);
//				  }
//				  if(dequeuedVal[0] == '9')
//				  {
//					  moveCarForwardLeft_u(9, 1800);
//				  }
//
//
//
//				  if(dequeuedVal[0] == 'P')
//				  {
//					  moveCarForward(6,1500);
//				  }
//				  if(dequeuedVal[0] == 'Y')
//				  {
//					  moveCarForwardLeft(45,1500);
//				  }
//				  if(dequeuedVal[0] == 'Z')
//				  {
//					  moveCarForwardRight(45,1500);
//				  }
					if(dequeuedVal[0] == 'a'){
						moveCarForward(5,constSpeedStraight);
					}
//					if(dequeuedVal[0] == 'b'){
//						moveCarForward(10,constSpeedStraight);
//					}
//					if(dequeuedVal[0] == 'c'){
//						moveCarForwardLeft(5,constSpeed);
//					}
//					if(dequeuedVal[0] == 'd'){
//						moveCarForwardRight(5,constSpeed);
//					}
//					if(dequeuedVal[0] == 'e'){
//						moveCarBackwards(5,constSpeed);
//					}
//					if(dequeuedVal[0] == 'f'){
//						moveCarBackwards(10,constSpeed);
//					}
					if(dequeuedVal[0] == 'g'){
						moveCarForwardLeft(45,constSpeedTurn);
					}
					if(dequeuedVal[0] == 'h'){
						moveCarForwardRight(45,constSpeedTurn);
					}
					if(dequeuedVal[0] == 'i'){
						moveCarForwardLeft(90,constSpeedTurn);
					}
					if(dequeuedVal[0] == 'j'){
						moveCarForwardRight(90,constSpeedTurn);
					}
//					if(dequeuedVal[0] == 'k'){
//						moveCarForward(100,constSpeed);
//					}
//					if(dequeuedVal[0] == 'l'){
//						moveCarForwardRight(60,constSpeed);
//					}
//					if(dequeuedVal[0] == 'm'){
//						moveCarForwardRight(30,constSpeed);
//					}
//					if(dequeuedVal[0] == 'n'){
//					  flag_Calibration = 1;
//					}
//					if(dequeuedVal[0] == 'y'){
//						moveCarForwardRight(45,constSpeed);
//					}
//					if(dequeuedVal[0] == 'z'){
//						moveCarForwardLeft(45,constSpeed);
//					}
//					if(dequeuedVal[0] == 'A'){
//						moveCarBackwardRight(90,constSpeed);
//					}
//					if(dequeuedVal[0] == 'B'){
//						moveCarBackwardLeft(90,constSpeed);
//					}
//					if(dequeuedVal[0] == 'C'){
//						moveCarForwardRight(18,constSpeed);
//					}
//					if(dequeuedVal[0] == 'D'){
//						moveCarBackwardLeft(18,constSpeed);
//					}
//					if(dequeuedVal[0] == 'E'){
//						moveCarForwardLeft(18,constSpeed);
//					}
//					if(dequeuedVal[0] == 'F'){
//						moveCarBackwardRight(18,constSpeed);
//					}
//					if(dequeuedVal[0] == 'H'){
//						moveCarBackwards(1,constSpeedStraight);
//					}
//					if(dequeuedVal[0] == 'I'){
//						moveCarBackwards(3,constSpeedStraight);
//					}
//					if(dequeuedVal[0] == 'J'){
//						moveCarForward(1,constSpeedStraight);
//					}
					// all for week 9
					if(dequeuedVal[0] == 'G'){
						moveCarForward(200,constSpeedStraight);
						week9InitialTravelFlag = 1;
						week9InitialTravelMovementFlag = 1;
					}
					if(dequeuedVal[0] == 'K'){
						week9RightTurnTravelFlag = 1;
						week9DetectLeftWall = 1;
						moveCarForward(60,constSpeedStraight);
					}
					if(dequeuedVal[0] == 'Z'){
						moveCarForward(200,constSpeedStraight);
						week9returningFlag = 1;
					}
				  if(dequeuedVal[0] == 'p'){
					  int tempDis;
					  tempDis = 100 * (dequeuedVal[1] - '0') + 10 *  (dequeuedVal[2] - '0') +  (dequeuedVal[3] - '0');
					moveCarForward(tempDis,constSpeedStraight);
				  }
				  if(dequeuedVal[0] == 'q'){
					  int tempDis;
					  tempDis = 100 * (dequeuedVal[1] - '0') + 10 *  (dequeuedVal[2] - '0') +  (dequeuedVal[3] - '0');
					moveCarForwardLeft(tempDis,constSpeedTurn);
				  }
				  if(dequeuedVal[0] == 'r'){
					  int tempDis;
					  tempDis = 100 * (dequeuedVal[1] - '0') + 10 *  (dequeuedVal[2] - '0') +  (dequeuedVal[3] - '0');
					moveCarForwardRight(tempDis,constSpeedTurn);
				  }
				  if(dequeuedVal[0] == 's'){
					  int tempDis;
					  tempDis = 100 * (dequeuedVal[1] - '0') + 10 *  (dequeuedVal[2] - '0') +  (dequeuedVal[3] - '0');
					moveCarBackwards(tempDis,constSpeedStraight);
				  }
				}
			  else
			  {
				  //osDelay(2000);
				  if(readySent == 0 && flag_Calibration == 0)
				  {
//					  if(readyFlag == 0)
//					  {
//						  HAL_UART_Transmit(&huart3, sendAlgo_ready,7,0xFFFF);
//					  }
//					  else
//					  {
//						  HAL_UART_Transmit(&huart3, sendAlgo_Ready,7,0xFFFF);
//					  }
					  readySent = 1;
				  }

			  }
		  }
		  osDelay(100);
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
	  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
  /* Infinite loop */
  for(;;)
  {
	  if(delayMotor == 1)
	  {
		  htim1.Instance->CCR4 = motorAngle;
		  osDelay(300);
		  if(reAlignWheelForward == 1)
		  {
			  htim1.Instance->CCR4 = 74;
			  osDelay(100);
			  htim1.Instance->CCR4 = 71;
			  osDelay(100);
			  htim1.Instance->CCR4 = 72;
			  reAlignWheelForward = 0;
		  }
		  if(reAlignWheelBackward == 1)
		  {
			  htim1.Instance->CCR4 = 74;
			  osDelay(100);
			  htim1.Instance->CCR4 = 71;
			  osDelay(100);
			  htim1.Instance->CCR4 = 72;
			  reAlignWheelBackward = 0;
		  }
		  osDelay(50);
		  changeInitialVal_R = 1;
		  changeInitialVal_L = 1;
		  delayMotor = 0;
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

/* USER CODE END Header_the_RightEncoder */
void the_RightEncoder(void *argument)
{
  /* USER CODE BEGIN the_RightEncoder */
	HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
	int cnt1_R = 0;
	int cnt2_R = 0;
	int cnt1_L = 0;
	int cnt2_L = 0;
	// tick2 was meant for correcting misaligned direction
	uint32_t tick = 0;
	// stores the counter val to be used by all the other lines of codes
//	int tempCounterVal_R;
//	int tempCounterVal_L;
	// set the initial distance counter val to know how far the car has traveled
//	int initialDistTempVal_R = 0;
//	int initialDistTempVal_L = 0;
	// used for PID
	int current_error_R = 0;
	int integration_sum_R  = 0;
	int previous_error_R = 0;
	int current_error_L = 0;
	int integration_sum_L  = 0;
	int previous_error_L = 0;
	// used to convert the speed from per 100ms to per sec
	int speedPerSec_R = 0;
	int speedPerSec_L = 0;
	// to store the pid adjustment val to modify the pid value
	float pidAdjustment_R = 0;
	float pidAdjustment_L = 0;
	uint8_t encoderUart[20];
  /* Infinite loop */
  for(;;)
  {
	  tempCounterVal_R = __HAL_TIM_GET_COUNTER(&htim2);
	  tempCounterVal_L = __HAL_TIM_GET_COUNTER(&htim3);
		if(changeInitialVal_R == 1)
		{
			// resets all the initial values
			initialDistTempVal_R = tempCounterVal_R;
			integration_sum_R = 0;
			previous_error_R = 0;
			initialDistTempVal_L = tempCounterVal_L;
			integration_sum_L = 0;
			previous_error_L = 0;
			if(withPid == 0)
			{
				pwmVal_R = pwmVal_R_ori;
				pwmVal_L = pwmVal_L_ori;
			}
			else
			{
				pwmVal_R = 0;
				pwmVal_L = 0;
			}
			motorRunning_R = 1;
			motorRunning_L = 1;
			tick = HAL_GetTick();
			changeInitialVal_R = 0;
			changeInitialVal_L = 0;
		}
		if(motorRunning_R == 1 || motorRunning_L == 1){
			if(HAL_GetTick()-tick > 100L){	// unable to display properly below 1000L
				cnt2_R = tempCounterVal_R;
				cnt2_L = tempCounterVal_L;
				if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2)){
					if(cnt2_R<cnt1_R)
						diff_R = cnt1_R - cnt2_R;
					else
						diff_R = (65535 - cnt2_R)+cnt1_R;
				}
				else{
					if(cnt2_R>cnt1_R)
						diff_R = cnt2_R - cnt1_R;
					else
						diff_R = (65535 - cnt1_R)+cnt2_R;
				}
				if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3)){
					if(cnt2_L<cnt1_L)
						diff_L = cnt1_L - cnt2_L;
					else
						diff_L = (65535 - cnt2_L)+cnt1_L;
				}
				else{
					if(cnt2_L>cnt1_L)
						diff_L = cnt2_L - cnt1_L;
					else
						diff_L = (65535 - cnt1_L)+cnt2_L;
				}
				dir_R = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2);
				dir_L = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3);
				cnt1_R = tempCounterVal_R;
				cnt1_L = tempCounterVal_L;
				tick = HAL_GetTick();
				// PID algo, currently still causes robot to deviate
//				speedPerSec_R = diff_R * 10;
//				if(speedPerSec_R > 10000) speedPerSec_R = 0;
//				if(withPid == 1)
//					if(motorRunning_R == 1){
//					//if(1 == 0){
//					//if(speedPerSec > pwmVal_R_ori - (pwmVal_R_ori * 0.8) && speedPerSec < pwmVal_R_ori + (pwmVal_R_ori * 0.8) && delayMotor == 0){
//						current_error_R = pwmVal_R_ori - speedPerSec_R;
//						integration_sum_R = integration_sum_R + current_error_R ;
//						pidAdjustment_R = PID_R(current_error_R,previous_error_R,integration_sum_R);
//						pwmVal_R = pwmVal_R + pidAdjustment_R;
//						previous_error_R = current_error_R;
//					}
//				speedPerSec_L = diff_L * 10;
//				if(speedPerSec_L > 10000) speedPerSec_L = 0;
//				if(withPid == 1)
//					if(motorRunning_L == 1){
//						current_error_L = pwmVal_L_ori - speedPerSec_L;
//						integration_sum_L = integration_sum_L + current_error_L ;
//						pidAdjustment_L = PID_L(current_error_L,previous_error_L,integration_sum_L);
//						pwmVal_L = pwmVal_L + pidAdjustment_L;
//						previous_error_L = current_error_L;
//					}
			}
			//week9
//			if(week9InitialTravelFlag == 1 || week9RightTurnTravelFlag == 1)
//			{
//				if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2)){
//					if(initialDistTempVal_R > tempCounterVal_R){
//						if(week9InitialTravelFlag == 1)
//						{
//							week9InitialTravel = initialDistTempVal_R - tempCounterVal_R;
//							//week9InitialTravelFlag = 0;
//						}
//						if(week9RightTurnTravelFlag == 1)
//						{
//							week9RightTurnTravel = initialDistTempVal_R - tempCounterVal_R;
//							//week9RightTurnTravelFlag = 0;
//						}
//					}
//					else
//					{
//						if(week9InitialTravelFlag == 1){
//							week9InitialTravel = 65535 - tempCounterVal_R + initialDistTempVal_R;
//							//week9InitialTravelFlag = 0;
//						}
//						if(week9RightTurnTravelFlag == 1){
//							week9RightTurnTravel = 65535 - tempCounterVal_R + initialDistTempVal_R;
//							//week9RightTurnTravelFlag = 0;
//						}
//					}
//				}else{
//					if(initialDistTempVal_R < tempCounterVal_R){
//						if(week9InitialTravelFlag == 1){
//							week9InitialTravel = tempCounterVal_R - initialDistTempVal_R;
//							//week9InitialTravelFlag = 0;
//						}
//						if(week9RightTurnTravelFlag == 1){
//							week9RightTurnTravel = tempCounterVal_R - initialDistTempVal_R;
//							//week9RightTurnTravelFlag = 0;
//						}
//					}
//					else
//					{
//						if(week9InitialTravelFlag == 1){
//							week9InitialTravel = 65535 - initialDistTempVal_R + tempCounterVal_R;
//							//week9InitialTravelFlag = 0;
//						}
//						if(week9RightTurnTravelFlag == 1){
//							week9RightTurnTravel = 65535 - initialDistTempVal_R + tempCounterVal_R;
//							//week9RightTurnTravelFlag = 0;
//						}
//					}
//				}
//				week9InitialTravel = week9InitialTravel / multiplierConstant;
//				week9RightTurnTravel = week9RightTurnTravel / multiplierConstant;
//			}
			if(turningLeft == 0){
				// count the distance travelled and stops the car when the dist is req to required
				if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2)){
					if(initialDistTempVal_R >= tempCounterVal_R){
						if(initialDistTempVal_R - tempCounterVal_R  > dist_to_travel_R  && initialDistTempVal_R - tempCounterVal_R < 20000){
							motor_stop();
						}
					}
					else
					{
						if(65535 - tempCounterVal_R + initialDistTempVal_R > dist_to_travel_R   && 65535 - tempCounterVal_R + initialDistTempVal_R < 20000){
							motor_stop();
						}
					}
				}else{
					if(initialDistTempVal_R <= tempCounterVal_R){
						if(tempCounterVal_R - initialDistTempVal_R > dist_to_travel_R  && tempCounterVal_R - initialDistTempVal_R - tempCounterVal_R < 20000){
							motor_stop();
						}
					}
					else
					{
						if(65535 - initialDistTempVal_R + tempCounterVal_R > dist_to_travel_R   && 65535 - initialDistTempVal_R + tempCounterVal_R < 20000){
							motor_stop();
						}
					}
				}
			}
			else
			{
				if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3)){
					if(initialDistTempVal_L >= tempCounterVal_L){
						if(initialDistTempVal_L - tempCounterVal_L  > dist_to_travel_L && initialDistTempVal_L - tempCounterVal_L < 20000){
							motor_stop();
						}
					}
					else
					{
						if(65535 - tempCounterVal_L + initialDistTempVal_L> dist_to_travel_L  && 65535 - tempCounterVal_L + initialDistTempVal_L < 20000){
							motor_stop();
						}
					}
				}else
				{
					if(initialDistTempVal_L <= tempCounterVal_L){
						if( tempCounterVal_L  - initialDistTempVal_L > dist_to_travel_L  && tempCounterVal_L  - initialDistTempVal_L < 20000){
							motor_stop();
						}
					}
					else
					{
						if(65535 - initialDistTempVal_L + tempCounterVal_L > dist_to_travel_L  && 65535 - initialDistTempVal_L + tempCounterVal_L < 20000){
							motor_stop();
						}
					}

				}
			}
		}
	    //osDelay(1);
  }
  /* USER CODE END the_RightEncoder */
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
//	uint8_t display_LMotor[30];
//	uint8_t display_Dist[15];
	  uint8_t display_DistR[15];
//	  uint8_t display_DistL[15];
//	  uint8_t display_DistState[15];
//	  uint8_t display_Actions[15];
  for(;;)
  {
	  sprintf(display_Rx,"dtt_L:%d\0",dist_to_travel_R);
	  OLED_ShowString(0,0,display_Rx);

//		sprintf(ultraLed1,"Distance:%d         \0",Distance);
//		//ultraSensorCount = 0;
//		OLED_ShowString(0,30,ultraLed1);
//		sprintf(ultraLed1,"U_Count:%d         \0",ultraSensorCount);
//		OLED_ShowString(0,0,ultraLed1);
//		sprintf(ultraLed1,"U1:%d    \0",IC_Val1);
//		OLED_ShowString(0,40,ultraLed1);
//		sprintf(ultraLed1,"U2:%d    \0",IC_Val2);
//		OLED_ShowString(0,50,ultraLed1);
//		  sprintf(display_RMotor, "M_R:%5d..%1d\0",diff_R,dir_R);
//		  OLED_ShowString(0,10,display_RMotor);
//		  sprintf(display_RMotor, "M_L:%5d..%1d\0",diff_L,dir_L);
//	  OLED_ShowString(0,20,display_RMotor);
	  sprintf(display_RMotor, "init_L:%d\0", initialDistTempVal_L);
	  OLED_ShowString(0,10,display_RMotor);
	  sprintf(display_RMotor, "tcv_L:%d\0",tempCounterVal_L);
  OLED_ShowString(0,20,display_RMotor);
//		if(dir_L == 0)	dir_L == 1;
//		else dir_L == 0;
//	  sprintf(display_LMotor, "%1d : %1d\0",(int)dist_to_travel_L,(int)dist_to_travel_R);
//	  OLED_ShowString(0,30,display_LMotor);
//	  int tempInt1 = pwmVal_R_ori;
//	  float tmpFrac = pwmVal_R_ori - tempInt1;      // Get fraction (0.0123).
//	  int tempInt2 = trunc(tmpFrac * 10000);
//	  sprintf (display_Dist, "%d.%04d\n", tempInt1, tempInt2);
//	  sprintf(display_Dist, "%10f\0",pwmVal_R);
//	  OLED_ShowString(0,30,display_Dist);
	  sprintf(display_DistR, "dtt_R: %d\0",dist_to_travel_L);
//	  sprintf(display_DistL, "d_L: %5d\0",distance_IR_L);
////		OLED_ShowString(0,20,display_Actions);
////		OLED_ShowString(0,30,display_DistState);
		OLED_ShowString(0,40,display_DistR);
//		OLED_ShowString(0,50,display_DistL);
	  OLED_Refresh_Gram();
    osDelay(1);
  }
  /* USER CODE END the_OLED */
}

/* USER CODE BEGIN Header_StartTask08 */
/**
* @brief Function implementing the ultrasonicSenso thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask08 */


void enqueue(char item[])
{
    //int insert_item;
    if (Rear == 8000 - 1)
       printf("Overflow \n");
    else
    {
        if (Front == - 1)

        Front = 0;
        printf("Element to be inserted in the Queue\n : ");
        //scanf("%d", &insert_item);
        Rear = Rear + 1;
        strncpy(action_queue[Rear], item, 4);
        //action_queue[Rear] = item;
    }
}
void motor_stop()
{
	pwmVal_R = 0;
	motorRunning_R = 0;
	pwmVal_L = 0;
	motorRunning_L = 0;
}
char* dequeue()
{
    if (Front == - 1 || Front > Rear)
    {
        printf("Underflow \n");
        return ;
    }
    else
    {
        Front = Front + 1;
    	return action_queue[Front - 1];
        //printf("Element deleted from the Queue: %d\n", ]);
    }
}
void StartTask08(void *argument)
{
  /* USER CODE BEGIN StartTask08 */
  /* Infinite loop */
	  //HAL_UART_Transmit(&huart3, sendAlgo_found,7,0xFFFF);
	char tempweek9command[4];
	int triggerCount = 0;
	int detectionCount = 0;
	int reqDetectionCount = 1;
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);

  for(;;)
  {
//		cnt1 = __HAL_TIM_GET_COUNTER(&htim4);
//		sprintf(ultraLed,"U:%d\0",cnt1);
//		OLED_ShowString(0,0,ultraLed);
//	  osDelay();
	  if(ultraSonicReady == 1 && triggerCount >= 0){
		  ultraSonicReady = 0;

			__HAL_TIM_SET_COUNTER(&htim4, 0);  // reset the counter
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);  // pull the TRIG pin HIGH
		osDelay(1);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);  // pull the TRIG pin low
//		HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
		triggerCount += 1;
	  }
	  if(week9InitialTravelMovementFlag == 1 && delayMotor == 0)
	  {
		  if(Distance < 55)
		  {
			  detectionCount += 1;
			  if(detectionCount == reqDetectionCount){
				week9InitialTravelFlag = 0;
				week9InitialTravelMovementFlag = 0;
				motor_stop();
				enqueue("h");
				enqueue("p059");
				enqueue("g");
				enqueue("p000");
				enqueue("i");
				enqueue("p060");
				enqueue("i");
				enqueue("p005");
				enqueue("g");
				enqueue("p059");
				enqueue("h");
				enqueue("Z");
				detectionCount = 0;
			  }
		  }
		  else
		  {
			  detectionCount = 0;
		  }
	  }
	  if(week9returningFlag == 1 && delayMotor == 0)
	  {
		  if(Distance < 22)
		  {
			  detectionCount += 1;
			  if(detectionCount == reqDetectionCount){
				  motor_stop();
				  week9returningFlag = 0;
				  HAL_UART_Transmit(&huart3, sendAND_End,sizeof('a')*11,0xFFFF);
				  detectionCount = 0;
			  }
		  }
		  else
		  {
			  detectionCount = 0;
		  }
	  }
	  osDelay(100);
		//osDelay(1000);
  }
  /* USER CODE END StartTask08 */
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
