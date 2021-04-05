/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MotorControl.h"
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
/* USER CODE BEGIN Variables */
extern CAN_HandleTypeDef hcan3;
extern SPI_HandleTypeDef hspi3;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId vKinematicaHandle;
osThreadId vPDUReaderHandle;
osThreadId vMotorControlHandle;
osThreadId vServoControlHandle;
osMessageQId xQueuePDUDateHandle;
osMessageQId xQueueVelDateHandle;
osMessageQId xQueueAngleDateHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void vKinematicaTask(void const * argument);
void vPDUReaderTask(void const * argument);
void vMotorControlTask(void const * argument);
void vServoControlTask(void const * argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of xQueuePDUDate */
  osMessageQDef(xQueuePDUDate, 5, uint32_t);
  xQueuePDUDateHandle = osMessageCreate(osMessageQ(xQueuePDUDate), NULL);

  /* definition and creation of xQueueVelDate */
  osMessageQDef(xQueueVelDate, 20, Motor_t);
  xQueueVelDateHandle = osMessageCreate(osMessageQ(xQueueVelDate), NULL);

  /* definition and creation of xQueueAngleDate */
  osMessageQDef(xQueueAngleDate, 4, float);
  xQueueAngleDateHandle = osMessageCreate(osMessageQ(xQueueAngleDate), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 512);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of vKinematica */
  osThreadDef(vKinematica, vKinematicaTask, osPriorityIdle, 0, 512);
  vKinematicaHandle = osThreadCreate(osThread(vKinematica), NULL);

  /* definition and creation of vPDUReader */
  osThreadDef(vPDUReader, vPDUReaderTask, osPriorityIdle, 0, 512);
  vPDUReaderHandle = osThreadCreate(osThread(vPDUReader), NULL);

  /* definition and creation of vMotorControl */
  osThreadDef(vMotorControl, vMotorControlTask, osPriorityIdle, 0, 512);
  vMotorControlHandle = osThreadCreate(osThread(vMotorControl), NULL);

  /* definition and creation of vServoControl */
  osThreadDef(vServoControl, vServoControlTask, osPriorityIdle, 0, 256);
  vServoControlHandle = osThreadCreate(osThread(vServoControl), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_vKinematicaTask */
/**
* @brief Function implementing the vKinematica thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vKinematicaTask */
void vKinematicaTask(void const * argument){
	uint32_t vel_mean=0;
	uint32_t rx_mean=0;
	uint32_t dir_mean=0;
	uint32_t ry_mean=0;
	uint32_t mode=0;
	while(1){
		if ((uxQueueMessagesWaiting(xQueuePDUDateHandle) != 5) || (uxQueueMessagesWaiting(xQueueVelDateHandle) == 4) || (uxQueueMessagesWaiting(xQueueAngleDateHandle) == 4)){
         taskYIELD();
     }
		 else{
				xQueueReceive(xQueuePDUDateHandle, &vel_mean, 0);
				xQueueReceive(xQueuePDUDateHandle, &dir_mean, 0);
				xQueueReceive(xQueuePDUDateHandle, &rx_mean, 0);
				xQueueReceive(xQueuePDUDateHandle, &ry_mean, 0);
				xQueueReceive(xQueuePDUDateHandle, &mode, 0);
			 
				normaliz(vel_mean, rx_mean, dir_mean, ry_mean);
				kinematica(mode);
			 
				xQueueSendToBack(xQueueVelDateHandle, &Vfl, 0);
				xQueueSendToBack(xQueueVelDateHandle, &Vfr, 0);
				xQueueSendToBack(xQueueVelDateHandle, &Vrl, 0);
				xQueueSendToBack(xQueueVelDateHandle, &Vrr, 0);
			 
				xQueueSendToBack(xQueueAngleDateHandle, &gfl, 0);
				xQueueSendToBack(xQueueAngleDateHandle, &gfr, 0);
				xQueueSendToBack(xQueueAngleDateHandle, &grl, 0);
				xQueueSendToBack(xQueueAngleDateHandle, &grr, 0);
				
		 }
		 vTaskDelay(100);
	
  }
}

/* USER CODE BEGIN Header_vODUReaderTask */
/**
* @brief Function implementing the vPDUReader thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vODUReaderTask */
void vPDUReaderTask(void const * argument){
	uint32_t mode=0;
	uint32_t vel_mean=0;
	uint32_t dir_mean=0;
	uint32_t rx_mean=0;
	uint32_t smt_src1=0;
	uint32_t smt_src2=0;
	uint32_t smt_src3=0;
	uint32_t smt_src4=0;
	uint32_t ry_mean=0;
	uint16_t channels[18]={0};
	while (1){
		if( uxQueueMessagesWaiting( xQueuePDUDateHandle ) != 5 ){
					receiveSBusDate(&channels);
					vel_mean=channels[RIGHT_VERT];
					dir_mean=channels[RIGHT_HORIZ];
					rx_mean=channels[LEFT_VERT];
					smt_src1=channels[ELEVDR];
					smt_src2=channels[AILDR];
					smt_src3=channels[GEAR];
					smt_src4=channels[RUDDR];
					ry_mean=channels[LEFT_HORIZ];
				if(checkDate(vel_mean) && checkDate(dir_mean) && checkDate(rx_mean) && \
					checkDate(smt_src1) && checkDate(smt_src2) && checkDate(smt_src3) && checkDate(ry_mean)) {
					// if(smt_src2>700 || smt_src2<200)
					// {
					// 	reset();
					// }				
						
					xQueueSendToBack(xQueuePDUDateHandle, &vel_mean, 0);
					xQueueSendToBack(xQueuePDUDateHandle, &dir_mean, 0);
					xQueueSendToBack(xQueuePDUDateHandle, &rx_mean, 0);
					xQueueSendToBack(xQueuePDUDateHandle, &ry_mean, 0);
					xQueueSendToBack(xQueuePDUDateHandle, &mode, 0);
					
          
					// xTaskSuspend(vKinematicaHandle);
					// xTaskSuspend(vServoControlHandle);//date ready
				}
				else{
					// xTaskResume(vKinematicaHandle);
					// xTaskResume(vServoControlHandle);//no pdu
				}
				vTaskDelay(100);
		}
		else 
      taskYIELD();
			
	}
}

/* USER CODE BEGIN Header_vMotorControlTask */
/**
* @brief Function implementing the vMotorControl thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vMotorControlTask */
void vMotorControlTask(void const * argument)
{
  /* USER CODE BEGIN vMotorControlTask */
  Motor_t motor;
  int16_t i;
  motor.command=MOTOR_START;
  motorRealeseCommand(motor);
  for(;;)
  {
    // motor.command=SET_TORQUE_PID;
    // motor.torquePID[0]=12;
    // motor.torquePID[1]=34;
    // motor.torquePID[2]=56;
    // motorRealeseCommand(motor);
    // osDelay(1000);
    // motor.torquePID[0]=0;
    // motor.torquePID[1]=0;
    // motor.torquePID[2]=0;
    // motorRealeseCommand(motor);
    // osDelay(1000);
    for(i=20;i<300;i++)
    {
      motor.command=CHANGE_SPEED;
      motor.speed=i;
      motorRealeseCommand(motor);
      osDelay(50);
    }
    for(i=300;i>20;i--)
    {
      motor.command=CHANGE_SPEED;
      motor.speed=i;
      motorRealeseCommand(motor);
      osDelay(50);
    }
    /*osDelay(5000);
    motor.command=MOTOR_STOP;
    motorRealeseCommand(motor);
    osDelay(5000);*/
  }
  /* USER CODE END vMotorControlTask */
}

/* USER CODE BEGIN Header_vServoControlTask */
/**
* @brief Function implementing the vServoControl thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vServoControlTask */
static servo Servos[4]={0};
void vServoControlTask(void const * argument){
	char i=0;
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
	while(1){
		if ((uxQueueMessagesWaiting(xQueuePDUDateHandle) != 4)){
         taskYIELD();
     }
		 else{
				for (i=0; i<4; i++) {					
					getCurrentAngle(i,&Servos[i], &hspi3);
					xQueueReceive(xQueuePDUDateHandle, &Servos[i], 0);
					PI_control(&Servos[i]);
				}
		 }
		 vTaskDelay(100);
	}
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
