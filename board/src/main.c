/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stdint.h"
//#include "motors.h"
//#include "ir_recv.h"
#include "dwt_stm32_delay.h"
#include "stdio.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PERIOD      1000

#define CMD_A       0xFF22DD
#define CMD_B       0xFF02FD
#define CMD_C       0xFFC23D
#define CMD_FWD     0xFF9867
#define CMD_BCK     0xFF38C7
#define CMD_LFT     0xFF30CF
#define CMD_RGT     0xFF7A85
#define CMD_BTN     0xFF18E7
#define CMD_PWR     0xFF629D

#define SPEED_FAST  100
#define SPEED_MED   50
#define SPEED_SLOW  25
#define SPEED_NONE  0

#define DIR_FWD     0
#define DIR_BCK     1
#define DIR_RGT     2
#define DIR_LFT     3

#define MAIN_DELAY  200
#define MOTOR_DELAY 20


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define DEBUG_ON() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
#define DEBUG_OFF() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
#define DEBUG_TOGGLE() HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

#define RIGHT_MOTOR_FORWARD(value) __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, (value * PERIOD) / 100)
#define RIGHT_MOTOR_REVERSE(value) __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, (value * PERIOD) / 100)
#define LEFT_MOTOR_FORWARD(value) __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, (value * PERIOD) / 100)
#define LEFT_MOTOR_REVERSE(value) __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, (value * PERIOD) / 100)

#define FORWARD_FAST() RIGHT_MOTOR_FORWARD(SPEED_FAST); RIGHT_MOTOR_REVERSE(SPEED_NONE); LEFT_MOTOR_FORWARD(SPEED_FAST); LEFT_MOTOR_REVERSE(SPEED_NONE)
#define FORWARD_MED() RIGHT_MOTOR_FORWARD(SPEED_MED); RIGHT_MOTOR_REVERSE(SPEED_NONE); LEFT_MOTOR_FORWARD(SPEED_MED); LEFT_MOTOR_REVERSE(SPEED_NONE)
#define FORWARD_SLOW() RIGHT_MOTOR_FORWARD(SPEED_SLOW); RIGHT_MOTOR_REVERSE(SPEED_NONE); LEFT_MOTOR_FORWARD(SPEED_SLOW); LEFT_MOTOR_REVERSE(SPEED_NONE)

#define REVERSE_FAST() RIGHT_MOTOR_REVERSE(SPEED_FAST); RIGHT_MOTOR_FORWARD(SPEED_NONE); LEFT_MOTOR_REVERSE(SPEED_FAST); LEFT_MOTOR_FORWARD(SPEED_NONE)
#define REVERSE_MED() RIGHT_MOTOR_REVERSE(SPEED_MED); RIGHT_MOTOR_FORWARD(SPEED_NONE); LEFT_MOTOR_REVERSE(SPEED_MED); LEFT_MOTOR_FORWARD(SPEED_NONE)
#define REVERSE_SLOW() RIGHT_MOTOR_REVERSE(SPEED_SLOW); RIGHT_MOTOR_FORWARD(SPEED_NONE); LEFT_MOTOR_REVERSE(SPEED_SLOW); LEFT_MOTOR_FORWARD(SPEED_NONE)

#define TURN_CW_FAST() RIGHT_MOTOR_FORWARD(SPEED_NONE); RIGHT_MOTOR_REVERSE(SPEED_FAST); LEFT_MOTOR_FORWARD(SPEED_FAST); LEFT_MOTOR_REVERSE(SPEED_NONE)
#define TURN_CW_MED() RIGHT_MOTOR_FORWARD(SPEED_NONE); RIGHT_MOTOR_REVERSE(SPEED_MED); LEFT_MOTOR_FORWARD(SPEED_MED); LEFT_MOTOR_REVERSE(SPEED_NONE)
#define TURN_CW_SLOW() RIGHT_MOTOR_FORWARD(SPEED_NONE); RIGHT_MOTOR_REVERSE(SPEED_SLOW); LEFT_MOTOR_FORWARD(SPEED_SLOW); LEFT_MOTOR_REVERSE(SPEED_NONE)

#define TURN_CCW_FAST() RIGHT_MOTOR_FORWARD(SPEED_FAST); RIGHT_MOTOR_REVERSE(SPEED_NONE); LEFT_MOTOR_FORWARD(SPEED__NONE); LEFT_MOTOR_REVERSE(SPEED_FAST)
#define TURN_CCW_MED() RIGHT_MOTOR_FORWARD(SPEED_MED); RIGHT_MOTOR_REVERSE(SPEED_NONE); LEFT_MOTOR_FORWARD(SPEED_NONE); LEFT_MOTOR_REVERSE(SPEED_MED)
#define TURN_CCW_SLOW() RIGHT_MOTOR_FORWARD(SPEED_SLOW); RIGHT_MOTOR_REVERSE(SPEED_NONE); LEFT_MOTOR_FORWARD(SPEED__NONE); LEFT_MOTOR_REVERSE(SPEED_SLOW)

