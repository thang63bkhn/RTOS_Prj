/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_i2c.h"
#include "ds3231_i2c.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
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
I2C_HandleTypeDef hi2c2;
I2C_HandleTypeDef hi2c3;

UART_HandleTypeDef huart2;

osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osThreadId myTask03Handle;
osThreadId myTask04Handle;
osThreadId myTask05Handle;
osThreadId myTask06Handle;
osMutexId LCD_MutexHandle;
osSemaphoreId LCD_SemHandle;
osSemaphoreId UART_SemHandle;
/* USER CODE BEGIN PV */
typedef struct Data {
	char DateString[23];
	char TimeString[16];
} DataTypeDef;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_I2C3_Init(void);
static void MX_USART2_UART_Init(void);
void Read_time(void const * argument);
void Read_temperature(void const * argument);
void LCD_temperature(void const * argument);
void LCD_time(void const * argument);
void UART_temperature(void const * argument);
void UART_time(void const * argument);

/* USER CODE BEGIN PFP */
static void InitHardware(void);
static void SetTime(char* timeString);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void InitHardware(void)
{
	DS3231_Init(&hi2c2);
	DS3231_EnableAlarm();
	SetTime("22:08:14:01:16:30:20");

	LCD_Init(&hi2c3);
	LCD_EnableBacklight();
}
void SetTime(char* timeString)
{
	DS3231_TimeTypeDef myTime;
	char Time_String[3];
	
	strncpy(Time_String, timeString, 2);
	myTime.Year = (uint8_t)atoi(Time_String);
		
	strncpy(Time_String, timeString + 3, 2);
	myTime.Month = (uint8_t)atoi(Time_String);
		
	strncpy(Time_String, timeString + 6, 2);
	myTime.Date = (uint8_t)atoi(Time_String);
		
	strncpy(Time_String, timeString + 9, 2);
	myTime.DoW = (uint8_t)atoi(Time_String);
		
	strncpy(Time_String, timeString + 12, 2);
	myTime.Hour = (uint8_t)atoi(Time_String);
		
	strncpy(Time_String, timeString + 15, 2);
	myTime.Min = (uint8_t)atoi(Time_String);
		
	strncpy(Time_String, timeString + 18, 2);
	myTime.Sec = (uint8_t)atoi(Time_String);
	
	DS3231_SetTime(&myTime);
}

