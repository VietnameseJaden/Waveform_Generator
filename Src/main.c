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
#include "Display.h"
#include "kuromi.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


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
ADC_HandleTypeDef hadc1;


UART_HandleTypeDef huart1;


/* USER CODE BEGIN PV */


/* USER CODE END PV */


/* Private function prototypes -----------------------------------------------*/

static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */


/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */
void delay_ms(uint32_t delay);
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


  /* USER CODE BEGIN Init */




  /* Configure the system clock */


  /* USER CODE BEGIN SysInit */


  /* USER CODE END SysInit */


  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN; // Set I/O ports

	(void)RCC->IOPENR; // System check


	GPIOA -> MODER &= ~(3UL << 0); // bit mask 11 on position 0
	GPIOA -> MODER |= (3UL << 0); // send bits 11 to position 0
	GPIOA -> MODER &= ~(3UL << 10) | (3UL << 0);
	GPIOA -> MODER &= ~(3UL << 12) | (3UL << 0);
	GPIOA -> MODER &= ~(3UL << 14) | (3UL << 0);
	GPIOA -> MODER &= ~(3UL << 22) | (3UL << 0);
	GPIOA -> MODER &= ~(3UL << 30) | (3UL << 0);
	GPIOA -> MODER &= ~(3UL << 2);
	GPIOA -> MODER |= (3UL << 2);
	GPIOA->PUPDR &= ~((3UL << 0) | (3UL << 2)); // turn off pull-down pull up
	RCC->APBENR2 |= (1UL << 20);



	if((ADC1 -> CR & (1UL << 0)) != 0UL) // Check if ADEN is on
	{
	  if((ADC1 -> CR & (1UL << 2)) != 0UL)
		{
		  ADC1 -> CR |= (1UL << 4);

		  while((ADC1 -> CR & (1UL << 4)) != 0UL)
		  {
		  }

		}

	  ADC1 -> CR |= (1UL << 1); // HARDWARE RESET ADEN
	  while ((ADC1 -> CR & (1UL << 0)) != 0UL) // WAITS FOR ADEN TO RESET
	  {
	  }
	}


	ADC1 -> CFGR2 &= ~(3UL << 30);
	ADC1 -> CFGR2 |= (1UL << 30);

	ADC1->CFGR1 &= ~((1UL << 21) | (1UL << 15) | (1UL << 13) | (1UL << 0));
	ADC1 -> SMPR &= ~(7UL << 0);
	ADC1 -> SMPR |= (6UL << 0);

	ADC1 -> CR |= (1UL << 28); // VOLTAGE REGULATOR ON


	delay_ms(500); // MAKE SURE EVERYTHING IS ON


	uint32_t calibration;
	ADC1 -> CR |= (1UL << 31); //ADCAL ON
	while ((ADC1->CR & (1UL << 31)) != 0UL)
	{
	}
	calibration = ADC1 -> CALFACT & 0x7FUL; // change to [0:6] bit
	calibration++;
	ADC1 -> CALFACT = (ADC1 -> CALFACT & ~0x7FUL) | (calibration & 0x7FUL); // set to new [0:6] bit

	ADC1 -> CHSELR = (1U << 14);
	ADC1 -> CHSELR = (1U << 0) | (1U << 1);


	while ((ADC1 -> ISR & (1UL << 13)) == 0UL) // Wait for CCRDY
	{
	}

	ADC1 -> ISR = (1UL << 13); // clear CCRDY

	ADC1 -> ISR = (1UL << 0);
	ADC1 -> CR |= (1UL << 0); //SET ADEN ON


	while ((ADC1 -> ISR & (1UL << 0)) == 0UL) // Wait for ADRDY to turn on
	{
	}

	/* USER CODE END 2 */



	  OLED_Init();
	  clearbuffer();
	  OLED_Update();

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

	  uint16_t y;

	  ADC1 -> ISR = (1UL << 2) | (1UL << 3) | (1UL << 4); // Reset conversion flags
	  ADC1 -> CR |= (1UL << 2);

	  while ((ADC1->ISR & (1UL << 2)) == 0UL)
	  {
	  }

	  y = (uint16_t)ADC1 -> DR;

	  draw_wave(y);
	  OLED_Update();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}


void delay_ms(uint32_t delay)
{


	SysTick->LOAD = 12000-1;
	SysTick->VAL = 0;


	SysTick->CTRL = (1U << 0) | (1U << 2);
	for(uint32_t i = 0; i<delay; i++)
		{
			while(!(SysTick-> CTRL & (1U << 16)))
			{
			}
		}
	SysTick->CTRL = 0;
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


  __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_0);


  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV4;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }


  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;


  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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


  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_SEQ_FIXED;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }


  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }


  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */


  /* USER CODE END ADC1_Init 2 */


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
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
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
  __HAL_RCC_GPIOA_CLK_ENABLE();


  /*Configure GPIO pin : PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


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
