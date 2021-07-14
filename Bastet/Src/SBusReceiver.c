#include "SBusReceiver.h"
#include "main.h"


extern osSemaphoreId dmaReciveSemaphoreHandle;

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
// 	if(huart==&huart1)
// 	{
// 		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
// 		xSemaphoreGiveFromISR(dmaReciveSemaphoreHandle,&xHigherPriorityTaskWoken);
// 		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
// 	}	
// }

void tim2Init1(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;//Тактирование потра A
	GPIOB->MODER&=~GPIO_MODER_MODER3_1;;//PA5 на AF
	GPIOB->AFR[0]&=~(1<<12);
	GPIOA->MODER|=GPIO_MODER_MODER5_1;;//PA5 на AF
	GPIOA->AFR[0]|=1<<20;
	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;//Включить тактирование таймера-счетчика 9
	TIM2->PSC=1680/4;//Делитель на 10КГц
	TIM2->ARR=0xFFFF;//Считаем до конца
	
	TIM2->CR1=0;
	TIM2->CCER=0;
	TIM2->SMCR=0;
	TIM2->DIER=0;
	TIM2->CCMR1=0;
	
	//Канал 1
	TIM2->CCMR1|=TIM_CCMR1_CC1S_0;
	TIM2->CCMR1|=TIM_CCMR1_IC1F_0;
	//Канал 2
	TIM2->CCMR1|=TIM_CCMR1_CC2S_1;
	TIM2->CCMR1|=TIM_CCMR1_IC2F_0;
	
	TIM2->SMCR|=TIM_SMCR_TS_0 | TIM_SMCR_TS_2;
	TIM2->DIER|=TIM_DIER_CC1IE;
	//TIM2->DIER|=TIM_DIER_CC2IE;
	TIM2->SMCR|=TIM_SMCR_SMS_2;//Master mode
	
	TIM2->CCER|=TIM_CCER_CC2P;
	
	TIM2->CCER|=TIM_CCER_CC1E;//Режим захвата
	TIM2->CCER|=TIM_CCER_CC2E;//Режим захвата
	
	TIM2->CR1|=TIM_CR1_CEN;
}

void tim2Init2(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;//Тактирование потра A
	GPIOA->MODER&=~GPIO_MODER_MODER5_1;;//PA5 на AF
	GPIOA->AFR[0]&=~(1<<20);
	GPIOB->MODER|=GPIO_MODER_MODER3_1;;//PB3 на AF
	GPIOB->AFR[0]|=(1<<12);
	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;//Включить тактирование таймера-счетчика 9
	TIM2->PSC=1680/4;//Делитель на 10КГц
	TIM2->ARR=0xFFFF;//Считаем до конца
	
	TIM2->CR1=0;
	TIM2->CCER=0;
	TIM2->SMCR=0;
	TIM2->DIER=0;
	TIM2->CCMR1=0;
	
	//Канал 1
	TIM2->CCMR1|=TIM_CCMR1_CC1S_1;
	TIM2->CCMR1|=TIM_CCMR1_IC1F_0;
	//Канал 2
	TIM2->CCMR1|=TIM_CCMR1_CC2S_0;
	TIM2->CCMR1|=TIM_CCMR1_IC2F_0;
	
	TIM2->SMCR|=TIM_SMCR_TS_1 | TIM_SMCR_TS_2;
	TIM2->DIER|=TIM_DIER_CC1IE;
	
	TIM2->SMCR|=TIM_SMCR_SMS_2;//Master mode

	TIM2->CCER|=TIM_CCER_CC1P;
	
	TIM2->CCER|=TIM_CCER_CC1E;//Режим захвата
	TIM2->CCER|=TIM_CCER_CC2E;//Режим захвата
	
	TIM2->CR1|=TIM_CR1_CEN;
}

