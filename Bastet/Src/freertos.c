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
/* USER CODE END Variables */
osThreadId tcpTaskHandle;
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
  /* definition and creation of tcpTask */
  osThreadDef(tcpTask, vTcpTask, osPriorityAboveNormal, 0, 3000);
  tcpTaskHandle = osThreadCreate(osThread(tcpTask), NULL);

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
    /* Create a new connection identifier. */
    // conn = netconn_new(NETCONN_TCP);
    // if (conn != NULL)
    // {
    //     /* Bind connection to well known port number 80. */
    //     err = netconn_bind(conn, NULL, 80);
    //     if (err == ERR_OK)
    //     {
    //         /* Tell connection to go into listening mode. */
    //         netconn_listen(conn);
    //         while (1)
    //         {
    //             /* Grab new connection. */
    //             accept_err = netconn_accept(conn, &newconn);

    //             /* Process the new connection. */
    //             if (accept_err == ERR_OK)
    //             {
    //                while ((recv_err = netconn_recv(newconn, &buf)) == ERR_OK)
    //                 {
    //                     do
    //                     {
    //                         netbuf_data(buf, &data, &len);
    //                         //netconn_write(newconn, data, len, NETCONN_COPY);
    //                     } while (netbuf_next(buf) >= 0);
    //                     netbuf_delete(buf);
    //                     if (len>=3)
    //                     {
    //                         cmd.cmd_ang=0;
    //                         cmd.cmd_vel=0;
    //                         i=len-1;
    //                         i_pow=0;
    //                         while(data[i]!=','){
    //                             cmd.cmd_ang += (data[i]-0x30)*pow(10,i_pow);
    //                             i--;
    //                             i_pow++;
    //                         }                       
    //                         i--;                
    //                         i_pow=0;
    //                         while(data[i]!='/')
    //                         {
    //                             cmd.cmd_vel += (data[i]-0x30)*pow(10,i_pow);
    //                             i--;
    //                             i_pow++;
    //                         }
    //                         xQueueSendToBack(xQueueTcpCmdHandle,&cmd,0);
    //                     }
    //                 }
                  
    //                 /* Close connection and discard connection identifier. */
    //                 netconn_close(newconn);
    //                 netconn_delete(newconn);
    //             }
    //         }
    //     }
    //     else
    //     {
    //         netconn_delete(newconn);
    //     }
    // }
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
	while(1){
		if (xQueueReceive(xQueuePDUDateHandle,&pduData,portMAX_DELAY)==pdTRUE)
    {
      if(pduData.gear==1 && (Motors[0].status==STOP || Motors[1].status==STOP || Motors[2].status==STOP || Motors[3].status==STOP))
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
      if(pduData.gear==0 && (Motors[0].status==RUN || Motors[1].status==RUN || Motors[2].status==RUN || Motors[3].status==RUN))
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
      if(pduData.gear==1)
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
	while (1)
  {
		receiveSBusDate(channels);
		pduData.vel_mean=channels[RIGHT_VERT];
		pduData.dir_mean=channels[RIGHT_HORIZ];
		pduData.rx_mean=channels[LEFT_VERT];
    pduData.ry_mean=channels[LEFT_HORIZ];
		pduData.elevdr=channels[ELEVDR];
		pduData.aildr=channels[AILDR];
		pduData.gear=channels[GEAR];
		pduData.rudr=channels[RUDDR];
		if(checkDate(pduData.vel_mean) && checkDate(pduData.dir_mean) && checkDate(pduData.rx_mean) && \
				checkDate(pduData.elevdr) && checkDate(pduData.aildr) && checkDate(pduData.gear) && checkDate(pduData.ry_mean)) 
    {						
			pduData.elevdr=checkLevel(pduData.elevdr);
      pduData.aildr=checkLevel(pduData.aildr);
      pduData.gear=checkLevel(pduData.gear);
      pduData.rudr=checkLevel(pduData.rudr);
      if(pduData.elevdr==1)
      {
        if (xQueueReceive(xQueueTcpCmdHandle,&cmd,portMAX_DELAY)==pdTRUE)
        {          
          pduData.vel_mean=cmd.cmd_vel;
          pduData.dir_mean=cmd.cmd_ang;
        }  
      }
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

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
  dirInit();
  tim1Init();
  tim2Init();
  tim8Init();
  tim9Init();
  /* Infinite loop */
  while(1)
  {
    if(xQueueReceive(xQueueAngleDateHandle,&servotarget,0)==pdTRUE)
    {
       Servos[SERVO_FL].targetAngle=servotarget.targetFrontLeft;
       Servos[SERVO_FR].targetAngle=servotarget.targetFrontRight;
       Servos[SERVO_RL].targetAngle=servotarget.targetRearLeft;
       Servos[SERVO_RR].targetAngle=servotarget.targetRearRight;
    }		
    for (i = 0; i < 4; i++)
    {
      getCurrentAngle(i, &Servos[i], &hspi3);
    } 
    
    Servos[SERVO_FL].dAngle=fabs(Servos[SERVO_FL].targetAngle-Servos[SERVO_FL].currentAngle); 
    Servos[SERVO_FR].dAngle=fabs(Servos[SERVO_FR].targetAngle-Servos[SERVO_FR].currentAngle);
    Servos[SERVO_RL].dAngle=fabs(Servos[SERVO_RL].targetAngle-Servos[SERVO_RL].currentAngle);
    Servos[SERVO_RR].dAngle=fabs(Servos[SERVO_RR].targetAngle-Servos[SERVO_RR].currentAngle);	

    Servos[SERVO_FL].dir=intSign(Servos[SERVO_FL].targetAngle-Servos[SERVO_FL].currentAngle);
    Servos[SERVO_FR].dir=intSign(Servos[SERVO_FR].targetAngle-Servos[SERVO_FR].currentAngle);
    Servos[SERVO_RL].dir=intSign(Servos[SERVO_RL].targetAngle-Servos[SERVO_RL].currentAngle);
    Servos[SERVO_RR].dir=intSign(Servos[SERVO_RR].targetAngle-Servos[SERVO_RR].currentAngle);

    gm=Servos[SERVO_FL].dAngle;
    if (gm<Servos[SERVO_FR].dAngle) gm=Servos[SERVO_FR].dAngle;
    if (gm<Servos[SERVO_RL].dAngle) gm=Servos[SERVO_RL].dAngle;
    if (gm<Servos[SERVO_RR].dAngle) gm=Servos[SERVO_RR].dAngle;
    
    TIM1->ARR=150*gm/Servos[SERVO_FL].dAngle;
    TIM8->ARR=150*gm/Servos[SERVO_FR].dAngle;
    TIM9->ARR=150*gm/Servos[SERVO_RL].dAngle;
    TIM12->ARR=150*gm/Servos[SERVO_RR].dAngle;
    for (i = 0; i < 4; i++) setServo(Servos[i],i);
    vTaskDelay(100);
	}
  /* USER CODE END vServoControlTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
