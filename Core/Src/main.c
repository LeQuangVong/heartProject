/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
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
#include "my_SSD1306.h"
#include "my_MAX30100.h"
#include "my_Fonts.h"

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
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
MAX30100_Handle_t max30100;

float ir_filtered = 0.0, red_filtered = 0.0;
float dc_red = 0.0, dc_ir = 0.0, ac_red = 0.0, ac_ir = 0.0;
float spo2 = 0.0, heart_rate = 0.0;

const int sample_rate = 100;
const int analysis_time = 5;
int length = sample_rate * analysis_time;
int peak_count = 0;
float peak_interval = 0;
uint8_t fifo_full_flag = 0;
uint8_t int_status;
uint8_t data_ready_flag = 0;
uint8_t error_flag = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
void process(void);
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
  MX_I2C1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  SSD1306_Init();
  MAX30100_Init(&max30100, &hi2c1);
  SSD1306_Fill(Black);
  //SSD1306_WriteString("SSD1306 OK", Font_7x10, White);
  SSD1306_UpdateScreen();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if(fifo_full_flag)
	  {
		  fifo_full_flag = 0;
	      MAX30100_Read_FIFO(&max30100); // Đọc FIFO
	  }

	  if(data_ready_flag)
	  {
	      data_ready_flag = 0;
	      process(); // Xử lý dữ liệu (lọc, tính toán, hiển thị)
	  }

	  if (error_flag)
	  {
		  SSD1306_SetCursor(0, 0);
		  SSD1306_WriteString("Error detected", Font_7x10, White);
		  SSD1306_UpdateScreen();
		  HAL_Delay(1000);
		  MAX30100_Init(&max30100, &hi2c1);
		  error_flag = 0;
		  SSD1306_SetCursor(0, 20);
		  SSD1306_WriteString("Restarting...", Font_7x10, White);
		  SSD1306_UpdateScreen();
		  HAL_Delay(2000); // Khởi động lại cảm biến
	  }
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CS_Pin|DC_Pin|RES_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_Pin DC_Pin RES_Pin */
  GPIO_InitStruct.Pin = CS_Pin|DC_Pin|RES_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

}

/* USER CODE BEGIN 4 */
void process()
{
	if(int_status & (1 << 5))
	{
		ir_filtered = low_pass_filter(max30100.ir, ir_filtered, 0.9);
		peak_count = detect_peak(&ir_filtered, length);
		peak_interval = 1.0f / sample_rate;
		heart_rate = (60.0f / (peak_interval * peak_count));
	}

	if(int_status & (1 << 4))
	{
		red_filtered = low_pass_filter(max30100.red, red_filtered, 0.9);

		dc_ir = ir_filtered;
		dc_red = red_filtered;

		ac_ir = max30100.ir - dc_ir;
		ac_red = max30100.red - dc_red;
		spo2 = calculate_spo2(ac_red, dc_red, ac_ir, dc_ir);
	}

	if (heart_rate < 40.0f || heart_rate > 180.0f)
	{
		SSD1306_SetCursor(0, 20);
	    SSD1306_WriteString("HR Error", Font_7x10, White);
	    SSD1306_UpdateScreen();  // Cập nhật màn hình
	}

	if (spo2 < 85.0f || spo2 > 100.0f)
	{
		SSD1306_SetCursor(0, 40);
	    SSD1306_WriteString("SpO2 Error", Font_7x10, White);
	    SSD1306_UpdateScreen();  // Cập nhật màn hình
	}

	char buffer[32];

    SSD1306_SetCursor(0, 0);
    SSD1306_WriteString("MAX30100 Data", Font_7x10, White);

    snprintf(buffer, sizeof(buffer), "HR: %.1f bpm", heart_rate);
    SSD1306_SetCursor(0, 20);
    SSD1306_WriteString(buffer, Font_7x10, White);

    snprintf(buffer, sizeof(buffer), "SpO2: %.1f %%", spo2);
    SSD1306_SetCursor(0, 40);
    SSD1306_WriteString(buffer, Font_7x10, White);

    SSD1306_UpdateScreen();  // Cập nhật màn hình
    HAL_Delay(1000);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_1)
	{
		if (HAL_I2C_Mem_Read(&hi2c1, MAX30100_ADDR, MAX30100_IT_STATUS, I2C_MEMADD_SIZE_8BIT, &int_status, 1, HAL_MAX_DELAY) != HAL_OK)
		{
			// Xử lý lỗi nếu không đọc được
			error_flag = 1;
			return;
		}

		if (int_status & (1 << 7)) // FIFO Full
		{
			fifo_full_flag = 1;
		}

		// Kiểm tra dữ liệu sẵn sàng
		if (int_status & (1 << 5)) // Data Ready
		{
			data_ready_flag = 1;
		}

		if(int_status & (1 << 6))
		{
			error_flag = 1;
		}
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