void tim3Init1(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;//Тактирование потра A
	GPIOA->MODER&=~GPIO_MODER_MODER7_1;;//PA6 на AF
	GPIOA->AFR[0]&=~(2<<28);
	GPIOA->MODER|=GPIO_MODER_MODER6_1;;//PA6 на AF
	GPIOA->AFR[0]|=2<<24;
	RCC->APB1ENR|=RCC_APB1ENR_TIM3EN;//Включить тактирование таймера-счетчика 9
	TIM3->PSC=1680/4;//Делитель на 10КГц
	TIM3->ARR=0xFFFF;//Считаем до конца

	TIM3->CR1=0;
	TIM3->CCER=0;
	TIM3->SMCR=0;
	TIM3->DIER=0;
	TIM3->CCMR1=0;
	//Канал 1
	TIM3->CCMR1|=TIM_CCMR1_CC1S_0;
	TIM3->CCMR1|=TIM_CCMR1_IC1F_0;
	//Канал 2
	TIM3->CCMR1|=TIM_CCMR1_CC2S_1;
	TIM3->CCMR1|=TIM_CCMR1_IC2F_0;
	
	TIM3->SMCR|=TIM_SMCR_TS_0 | TIM_SMCR_TS_2;
	TIM3->DIER|=TIM_DIER_CC1IE;
	
	TIM3->SMCR|=TIM_SMCR_SMS_2;//Master mode
	
	TIM3->CCER|=TIM_CCER_CC2P;
	
	TIM3->CCER|=TIM_CCER_CC1E;//Режим захвата
	TIM3->CCER|=TIM_CCER_CC2E;//Режим захвата
	
	TIM3->CR1|=TIM_CR1_CEN;
}
void tim3Init2(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;//Тактирование потра A
	GPIOA->MODER&=~GPIO_MODER_MODER6_1;;//PA6 на AF
	GPIOA->AFR[0]&=~(2<<24);
	GPIOA->MODER|=GPIO_MODER_MODER7_1;;//PA6 на AF
	GPIOA->AFR[0]|=2<<28;
	RCC->APB1ENR|=RCC_APB1ENR_TIM3EN;//Включить тактирование таймера-счетчика 9
	TIM3->PSC=1680/4;//Делитель на 10КГц
	TIM3->ARR=0xFFFF;//Считаем до конца

	TIM3->CR1=0;
	TIM3->CCER=0;
	TIM3->SMCR=0;
	TIM3->DIER=0;
	TIM3->CCMR1=0;
	//Канал 1
	TIM3->CCMR1|=TIM_CCMR1_CC1S_1;
	TIM3->CCMR1|=TIM_CCMR1_IC1F_0;
	//Канал 2
	TIM3->CCMR1|=TIM_CCMR1_CC2S_0;
	TIM3->CCMR1|=TIM_CCMR1_IC2F_0;
	
	TIM3->SMCR|=TIM_SMCR_TS_1 | TIM_SMCR_TS_2;
	TIM3->DIER|=TIM_DIER_CC1IE;
	
	TIM3->SMCR|=TIM_SMCR_SMS_2;//Master mode
	
	TIM3->CCER|=TIM_CCER_CC1P;
	
	TIM3->CCER|=TIM_CCER_CC1E;//Режим захвата
	TIM3->CCER|=TIM_CCER_CC2E;//Режим захвата
	
	TIM3->CR1|=TIM_CR1_CEN;
}

void tim4Init1(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIODEN;//Тактирование потра D
	GPIOD->MODER&=~GPIO_MODER_MODER13_1;//PD13 на AF
	GPIOD->AFR[1]&=~(2<<20);
	GPIOD->MODER|=GPIO_MODER_MODER12_1;;//PD12 на AF
	GPIOD->AFR[1]|=(2<<16);
	RCC->APB1ENR|=RCC_APB1ENR_TIM4EN;//Включить тактирование таймера-счетчика 9
	TIM4->PSC=1680/4;//Делитель на 10КГц
	TIM4->ARR=0xFFFF;//Считаем до конца

	TIM4->CR1=0;
	TIM4->CCER=0;
	TIM4->SMCR=0;
	TIM4->DIER=0;
	TIM4->CCMR1=0;
		
	//Канал 1
	TIM4->CCMR1|=TIM_CCMR1_CC1S_0;
	TIM4->CCMR1|=TIM_CCMR1_IC1F_0;
	//Канал 2
	TIM4->CCMR1|=TIM_CCMR1_CC2S_1;
	TIM4->CCMR1|=TIM_CCMR1_IC2F_0;
	TIM4->CCER|=TIM_CCER_CC2P;
	
	TIM4->SMCR|=TIM_SMCR_TS_0 | TIM_SMCR_TS_2;
	TIM4->DIER|=TIM_DIER_CC1IE;
	TIM4->DIER|=TIM_DIER_CC2IE;
	TIM4->SMCR|=TIM_SMCR_SMS_2;//Master mode
	
	TIM4->CCER|=TIM_CCER_CC1E;//Режим захвата
	TIM4->CCER|=TIM_CCER_CC2E;//Режим захвата
	TIM4->CR1|=TIM_CR1_CEN;
}

