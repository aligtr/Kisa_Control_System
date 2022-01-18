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
#include "can.h" //TX-PB13, RX-PB12
#include "spi.h" //MISO-PC11,SCK-PC10
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "servo.h"
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

/* USER CODE BEGIN PV */
extern  SPI_HandleTypeDef hspi3;
extern CAN_HandleTypeDef hcan2;
extern Servo_t servo[4];
CAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8] = {0,};
uint8_t mode=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint16_t getCurrentAngle(char i, SPI_HandleTypeDef* hspi);
void canFrameGet(CAN_HandleTypeDef* hcan);
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
	uint8_t i=0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	tim1Init();
	tim8Init();
	tim9Init();
	tim12Init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN2_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
	for(i=0;i<4;i++){
		servo[i].calibFlag = 0;
		servo[i].aimAngle = 2048;
		servo[i].pulseCounter = 2048;
		servo[i].aimPulse = 2048;
		servo[i].encoderDate = 2048;
	}
	
	HAL_CAN_Start(&hcan2);	
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		mode=1;//always working state
		servo[0].aimPulse=1024;
		servo[1].aimPulse=1024;
		servo[2].aimPulse=1024;
		servo[3].aimPulse=1024;
		if (mode==0){	
			for(i=0;i<4;i++) servo[i].calibFlag = 0;
			
			while(servo[0].calibFlag==0 || servo[1].calibFlag==0 || servo[2].calibFlag==0 || servo[3].calibFlag==0){
				for(i=0;i<4;i++){
					servo[i].encoderDate = getCurrentAngle(i, &hspi3);
				  if (servo[i].encoderDate < 1024 || servo[i].encoderDate > 3072)  while(1); // error state
				}
				Calibration();
			}
			mode=1;
		}		
		else {
			for(i=0;i<4;i++) 
				if (servo[i].encoderDate < 1024 || servo[i].encoderDate > 3072)  while(1); // error state
			servoControl();
		}
		
		HAL_Delay(100);
		
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

uint16_t getCurrentAngle(char i, SPI_HandleTypeDef* hspi)
{
	uint16_t encoderDate=0;
	uint8_t buff[4];
	switch (i)
	{
		case 0:
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
 			HAL_SPI_Receive(hspi,buff,4,100);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
		break;
		case 1:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
			HAL_SPI_Receive(hspi,buff,4,100);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		break;
		case 2:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
			HAL_SPI_Receive(hspi,buff,4,100);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
		break;
		case 3:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
			HAL_SPI_Receive(hspi,buff,4,100);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);
		break;
	}
	encoderDate = ((uint16_t)buff[0]<<4 | (uint32_t)buff[3]<<3  | ((uint32_t)buff[2])/32);
	
	return encoderDate;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

        if(RxHeader.StdId == 0x02A0)
        {
					if(RxHeader.DLC == 1){
						mode=0;
						servo[0].aimAngle=0;
						servo[1].aimAngle=0;
						servo[2].aimAngle=0;
						servo[3].aimAngle=0;
					}
					else if(RxHeader.DLC == 9){					
						mode=1;
						servo[0].aimAngle=(uint16_t)RxData[0]<<8 | (uint16_t)RxData[1];
						servo[1].aimAngle=(uint16_t)RxData[2]<<8 | (uint16_t)RxData[3];;
						servo[2].aimAngle=(uint16_t)RxData[4]<<8 | (uint16_t)RxData[5];;
						servo[3].aimAngle=(uint16_t)RxData[6]<<8 | (uint16_t)RxData[7];;
					}
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