xQueueHandle temp_queue_handler;
xQueueHandle time_queue_handler;
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
  MX_I2C2_Init();
  MX_I2C3_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	InitHardware();
  /* USER CODE END 2 */

  /* Create the mutex(es) */
  /* definition and creation of LCD_Mutex */
  osMutexDef(LCD_Mutex);
  LCD_MutexHandle = osMutexCreate(osMutex(LCD_Mutex));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of LCD_Sem */
  osSemaphoreDef(LCD_Sem);
  LCD_SemHandle = osSemaphoreCreate(osSemaphore(LCD_Sem), 1);

  /* definition and creation of UART_Sem */
  osSemaphoreDef(UART_Sem);
  UART_SemHandle = osSemaphoreCreate(osSemaphore(UART_Sem), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
	temp_queue_handler = xQueueCreate(5, sizeof(float));
  time_queue_handler = xQueueCreate(5, sizeof(DataTypeDef));
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, Read_time, osPriorityAboveNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, Read_temperature, osPriorityRealtime, 0, 128);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* definition and creation of myTask03 */
  osThreadDef(myTask03, LCD_temperature, osPriorityHigh, 0, 256);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  /* definition and creation of myTask04 */
  osThreadDef(myTask04, LCD_time, osPriorityNormal, 0, 256);
  myTask04Handle = osThreadCreate(osThread(myTask04), NULL);

  /* definition and creation of myTask05 */
  osThreadDef(myTask05, UART_temperature, osPriorityHigh, 0, 256);
  myTask05Handle = osThreadCreate(osThread(myTask05), NULL);

  /* definition and creation of myTask06 */
  osThreadDef(myTask06, UART_time, osPriorityNormal, 0, 256);
  myTask06Handle = osThreadCreate(osThread(myTask06), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
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
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

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
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MEMS_CS_GPIO_Port, MEMS_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : MEMS_CS_Pin */
  GPIO_InitStruct.Pin = MEMS_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MEMS_CS_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_Read_time */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Read_time */
void Read_time(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
		DS3231_TimeTypeDef	getDS3231Time;
		DataTypeDef Data1;
		DataTypeDef *DataToSend;
		
		if (DS3231_GetTime(&getDS3231Time) != HAL_OK)
		{
			HAL_UART_Transmit(&huart2, (uint8_t*)"Get Time error !\n", 18, HAL_MAX_DELAY);
		}

		if (getDS3231Time.DoW == 1)
		{
			sprintf(Data1.DateString, "SUN/%02d/%02d/20%02d\n", getDS3231Time.Date, getDS3231Time.Month, getDS3231Time.Year);
		}
		else if (getDS3231Time.DoW == 2)
		{
			sprintf(Data1.DateString, "MON/%02d/%02d/20%02d\n", getDS3231Time.Date, getDS3231Time.Month, getDS3231Time.Year);
		}
		else if (getDS3231Time.DoW == 3)
		{
			sprintf(Data1.DateString, "TUE/%02d/%02d/20%02d\n", getDS3231Time.Date, getDS3231Time.Month, getDS3231Time.Year);
		}
		else if (getDS3231Time.DoW == 4)
		{
			sprintf(Data1.DateString, "WED/%02d/%02d/20%02d\n", getDS3231Time.Date, getDS3231Time.Month, getDS3231Time.Year);
		}
		else if (getDS3231Time.DoW == 5)
		{
			sprintf(Data1.DateString, "THU/%02d/%02d/20%02d\n", getDS3231Time.Date, getDS3231Time.Month, getDS3231Time.Year);
		}
		else if (getDS3231Time.DoW == 6)
		{
			sprintf(Data1.DateString, "FRI/%02d/%02d/20%02d\n", getDS3231Time.Date, getDS3231Time.Month, getDS3231Time.Year);
		}
		else if (getDS3231Time.DoW == 7)
		{
			sprintf(Data1.DateString, "SAT/%02d/%02d/20%02d\n", getDS3231Time.Date, getDS3231Time.Month, getDS3231Time.Year);
		}
		sprintf(Data1.TimeString, "%02d:%02d:%02d", getDS3231Time.Hour, getDS3231Time.Min, getDS3231Time.Sec);
		
		DataToSend = pvPortMalloc(sizeof(DataTypeDef));
		
		strcpy(DataToSend->DateString, Data1.DateString);
		strcpy(DataToSend->TimeString, Data1.TimeString);
		
		xQueueSend(time_queue_handler, &DataToSend, 100);
		osDelay(10);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_Read_temperature */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Read_temperature */
void Read_temperature(void const * argument)
{
  /* USER CODE BEGIN Read_temperature */
  /* Infinite loop */
  for(;;)
  {
		uint8_t dataRecv[2];
		uint16_t i2cdata;
		float tempMLXobj1;
		float value = 0.0;
		
		HAL_I2C_Mem_Read(&hi2c1, 0x5A<<1, 0x07, 1, dataRecv, 3, 1000);
		i2cdata = (dataRecv[1]<<8 | dataRecv[0]);
		tempMLXobj1 = i2cdata * 0.02 - 273.15;
		
		if(tempMLXobj1 < 100)
		{
			value = tempMLXobj1;
		}
		
		xQueueSend(temp_queue_handler, &value, 100);
    osDelay(1);
  }
  /* USER CODE END Read_temperature */
}

/* USER CODE BEGIN Header_LCD_temperature */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LCD_temperature */
void LCD_temperature(void const * argument)
{
  /* USER CODE BEGIN LCD_temperature */
  /* Infinite loop */
  for(;;)
  {
	  char tempToLCD[20];
	  float recv_temperature = 0;
		
		xQueueReceive(temp_queue_handler, &recv_temperature, 1000);
		
		sprintf(tempToLCD, "TEMP: %0.2f", recv_temperature);
		
		osMutexWait(LCD_MutexHandle, osWaitForever);
		
		LCD_SendStringAt(tempToLCD, 0, 0);
		HAL_Delay(1000);
		LCD_SetFunction(0x01);
		HAL_Delay(100);
		
		osMutexRelease(LCD_MutexHandle);
		
	  osDelay(500);
  }
  /* USER CODE END LCD_temperature */
}

/* USER CODE BEGIN Header_LCD_time */
/**
* @brief Function implementing the myTask04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LCD_time */
void LCD_time(void const * argument)
{
  /* USER CODE BEGIN LCD_time */
  /* Infinite loop */
  for(;;)
  {
	  DataTypeDef* Data2;

	  xQueueReceive(time_queue_handler, &Data2, 100);
		
	  char dataD1[30];
	  char dataT1[30];
		char str1[10] = "DATE:";
		char str2[10] = "TIME:";
		
	  sprintf(dataD1, "%s %s\n", str1, Data2->DateString);
	  sprintf(dataT1, "%s %s\n", str2, Data2->TimeString);
		
	  vPortFree(Data2);
		osMutexWait(LCD_MutexHandle, osWaitForever);
		
		LCD_SendStringAt(dataD1, 0, 0);
		LCD_SendStringAt(dataT1, 0, 1);
		HAL_Delay(800);
		for(int i=0; i<strlen(dataD1); i++)
		{
			LCD_SetFunction(0x18);
			HAL_Delay(600);
		}
		LCD_SetFunction(0x01);
		HAL_Delay(100);

	  osMutexRelease(LCD_MutexHandle);
	  osDelay(500);
  }
  /* USER CODE END LCD_time */
}

/* USER CODE BEGIN Header_UART_temperature */
/**
* @brief Function implementing the myTask05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_UART_temperature */
void UART_temperature(void const * argument)
{
  /* USER CODE BEGIN UART_temperature */
  /* Infinite loop */
  for(;;)
  {
		float recv_temperature = 0;
		
		xQueueReceive(temp_queue_handler, &recv_temperature, 1000);

		char str[20]={0};
		sprintf(str, "\nTEMP: %0.2f\n", recv_temperature);

		osSemaphoreWait(UART_SemHandle, osWaitForever);

		HAL_UART_Transmit(&huart2,(uint8_t*) &str, sizeof(str), 100);

		osSemaphoreRelease(UART_SemHandle);
    osDelay(1000);
  }
  /* USER CODE END UART_temperature */
}

/* USER CODE BEGIN Header_UART_time */
/**
* @brief Function implementing the myTask06 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_UART_time */
void UART_time(void const * argument)
{
  /* USER CODE BEGIN UART_time */
  /* Infinite loop */
  for(;;)
  {
	  DataTypeDef* Data3;

	  xQueueReceive(time_queue_handler, &Data3, 1000);

	  char dataD2[20]={0};
	  char dataT2[20]={0};
		char str1[10] = "Date:";
		char str2[10] = "\nTime:";

	  sprintf(dataD2, "%s %s\n", str1, Data3->DateString);
	  sprintf(dataT2, "%s %s\n", str2, Data3->TimeString);

	  vPortFree(Data3);

	  osSemaphoreWait(UART_SemHandle, osWaitForever);

	  HAL_UART_Transmit(&huart2,(uint8_t*) &dataD2, sizeof(dataD2), 100);
	  HAL_UART_Transmit(&huart2,(uint8_t*) &dataT2, sizeof(dataT2), 100);

	  osSemaphoreRelease(UART_SemHandle);
	  osDelay(1000);
  }
  /* USER CODE END UART_time */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
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
