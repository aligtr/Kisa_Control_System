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
#include "semphr.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct 
{
  uint16_t cmd_vel;
  uint16_t cmd_ang;
}TcpCmd_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/*Static IP ADDRESS*/
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   0
#define IP_ADDR3   10
   
/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   0
#define GW_ADDR3   1 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern CAN_HandleTypeDef hcan3;
extern SPI_HandleTypeDef hspi3;
extern TIM_HandleTypeDef htim1;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId vKinematicaHandle;
osThreadId vPDUReaderHandle;
osThreadId vMotorControlHandle;
osThreadId vServoControlHandle;
osMessageQId xQueuePDUDateHandle;
osMessageQId xQueueVelDateHandle;
osMessageQId xQueueAngleDateHandle;
osMessageQId xQueueTcpCmdHandle;
osSemaphoreId dmaReciveSemaphoreHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* Private function prototypes -----------------------------------------------*/
void Netif_Config(void);
/* USER CODE END FunctionPrototypes */

void vTcpTask(void const * argument);
void vKinematicaTask(void const * argument);
void vPDUReaderTask(void const * argument);
void vMotorControlTask(void const * argument);
void vServoControlTask(void const * argument);

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

  /* Create the semaphores(s) */
  /* definition and creation of dmaReciveSemaphore */
  osSemaphoreDef(dmaReciveSemaphore);
  dmaReciveSemaphoreHandle = osSemaphoreCreate(osSemaphore(dmaReciveSemaphore), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  xSemaphoreTake(dmaReciveSemaphoreHandle,0);
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

  /* definition and creation of xQueueTcpCmd */
  osMessageQDef(xQueueTcpCmd, 10, TcpCmd_t);
  xQueueTcpCmdHandle = osMessageCreate(osMessageQ(xQueueTcpCmd), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, vTcpTask, osPriorityIdle, 0, 512);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of vKinematica */
  osThreadDef(vKinematica, vKinematicaTask, osPriorityHigh, 0, 512);
  vKinematicaHandle = osThreadCreate(osThread(vKinematica), NULL);

  /* definition and creation of vPDUReader */
  osThreadDef(vPDUReader, vPDUReaderTask, osPriorityNormal, 0, 512);
  vPDUReaderHandle = osThreadCreate(osThread(vPDUReader), NULL);

  /* definition and creation of vMotorControl */
  osThreadDef(vMotorControl, vMotorControlTask, osPriorityAboveNormal, 0, 512);
  vMotorControlHandle = osThreadCreate(osThread(vMotorControl), NULL);

  /* definition and creation of vServoControl */
  osThreadDef(vServoControl, vServoControlTask, osPriorityAboveNormal, 0, 512);
  vServoControlHandle = osThreadCreate(osThread(vServoControl), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_vTcpTask */
/**
  * @brief  Function implementing the tcpTask thread.
  * @param  argument: Not used
  * @retval None
  */

uint16_t pow(uint16_t num,uint16_t pow)
{
  uint16_t i,rez=1;
  for(i=1;i<=pow;i++)
  {
    rez*=num;
  }
  return rez;
}

/* USER CODE END Header_vTcpTask */
void vTcpTask(void const * argument)
{
  /* USER CODE BEGIN vTcpTask */
  // struct netconn *conn, *newconn;
  // err_t err, accept_err;
  // struct netbuf* buf;
  // uint8_t* data;
  // uint16_t len;
  // err_t recv_err;
  // TcpCmd_t cmd;
  // uint8_t cmd_data[5];
  // int8_t i, i_pow;
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END vTcpTask */
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
  Motors[0].motorID=WFL;
  Motors[1].motorID=WFR;
  Motors[2].motorID=WRL;
  Motors[3].motorID=WRR;
  Motors[0].status=STOP;
  Motors[1].status=STOP;
  Motors[2].status=STOP;
  Motors[3].status=STOP;
  Motors[0].prevRefImpact=0;
  Motors[1].prevRefImpact=0;
  Motors[2].prevRefImpact=0;
  Motors[3].prevRefImpact=0;
  servoTarget_t Servos;
  PduData_t pduData;
  uint8_t i=0;
  tim1Init();
  tim12Init();
  tim8Init();
  tim9Init();
  NVIC_EnableIRQ(TIM1_CC_IRQn);
	NVIC_EnableIRQ(TIM8_CC_IRQn);
	NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
	NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn);
	while(1){
		if (xQueueReceive(xQueuePDUDateHandle,&pduData,portMAX_DELAY)==pdTRUE)
    {
      if(pduData.elevdr==1 && (Motors[0].status==STOP || Motors[1].status==STOP || Motors[2].status==STOP || Motors[3].status==STOP))
      {
        Motors[0].command=MOTOR_START;
        Motors[1].command=MOTOR_START;
        Motors[2].command=MOTOR_START;
        Motors[3].command=MOTOR_START;
        for (i=0; i<4; i++)
        {
          xQueueSendToBack(xQueueVelDateHandle,&Motors[i],0);
        }
        Motors[0].status=RUN;
        Motors[1].status=RUN;
        Motors[2].status=RUN;
        Motors[3].status=RUN;
      }
      if(pduData.elevdr==0 && (Motors[0].status==RUN || Motors[1].status==RUN || Motors[2].status==RUN || Motors[3].status==RUN))
      {
        Motors[0].command=MOTOR_STOP;
        Motors[1].command=MOTOR_STOP;
        Motors[2].command=MOTOR_STOP;
        Motors[3].command=MOTOR_STOP;
        for (i=0; i<4; i++)
        {
          xQueueSendToBack(xQueueVelDateHandle,&Motors[i],0);
        }
        Motors[0].status=STOP;
        Motors[1].status=STOP;
        Motors[2].status=STOP;
        Motors[3].status=STOP;
      } 
      if(pduData.elevdr==1)
      {
        normaliz(pduData.vel_mean, pduData.rx_mean,pduData.dir_mean, pduData.ry_mean,pduData.elevdr);
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
  TcpCmd_t cmd;
	uint16_t channels[18];
  uint8_t str[200];
	while (1)
  {
		// receiveSBusDate(channels);
    	
    tim2Init1();
    tim3Init1();
    tim4Init1();
    //tim5Init1();
    osDelay(50);	
		pduData.vel_mean=450;
		pduData.dir_mean=TIM3->CCR2;
		pduData.rx_mean=TIM4->CCR2;
    pduData.elevdr=TIM2->CCR2;
    //pduData.ry_mean=TIM5->CCR2;
    tim2Init2();
    tim3Init2();
    tim4Init2();
    osDelay(50);
		pduData.rudr=TIM2->CCR1;
		pduData.ry_mean=TIM4->CCR1;
		
    // sprintf(str,"Vel_mean:%d dir_mean:%d rx_mean:%d ry_mean:%d elevdr:%d aildr:%d gear:%d rudr:%d \n\r",pduData.vel_mean,pduData.dir_mean,pduData.rx_mean,pduData.ry_mean,pduData.elevdr,pduData.aildr,pduData.gear,pduData.rudr);
		// HAL_UART_Transmit(&huart3,str,strlen(str),100);
    if(checkDate(pduData.vel_mean) && checkDate(pduData.dir_mean) && checkDate(pduData.rx_mean) && \
				checkDate(pduData.elevdr) && checkDate(pduData.ry_mean)) 
    {						
			pduData.elevdr=checkLevel(pduData.elevdr);
      //pduData.aildr=checkLevel(pduData.aildr);
      // pduData.gear=checkLevel(pduData.gear);
      pduData.rudr=checkLevel(pduData.rudr);
		  xQueueSendToBack(xQueuePDUDateHandle, &pduData, 0);
			// xTaskSuspend(vKinematicaHandle);
			// xTaskSuspend(vServoControlHandle);//date ready
		}
		else
    {
			// xTaskResume(vKinematicaHandle);
			// xTaskResume(vServoControlHandle);//no pdu
		}
	
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
      //motorRealeseCommand(motor);
      osDelay(10);
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
  uint8_t i=0;
  servo_t Servos[4];
  Motor_t motor;
  servoTarget_t servotarget;
  float gm;
  Servos[SERVO_FL].servoId=SFL;
  Servos[SERVO_FR].servoId=SFR;
  Servos[SERVO_RL].servoId=SRL;
  Servos[SERVO_RR].servoId=SRR; 
  
  Servos[SERVO_FL].targetAngle=0;
  Servos[SERVO_FR].targetAngle=0;
  Servos[SERVO_RL].targetAngle=0;
  Servos[SERVO_RR].targetAngle=0;

  Servos[SERVO_FL].currentAngle=0;
  Servos[SERVO_FR].currentAngle=0;
  Servos[SERVO_RL].currentAngle=0;
  Servos[SERVO_RR].currentAngle=0;

  Servos[SERVO_FL].initFlag=1;
  Servos[SERVO_FR].initFlag=1;
  Servos[SERVO_RL].initFlag=1;
  Servos[SERVO_RR].initFlag=1;

  // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
  // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
  // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
  // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
  // dirInit();
  /* Infinite loop */
  while(1)
  {
    // if(xQueueReceive(xQueueAngleDateHandle,&servotarget,0)==pdTRUE)
    // {
    //    Servos[SERVO_FL].targetAngle=servotarget.targetFrontLeft;
    //    Servos[SERVO_FR].targetAngle=servotarget.targetFrontRight;
    //    Servos[SERVO_RL].targetAngle=servotarget.targetRearLeft;
    //    Servos[SERVO_RR].targetAngle=servotarget.targetRearRight;
       
    // }		
    // TIM1->CCR1=700;
    // TIM1->ARR=1500;
    //HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,0);
    // HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
    //TIM12->CCR1=50;
    // for (i = 0; i < 4; i++)
    // {
    //   getCurrentAngle(i, &Servos[i], &hspi3);
    // } 
    
    // Servos[SERVO_FL].dAngle=fabs(Servos[SERVO_FL].targetAngle-Servos[SERVO_FL].currentAngle); 
    // Servos[SERVO_FR].dAngle=fabs(Servos[SERVO_FR].targetAngle-Servos[SERVO_FR].currentAngle);
    // Servos[SERVO_RL].dAngle=fabs(Servos[SERVO_RL].targetAngle-Servos[SERVO_RL].currentAngle);
    // Servos[SERVO_RR].dAngle=fabs(Servos[SERVO_RR].targetAngle-Servos[SERVO_RR].currentAngle);	

    // Servos[SERVO_FL].dir=intSign(Servos[SERVO_FL].targetAngle-Servos[SERVO_FL].currentAngle);
    // Servos[SERVO_FR].dir=intSign(Servos[SERVO_FR].targetAngle-Servos[SERVO_FR].currentAngle);
    // Servos[SERVO_RL].dir=intSign(Servos[SERVO_RL].targetAngle-Servos[SERVO_RL].currentAngle);
    // Servos[SERVO_RR].dir=intSign(Servos[SERVO_RR].targetAngle-Servos[SERVO_RR].currentAngle);

    // gm=Servos[SERVO_FL].dAngle;
    // if (gm<Servos[SERVO_FR].dAngle) gm=Servos[SERVO_FR].dAngle;
    // if (gm<Servos[SERVO_RL].dAngle) gm=Servos[SERVO_RL].dAngle;
    // if (gm<Servos[SERVO_RR].dAngle) gm=Servos[SERVO_RR].dAngle;
    
    // TIM1->ARR=150*gm/Servos[SERVO_FL].dAngle;
    // TIM8->ARR=150*gm/Servos[SERVO_FR].dAngle;
    // TIM9->ARR=150*gm/Servos[SERVO_RL].dAngle;
    // TIM12->ARR=150*gm/Servos[SERVO_RR].dAngle;
    // for (i = 0; i < 4; i++) setServo(Servos[i],i);
    vTaskDelay(100);
	}
  /* USER CODE END vServoControlTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