void tim4Init2(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIODEN;//Тактирование потра A
	GPIOD->MODER&=~GPIO_MODER_MODER12_1;;//PD12 на AF
	GPIOD->AFR[1]&=~(2<<16);
	GPIOD->MODER|=GPIO_MODER_MODER13_1;//PD13 на AF
	GPIOD->AFR[1]|=(2<<20);
	RCC->APB1ENR|=RCC_APB1ENR_TIM4EN;//Включить тактирование таймера-счетчика 9
	TIM4->PSC=1680/4;//Делитель на 10КГц
	TIM4->ARR=0xFFFF;//Считаем до конца

	TIM4->CR1=0;
	TIM4->CCER=0;
	TIM4->SMCR=0;
	TIM4->DIER=0;
	TIM4->CCMR1=0;
		
	//Канал 1
	TIM4->CCMR1|=TIM_CCMR1_CC1S_1;
	TIM4->CCMR1|=TIM_CCMR1_IC1F_0;
	//Канал 2
	TIM4->CCMR1|=TIM_CCMR1_CC2S_0;
	TIM4->CCMR1|=TIM_CCMR1_IC2F_0;
	TIM4->CCER|=TIM_CCER_CC1P;
	
	TIM4->SMCR|=TIM_SMCR_TS_1 | TIM_SMCR_TS_2;
	TIM4->DIER|=TIM_DIER_CC1IE;
	TIM4->DIER|=TIM_DIER_CC2IE;
	TIM4->SMCR|=TIM_SMCR_SMS_2;//Master mode
	
	TIM4->CCER|=TIM_CCER_CC1E;//Режим захвата
	TIM4->CCER|=TIM_CCER_CC2E;//Режим захвата
	TIM4->CR1|=TIM_CR1_CEN;
}

void tim5Init1(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;//Тактирование потра A
	GPIOA->MODER&=~GPIO_MODER_MODER1_1;//PA0 на AF
	GPIOA->AFR[0]&=~(2<<4);
	GPIOA->MODER|=GPIO_MODER_MODER0_1;//PA0 на AF
	GPIOA->AFR[0]|=2<<0;
	RCC->APB1ENR|=RCC_APB1ENR_TIM5EN;//Включить тактирование таймера-счетчика 9
	TIM5->PSC=1680/4 ;//Делитель на 10КГц
	TIM5->ARR=0xFFFF;//Считаем до конца
	
	TIM5->CR1=0;
	TIM5->CCER=0;
	TIM5->SMCR=0;
	TIM5->DIER=0;
	TIM5->CCMR1=0;
	//Канал 1
	TIM5->CCMR1|=TIM_CCMR1_CC1S_0;
	TIM5->CCMR1|=TIM_CCMR1_IC1F_0;
	//Канал 2
	TIM5->CCMR1|=TIM_CCMR1_CC2S_1;
	TIM5->CCMR1|=TIM_CCMR1_IC2F_0;
	
	TIM5->SMCR|=TIM_SMCR_TS_0 | TIM_SMCR_TS_2;
	TIM5->DIER|=TIM_DIER_CC1IE;
	TIM5->SMCR|=TIM_SMCR_SMS_2;//Master mode
	
	TIM5->CCER|=TIM_CCER_CC2P;
	
	TIM5->CCER|=TIM_CCER_CC1E;//Режим захвата
	TIM5->CCER|=TIM_CCER_CC2E;//Режим захвата
	TIM5->CR1|=TIM_CR1_CEN;
}

