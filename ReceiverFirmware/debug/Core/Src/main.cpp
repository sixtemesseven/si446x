/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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

#include "si446x.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

  bool arrayCompare(uint8_t* arr1, uint8_t* arr2)
  {
	  for(int i = 0; i < 7; i++)
	  {
		  if(arr1[i] != arr2[i])
		  {
			  return false;
		  }
	  }
	  return true;
  }

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
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
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  si446x rf(&hspi1, GPIOA, nSEL_Pin, GPIOF, SDN_Pin, GPIOA, IRQ_Pin); //446x object
  rf.powerUp(); //Power up

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  /*
	  for(int i = 0; i<3; i++) //Blink thre times as heartbeat
	 	  {
	 		  HAL_GPIO_WritePin(GPIOB, Relay_5_Pin, GPIO_PIN_SET);
	 		  HAL_GPIO_WritePin(GPIOA, Relay_1_Pin, GPIO_PIN_SET);
	 		  HAL_GPIO_WritePin(GPIOF, LED_Pin, GPIO_PIN_SET);
	 	  	  HAL_Delay(1000);
	 	  	  HAL_GPIO_WritePin(GPIOB, Relay_5_Pin, GPIO_PIN_RESET);
	 	  	  HAL_GPIO_WritePin(GPIOA, Relay_1_Pin, GPIO_PIN_RESET);
	 	  	  HAL_GPIO_WritePin(GPIOF, LED_Pin, GPIO_PIN_RESET);
	 	  	  HAL_Delay(1000);
	 	  }


	 	  if(HAL_GPIO_ReadPin(GPIOA, SW1_Pin) == GPIO_PIN_SET)
	 	  {
	 		  uint8_t packet[50] = {"OpenDoor1"};
	 		  HAL_UART_Transmit(&huart2, packet, 14, 5000);
	 		  rf.sendPacket(packet, 14, 1);
	 		  HAL_Delay(200);

	 		  HAL_Delay(1000);
	 	  }
	 	  */

	 	  //MODEM 2 - RECEIVER
	  	  HAL_GPIO_WritePin(GPIOB, Relay_5_Pin, GPIO_PIN_RESET);
	  	  while (1)
	 		  {
	 			  rf.startRX(1);
	 			  HAL_Delay(30);
	 			  uint8_t fifoBytes = rf.getRxFifoInfo();
	 			  uint8_t buf[16] = {};
	 			  if(fifoBytes > 0)
	 			  {
	 				  uint8_t packetReceived[] = "/n Packet Received \n";
	 				  HAL_UART_Transmit(&huart2, packetReceived, sizeof(packetReceived), 1000);
	 				  rf.getPacket(buf, fifoBytes);
	 				  //HAL_UART_Transmit(&huart2, buf, sizeof(buf), 1000);
	 				  //break;
	 				  //if(buf == answer){break;};
	 				  HAL_GPIO_WritePin(GPIOB, Relay_5_Pin, GPIO_PIN_SET);
	 				  HAL_Delay(100);
	 				  HAL_GPIO_WritePin(GPIOB, Relay_5_Pin, GPIO_PIN_RESET);
	 			  }
	 		  }

	 		 /*
	  	  	  	  	  	  HAL_GPIO_WritePin(GPIOF, Relay_5_Pin, GPIO_PIN_RESET);
	  	  	  	  	  	  rf.startRX(1);
	  	  	  	  	  	  HAL_Delay(1000);
	  		 			  uint8_t fifoBytes = rf.getRxFifoInfo();
	  		 			  uint8_t buf[16] = {};
	  		 			  if(fifoBytes > 0)
	  		 			  {
	  		 				  uint8_t packetReceived[] = "/n Packet Received \n";
	  		 				  HAL_UART_Transmit(&huart2, packetReceived, sizeof(packetReceived), 1000);
	  		 				  rf.getPacket(buf, fifoBytes);
	  		 				  HAL_UART_Transmit(&huart2, buf, sizeof(buf), 1000);
	  		 				  uint8_t answer[] = "1234567";
	  		 				  if(arrayCompare(buf, answer) == true)
	  		 				  {
	  		 					uint8_t packetReceived[] = "/n Sending Answer \n";
	  		 					HAL_UART_Transmit(&huart2, packetReceived, sizeof(packetReceived), 1000);
	  		 					uint8_t data[] = "7654321";
	  		 					rf.sendPacket(data, sizeof(data), 1);
	  		 					HAL_GPIO_WritePin(GPIOF, Relay_5_Pin, GPIO_PIN_SET);
	  		 					HAL_Delay(1000);
	  		 					HAL_GPIO_WritePin(GPIOF, Relay_5_Pin, GPIO_PIN_RESET);
	  		 				  }
	  		 			  }
			*/

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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Relay_5_GPIO_Port, Relay_5_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Relay_3_Pin|nSEL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_Pin SDN_Pin */
  GPIO_InitStruct.Pin = LED_Pin|SDN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : SW1_Pin */
  GPIO_InitStruct.Pin = SW1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(SW1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Relay_2_Pin IRQ_Pin Relay_4_Pin */
  GPIO_InitStruct.Pin = Relay_2_Pin|IRQ_Pin|Relay_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : Relay_5_Pin */
  GPIO_InitStruct.Pin = Relay_5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Relay_5_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Relay_3_Pin nSEL_Pin */
  GPIO_InitStruct.Pin = Relay_3_Pin|nSEL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/