/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum {
 MOTOR_STATE_STOP = 0, // Explicitly set to 0
 MOTOR_STATE_FORWARD,       // Defaults to 1
 MOTOR_STATE_BACKWARD       // Defaults to 2
  } MotorState_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

  #define ADC_MAX_VALUE       4095   // 12|-bit ADC has a max value of 4095
  #define PWM_PERIOD_VALUE    19999     // TIM1 ARR is set to 19999
  #define MAX_DUTY 50000

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc3;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

  uint16_t adc_value_yakse = 0;
  uint16_t adc_value_xakse = 0;
  int32_t x = 0;
  int32_t y = 0;
  int32_t turn = 0;
  uint32_t base_speed = 25000; // Base speed for the motors, can be adjusted as needed

  MotorState_t current_motor_state = MOTOR_STATE_STOP; // Initial state

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_ADC3_Init(void);
static void MX_TIM1_Init(void);
static void Set_Motor(GPIO_TypeDef *port, uint16_t pin_fram, uint16_t pin_tilbake, uint32_t pwm_channel, int32_t verdi);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART3_UART_Init();
  MX_ADC3_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); //Output til venstre
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); //Output til høyre

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


   /* 
      Tanken her er at potensiometer Joy-sticken gir to ulike ADC verdier, en for X-aksen og en for Y-aksen. Disse verdiene kan brukes til å styre PWM signalene som går til motorene.
      Potensiometeret vil gi en verdi mellom 0 og 4095 (Hvis det er en 12 bit ADC):
      Verdier over 2048 (I x-aksen) vil derfor gi en fremover bevegelse, og under vil gi en bakoverbevegelse.
      For å få en smooth overgang mellom fremover og bakover, kan man bruke en "deadzone" rundt midtpunktet (2048). Dette betyr at hvis verdien er mellom 1900 og 2200, vil motorene ikke bevege seg
      For å regne ut svinging ser man på y-akse ADC verdien. Hvis denne er over 2048, vil roboten svinge til høyre, og under 2048 vil den svinge til venstre. Igjen kan man bruke en deadzone for å unngå små bevegelser.

      Dette vil regnes ut med å ha en relativ hastighet basert på y-aksen på hvert hjul. 
      For simplisitetens skyld, har man en konstant hastighet ved fremover/bakover, og så justerer man hastigheten på hvert hjul basert på y-aksen. Dette kan gjøres med en enkel formel:
      left_motor_speed = base_speed + (y_value - 2048) / 2048
    
    */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


    HAL_ADC_Start(&hadc3); 
    
    HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
    adc_value_yakse = HAL_ADC_GetValue(&hadc3);

    HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
    adc_value_xakse = HAL_ADC_GetValue(&hadc3);


    //Setter Midt på joysticken som midtepunkt for PWM signalene
    x = adc_value_xakse - 2048;
    y = adc_value_yakse - 2048;

    turn = adc_value_yakse - 2048;

    int32_t fram = x;
    int32_t sving = y;

    //Dead zone
    if (fram > -200 && fram < 200) fram = 0;
    if (sving > -200 && sving < 200) sving = 0;

    int32_t venstre = fram + sving;
    int32_t hoyre = fram - sving;

    Set_Motor(GPIOF, GPIO_PIN_13, GPIO_PIN_12, TIM_CHANNEL_1, venstre);
    Set_Motor(GPIOF, GPIO_PIN_14, GPIO_PIN_15, TIM_CHANNEL_2, hoyre);
   

    /*switch(current_motor_state){

      //PF13 => venstre retning 1
      //PF12 => venstre retning 2
      //PF14 => høyre retning 1
      //PF15 => høyre retning 2

      //Retning 1 => fremover, Retning 2 => bakover

      case MOTOR_STATE_STOP:
        
       if( x > 200){
             
             current_motor_state = MOTOR_STATE_FORWARD; // Change to forward state
           } else if (x < -200){
             current_motor_state = MOTOR_STATE_BACKWARD; // Change to backward state
           }
           break;
       
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_RESET);

        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
        break;

        

      case MOTOR_STATE_FORWARD:

        //Fremover med svinging til høyre
        if( x > 200 && y > 200){

          HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
          HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_SET);
          HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_RESET);


          __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, base_speed + turn); // Venstre hjul
          __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, base_speed - turn); // Høyre hjul


        } 

        //fremover med svinging til venstre
        else if (x > 200 && y < -200){
          HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
          HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_SET);
          HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_RESET);

          __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, base_speed - turn); // Venstre hjul
          __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, base_speed + turn); // Høyre hjul
        }

        //Fremover uten svinging
        else if (x > 200 && y <= 200 && y >= -200){
          HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
          HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_SET);
          HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_RESET);

          __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, base_speed ); // Venstre hjul
          __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, base_speed ); // Høyre hjul
        }

       else if (x < 200 && y < 200 && x > -200 && y > -200){
        current_motor_state = MOTOR_STATE_STOP;
        break;
       }

      break;


      case MOTOR_STATE_BACKWARD:

        if (x < 200 && y < 200 && x > -200 && y > -200){
         current_motor_state = MOTOR_STATE_STOP;
         break;
        }
         
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_SET);

        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, base_speed ); // Venstre hjul
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, base_speed ); // Høyre hjul
      break;


    }*/
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 108;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  hadc3.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.NbrOfConversion = 2;
  hadc3.Init.DMAContinuousRequests = DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

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
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
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
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
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
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
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
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PF12 PF13 PF14 PF15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

static void Set_Motor(GPIO_TypeDef *port, uint16_t pin_fram, uint16_t pin_tilbake, uint32_t pwm_channel, int32_t verdi){
 
 if (verdi > 0){
  HAL_GPIO_WritePin(port, pin_fram, GPIO_PIN_SET);
  HAL_GPIO_WritePin(port, pin_tilbake, GPIO_PIN_RESET);
 } else if (verdi < 0){
  HAL_GPIO_WritePin(port, pin_fram, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(port, pin_tilbake, GPIO_PIN_SET);
  verdi = -verdi;
 } else {
  HAL_GPIO_WritePin(port, pin_fram, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(port, pin_tilbake, GPIO_PIN_RESET);
 }

 uint32_t duty = (uint32_t)verdi * MAX_DUTY / 2048;
 if (duty > MAX_DUTY) duty = MAX_DUTY;
 __HAL_TIM_SET_COMPARE(&htim1, pwm_channel, duty);
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