void tim5Init2(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;//Тактирование потра A
	GPIOA->MODER&=~GPIO_MODER_MODER0_1;//PA0 на AF
	GPIOA->AFR[0]&=~(2<<0);
	GPIOA->MODER|=GPIO_MODER_MODER1_1;//PA0 на AF
	GPIOA->AFR[0]|=2<<4;
	RCC->APB1ENR|=RCC_APB1ENR_TIM5EN;//Включить тактирование таймера-счетчика 9
	TIM5->PSC=1680/4 ;//Делитель на 10КГц
	TIM5->ARR=0xFFFF;//Считаем до конца
	
	TIM5->CR1=0;
	TIM5->CCER=0;
	TIM5->SMCR=0;
	TIM5->DIER=0;
	TIM5->CCMR1=0;
	//Канал 1
	TIM5->CCMR1|=TIM_CCMR1_CC1S_1;
	TIM5->CCMR1|=TIM_CCMR1_IC1F_0;
	//Канал 2
	TIM5->CCMR1|=TIM_CCMR1_CC2S_0;
	TIM5->CCMR1|=TIM_CCMR1_IC2F_0;
	
	TIM5->SMCR|=TIM_SMCR_TS_1 | TIM_SMCR_TS_2;
	TIM5->DIER|=TIM_DIER_CC1IE;
	TIM5->SMCR|=TIM_SMCR_SMS_2;//Master mode
	
	TIM5->CCER|=TIM_CCER_CC1P;
	
	TIM5->CCER|=TIM_CCER_CC1E;//Режим захвата
	TIM5->CCER|=TIM_CCER_CC2E;//Режим захвата
	TIM5->CR1|=TIM_CR1_CEN;
}


// void receiveSBusDate(uint16_t* channels)
// {
// 	uint8_t i=0;
// 	//HAL_UART_Receive_DMA(&huart4,(uint8_t *)buffer,50);

// 	// if(xSemaphoreTake(dmaReciveSemaphoreHandle,portMAX_DELAY)==pdTRUE)
// 	// {
// 	// 	for (i = 0; i < 100; i++)
// 	// 	{
// 	// 		if (buffer[i] == 0x0f) break;
// 	// 	}
// 		channels[RIGHT_HORIZ]  = ((buffer[i+1]    |buffer[i+2]<<8)                 & 0x07FF);
// 		channels[RIGHT_VERT]  = ((buffer[i+2]>>3 |buffer[i+3]<<5)                 & 0x07FF);
// 		channels[LEFT_VERT]  = ((buffer[i+3]>>6 |buffer[i+4]<<2 |buffer[i+5]<<10)  & 0x07FF);
// 		channels[LEFT_HORIZ]  = ((buffer[i+5]>>1 |buffer[i+6]<<7)                 & 0x07FF);
// 		channels[RUDDR]  = ((buffer[i+6]>>4 |buffer[i+7]<<4)                 & 0x07FF);
// 		channels[ELEVDR]  = ((buffer[i+7]>>7 |buffer[i+8]<<1 |buffer[i+9]<<9)   & 0x07FF);
// 		channels[AILDR]  = ((buffer[i+9]>>2 |buffer[i+10]<<6)                & 0x07FF);
// 		channels[GEAR]  = ((buffer[i+10]>>5|buffer[i+11]<<3)                & 0x07FF);
// 		channels[8]  = ((buffer[i+12]   |buffer[i+13]<<8)                & 0x07FF);
// 		channels[9]  = ((buffer[i+13]>>3|buffer[i+14]<<5)                & 0x07FF);
// 		channels[10] = ((buffer[i+14]>>6|buffer[i+15]<<2|buffer[i+16]<<10) & 0x07FF);
// 		channels[11] = ((buffer[i+16]>>1|buffer[i+17]<<7)                & 0x07FF);
// 		channels[12] = ((buffer[i+17]>>4|buffer[i+18]<<4)                & 0x07FF);
// 		channels[13] = ((buffer[i+18]>>7|buffer[i+19]<<1|buffer[i+20]<<9)  & 0x07FF);
// 		channels[14] = ((buffer[i+20]>>2|buffer[i+21]<<6)                & 0x07FF);
// 		channels[15] = ((buffer[i+21]>>5|buffer[i+22]<<3)                & 0x07FF);
// 		channels[16] =((buffer[i+23])      & 0x0001) ?  2047: 0;
// 		channels[17] =((buffer[i+23] >> 1) & 0x0001) ?  2047: 0;
// 	// }
// }

