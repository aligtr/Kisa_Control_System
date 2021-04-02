/**
  ******************************************************************************
  * File Name          : CAN.c
  * Description        : This file provides code for the configuration
  *                      of the CAN instances.
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

/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

CAN_HandleTypeDef hcan3;

/* CAN3 init function */
void MX_CAN3_Init(void)
{

  hcan3.Instance = CAN3;
  hcan3.Init.Prescaler = 6;
  hcan3.Init.Mode = CAN_MODE_NORMAL;
  hcan3.Init.SyncJumpWidth = CAN_SJW_2TQ;
  hcan3.Init.TimeSeg1 = CAN_BS1_15TQ;
  hcan3.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan3.Init.TimeTriggeredMode = DISABLE;
  hcan3.Init.AutoBusOff = DISABLE;
  hcan3.Init.AutoWakeUp = DISABLE;
  hcan3.Init.AutoRetransmission = DISABLE;
  hcan3.Init.ReceiveFifoLocked = DISABLE;
  hcan3.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan3) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN3)
  {
  /* USER CODE BEGIN CAN3_MspInit 0 */

  /* USER CODE END CAN3_MspInit 0 */
    /* CAN3 clock enable */
    __HAL_RCC_CAN3_CLK_ENABLE();
    __HAL_RCC_CAN2_CLK_ENABLE();
    __HAL_RCC_CAN1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN3 GPIO Configuration    
    PA8     ------> CAN3_RX
    PA15     ------> CAN3_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_CAN3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN3 interrupt Init */
    HAL_NVIC_SetPriority(CAN3_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN3_RX0_IRQn);
  /* USER CODE BEGIN CAN3_MspInit 1 */

  /* USER CODE END CAN3_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN3)
  {
  /* USER CODE BEGIN CAN3_MspDeInit 0 */

  /* USER CODE END CAN3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN3_CLK_DISABLE();
    __HAL_RCC_CAN2_CLK_DISABLE();
    __HAL_RCC_CAN1_CLK_DISABLE();
  
    /**CAN3 GPIO Configuration    
    PA8     ------> CAN3_RX
    PA15     ------> CAN3_TX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8|GPIO_PIN_15);

    /* CAN3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN3_RX0_IRQn);
  /* USER CODE BEGIN CAN3_MspDeInit 1 */

  /* USER CODE END CAN3_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
// uint8_t CANTransmitt(CAN_HandleTypeDef* canHandle,uint8_t* data,uint32_t messageID)
// {
//   uint8_t rez;
//   uint8_t TxData[8];
//    TxData[0] = 0x0;
//   TxData[1] = 0xAD;
//   TxData[2] = 0xDE;
//   TxData[3] = 0xAD;
//   TxData[4] = 0xBE;
//   TxData[5] = 0xEF;
//   TxData[6] = 0xFA;
//   TxData[7] = 0xCE;
//   CAN_TxHeaderTypeDef msgHeader;
//   if(HAL_CAN_GetTxMailboxesFreeLevel(canHandle)!=0)
//   {
//     msgHeader.StdId=0;
//     msgHeader.DLC=8;
//     msgHeader.TransmitGlobalTime=DISABLE;
//     msgHeader.RTR=CAN_RTR_DATA;
//     msgHeader.IDE=CAN_ID_STD;
//     HAL_CAN_AddTxMessage(&hcan3,&msgHeader,TxData,rez);
//     return rez; 
//   }
//   else
//   {
//     return 0xFF;
//   }
// }

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef msgHeader;
  uint32_t msgId = 0;
  uint8_t msgData[8];
  
  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &msgHeader, msgData);
    
  if (msgHeader.IDE == CAN_ID_EXT)
  {
    msgId = msgHeader.ExtId;
  }
  else
  {
    msgId = msgHeader.StdId;
  }
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