#define FORWARD(speed) RIGHT_MOTOR_FORWARD(speed); RIGHT_MOTOR_REVERSE(SPEED_NONE); LEFT_MOTOR_FORWARD(speed); LEFT_MOTOR_REVERSE(SPEED_NONE)
#define REVERSE(speed) RIGHT_MOTOR_REVERSE(speed); RIGHT_MOTOR_FORWARD(SPEED_NONE); LEFT_MOTOR_REVERSE(speed); LEFT_MOTOR_FORWARD(SPEED_NONE)
#define TURN_CW(speed) RIGHT_MOTOR_FORWARD(SPEED_NONE); RIGHT_MOTOR_REVERSE(speed); LEFT_MOTOR_FORWARD(speed); LEFT_MOTOR_REVERSE(SPEED_NONE)
#define TURN_CCW(speed) RIGHT_MOTOR_FORWARD(speed); RIGHT_MOTOR_REVERSE(SPEED_NONE); LEFT_MOTOR_FORWARD(SPEED__NONE); LEFT_MOTOR_REVERSE(speed)

#define STOP() RIGHT_MOTOR_FORWARD(SPEED_NONE); RIGHT_MOTOR_REVERSE(SPEED_NONE); LEFT_MOTOR_FORWARD(SPEED_NONE); LEFT_MOTOR_REVERSE(SPEED_NONE)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t data;

/**
* @brief Function to retrieve NEC encoded command
* @retval Value received
*/
uint32_t receive_data (void)
{
	uint32_t code = 0x00;

		  /* The START Sequence begin here
	   * there will be a pulse of 9ms LOW and
	   * than 4.5 ms space (HIGH)
	   */
	int check = 0;
	  while (!(HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4))) {
		  // wait for the pin to go high.. 9ms LOW
		  DWT_Delay_us(100);
		  check++;
	  };
	  while ((HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4))) {
		   // wait for the pin to go low.. 4.5ms HIGH
		  if (check < 50) {
			  return 0;
		  }
	  }


	  /*
	   * We are only going to check the SPACE after 562.5us pulse
	   * if the space is 562.5us, the bit indicates '0'
	   * if the space is around 1.6ms, the bit is '1'
	   */
    
    uint8_t count;
	  for (int i=0; i < 32; i++)
	  {
		  count = 0;

		  while (!(HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4))); // wait for pin to go high.. this is 562.5us LOW

		  while ((HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4)))  // count the space length while the pin is high
		  {
			  count++;
			  DWT_Delay_us(100);
		  }
		  if (count > 12) // if the space is more than 1.2 ms
		  {
			  code |= (1UL << (31-i));   // write 1
		  }

		  else code &= ~(1UL << (31-i));  // write 0
	  }
		return code;
}
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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  DWT_Delay_Init ();
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
  STOP();
  DEBUG_OFF();
  uint8_t speed = SPEED_SLOW;
  uint8_t direction = DIR_FWD;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  while ((HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4)));   // Wait for the pin to go low indicating value received
	  data = receive_data();
	  if (data > 0) {
		  DEBUG_TOGGLE();
      switch (data) {
        case CMD_A :
          speed = SPEED_SLOW;
          break;

        case CMD_B :
          speed = SPEED_MED;
          break;

        case CMD_C :
          speed = SPEED_FAST;
          break;

        case CMD_FWD :
          if (direction != DIR_FWD) {
            STOP();
            DWT_Delay_us(MOTOR_DELAY);
          }
          direction = DIR_FWD;
          break;

        case CMD_BCK :
          if (direction != DIR_BCK) {
            STOP();
            DWT_Delay_us(MOTOR_DELAY);
          }
          direction = DIR_BCK;
          break;

        case CMD_LFT :
          if (direction != DIR_BCK) {
            STOP();
            DWT_Delay_us(MOTOR_DELAY);
          }
          direction = DIR_LFT;          
          break;

        case CMD_RGT :
          if (direction != DIR_BCK) {
            STOP();
            DWT_Delay_us(MOTOR_DELAY);
          }
          direction = DIR_RGT;
          break;

        case CMD_PWR :
          DEBUG_TOGGLE();
          break;

        case CMD_BTN :
          speed = SPEED_NONE;
          break;
      }

      switch (direction) {
        case DIR_FWD :
          FORWARD(speed)
          break;
        case DIR_BCK :
          REVERSE(speed)
          break;
        case DIR_LFT :
          TURN_CCW(speed)
          break;
        case DIR_RGT :
          TURN_CW(speed)
          break;        
      }
    }
	
    data = 0;
	  HAL_Delay(MAIN_DELAY);
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_10;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  // Period is the ARR
  // PSC is Prescaler

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 127;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

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
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


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
