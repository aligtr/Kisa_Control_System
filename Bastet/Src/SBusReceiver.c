#include "SBusReceiver.h"
#include "main.h"

extern UART_HandleTypeDef huart1;
extern osSemaphoreId dmaReciveSemaphoreHandle;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart==&huart1)
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(dmaReciveSemaphoreHandle,&xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}	
}

uint8_t buffer[100];

void receiveSBusDate(uint16_t* channels)
{
	uint8_t i=0;
	HAL_UART_Receive_DMA(&huart1,(uint8_t *)buffer,100);
	if(xSemaphoreTake(dmaReciveSemaphoreHandle,portMAX_DELAY)==pdTRUE)
	{
		for (i = 0; i < 100; i++)
		{
			if (buffer[i] == 0x0f) break;
		}
		channels[RIGHT_HORIZ]  = ((buffer[i+1]    |buffer[i+2]<<8)                 & 0x07FF);
		channels[RIGHT_VERT]  = ((buffer[i+2]>>3 |buffer[i+3]<<5)                 & 0x07FF);
		channels[LEFT_VERT]  = ((buffer[i+3]>>6 |buffer[i+4]<<2 |buffer[i+5]<<10)  & 0x07FF);
		channels[LEFT_HORIZ]  = ((buffer[i+5]>>1 |buffer[i+6]<<7)                 & 0x07FF);
		channels[RUDDR]  = ((buffer[i+6]>>4 |buffer[i+7]<<4)                 & 0x07FF);
		channels[ELEVDR]  = ((buffer[i+7]>>7 |buffer[i+8]<<1 |buffer[i+9]<<9)   & 0x07FF);
		channels[AILDR]  = ((buffer[i+9]>>2 |buffer[i+10]<<6)                & 0x07FF);
		channels[GEAR]  = ((buffer[i+10]>>5|buffer[i+11]<<3)                & 0x07FF);
		channels[8]  = ((buffer[i+12]   |buffer[i+13]<<8)                & 0x07FF);
		channels[9]  = ((buffer[i+13]>>3|buffer[i+14]<<5)                & 0x07FF);
		channels[10] = ((buffer[i+14]>>6|buffer[i+15]<<2|buffer[i+16]<<10) & 0x07FF);
		channels[11] = ((buffer[i+16]>>1|buffer[i+17]<<7)                & 0x07FF);
		channels[12] = ((buffer[i+17]>>4|buffer[i+18]<<4)                & 0x07FF);
		channels[13] = ((buffer[i+18]>>7|buffer[i+19]<<1|buffer[i+20]<<9)  & 0x07FF);
		channels[14] = ((buffer[i+20]>>2|buffer[i+21]<<6)                & 0x07FF);
		channels[15] = ((buffer[i+21]>>5|buffer[i+22]<<3)                & 0x07FF);
		channels[16] =((buffer[i+23])      & 0x0001) ?  2047: 0;
		channels[17] =((buffer[i+23] >> 1) & 0x0001) ?  2047: 0;
	}
}