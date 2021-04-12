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
#include "SBusReceiver.h"
#include "ServoControl.h"
#include "pdu.h"
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
  osMessageQDef(xQueuePDUDate, 5, PduData_t);
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
  osThreadDef(defaultTask, StartDefaultTask, 0, 0, 512);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of vKinematica */
  osThreadDef(vKinematica, vKinematicaTask, 6, 0, 512);
  vKinematicaHandle = osThreadCreate(osThread(vKinematica), NULL);

  /* definition and creation of vPDUReader */
  osThreadDef(vPDUReader, vPDUReaderTask, 4, 0, 512);
  vPDUReaderHandle = osThreadCreate(osThread(vPDUReader), NULL);

  /* definition and creation of vMotorControl */
  osThreadDef(vMotorControl, vMotorControlTask, 5, 0, 512);
  vMotorControlHandle = osThreadCreate(osThread(vMotorControl), NULL);

  /* definition and creation of vServoControl */
  osThreadDef(vServoControl, vServoControlTask, 3, 0, 256);
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
void vKinematicaTask(void const * argument)
{
  /* USER CODE BEGIN vKinematicaTask */
  /* Infinite loop */
  Motor_t Motors[4];
  Motors[0].command=CHANGE_SPEED;
  Motors[1].command=CHANGE_SPEED;
  Motors[2].command=CHANGE_SPEED;
  Motors[3].command=CHANGE_SPEED;
  servoTarget_t Servos;
  PduData_t pduData;
  uint8_t i=0;
	while(1){
		if (xQueueReceive(xQueuePDUDateHandle,&pduData,portMAX_DELAY)==pdTRUE)
    {
			normaliz(pduData.vel_mean, pduData.rx_mean, pduData.dir_mean, pduData.ry_mean);
			kinematica(pduData.mode, &Motors, &Servos);
      for (i=0; i<4; i++){
        xQueueSendToBack(xQueueAngleDateHandle,&Servos,0);
      } 
      for (i=0; i<4; i++)
      {
        if(Motors[i].refImpact!=Motors[i].prevRefImpact)
        {
          xQueueSendToBack(xQueueVelDateHandle,&Motors[i],0);
        }
      } 
    }
  }
  /* USER CODE END vKinematicaTask */
}

/* USER CODE BEGIN Header_vPDUReaderTask */
/**
* @brief Function implementing the vPDUReader thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vPDUReaderTask */
void vPDUReaderTask(void const * argument)
{
  /* USER CODE BEGIN vPDUReaderTask */
  /* Infinite loop */
  PduData_t pduData;
	uint16_t channels[18];
	while (1)
  {
		receiveSBusDate(channels);
		pduData.vel_mean=channels[RIGHT_VERT];
		pduData.dir_mean=channels[RIGHT_HORIZ];
		pduData.rx_mean=channels[LEFT_VERT];
		pduData.smt_src1=channels[ELEVDR];
		pduData.smt_src2=channels[AILDR];
		pduData.smt_src3=channels[GEAR];
		pduData.smt_src4=channels[RUDDR];
		pduData.ry_mean=channels[LEFT_HORIZ];
		if(checkDate(pduData.vel_mean) && checkDate(pduData.dir_mean) && checkDate(pduData.rx_mean) && \
					checkDate(pduData.smt_src1) && checkDate(pduData.smt_src2) && checkDate(pduData.smt_src3) && checkDate(pduData.ry_mean)) 
    {
					// if(smt_src2>700 || smt_src2<200)
					// {
					// 	reset();
					// }				
						
		  xQueueSendToBack(xQueuePDUDateHandle, &pduData, 0);
			// xTaskSuspend(vKinematicaHandle);
			// xTaskSuspend(vServoControlHandle);//date ready
		}
		else
    {
			// xTaskResume(vKinematicaHandle);
			// xTaskResume(vServoControlHandle);//no pdu
		}
		vTaskDelay(100);			
	}
  /* USER CODE END vPDUReaderTask */
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
  for(;;)
  {
    if(xQueueReceive(xQueueVelDateHandle,&motor,portMAX_DELAY)==pdTRUE)
    {
      motorRealeseCommand(motor);
    }
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
void vServoControlTask(void const * argument)
{
  /* USER CODE BEGIN vServoControlTask */
  /* Infinite loop */
  uint8_t i=0;
  servo_t Servos[4];
  Motor_t motor;
  servoTarget_t servotarget;
  motor.command=CHANGE_TORQUE;
  Servos[SERVO_FL].servoId=SFL;
  Servos[SERVO_FR].servoId=SFR;
  Servos[SERVO_RL].servoId=SRL;
  Servos[SERVO_RR].servoId=SRR; 
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
	while(1)
  {
    if(xQueueReceive(xQueueAngleDateHandle,&servotarget,0)==pdTRUE)
    {
       Servos[SERVO_FL].targetAngle=servotarget.targetFrontLeft;
       Servos[SERVO_FR].targetAngle=servotarget.targetFrontRight;
       Servos[SERVO_RL].targetAngle=servotarget.targetRearLeft;
       Servos[SERVO_RR].targetAngle=servotarget.targetRearRight;
    }		
    getCurrentAngle(i,&Servos[i], &hspi3);
    for(i=0;i<4;i++)
    {
      motor.prevRefImpact=Servos[i].torque;
      PI_control(&Servos[i]);
      motor.refImpact=Servos[i].torque;
      if(motor.refImpact!=motor.prevRefImpact)
      {
        motor.motorID=Servos[i].servoId;
        xQueueSendToBack(xQueueVelDateHandle,&motor,0);
      }
    }
    vTaskDelay(100);
	}
  /* USER CODE END vServoControlTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
