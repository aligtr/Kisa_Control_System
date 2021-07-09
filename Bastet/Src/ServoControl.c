#include "ServoControl.h"
uint32_t encoderDate=0;
uint8_t buff[4]={0};

void setServo(servo_t Servo, char i)
{
	switch (i)
	{
		case 0:
			if (Servo.dir==1){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
				TIM1->CCR1 = 50;
			}
			else 
				if (Servo.dir==-1){
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
					TIM1->CCR1 = 50;
				}
				else {
					TIM1->CCR1 = 0xffff;
				}
		break;
		case 1:
			if (Servo.dir==1){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
				TIM8->CCR1 = 50;
			}
			else 
				if (Servo.dir==-1){
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
					TIM8->CCR1 = 50;
				}
				else {
					TIM8->CCR1 = 0xffff;
				}
		break;
		case 2:
			if (Servo.dir==1){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
				TIM9->CCR1 = 50;
			}
			else 
				if (Servo.dir==-1){
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
					TIM9->CCR1 = 50;
				}
				else {
					TIM9->CCR1 = 0xffff;
				}
		break;
		case 3:
			if (Servo.dir==1){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
				TIM12->CCR1 = 50;
			}
			else 
				if (Servo.dir==-1){
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
					TIM12->CCR1 = 50;
				}
				else {
					TIM12->CCR1 = 0xffff;
				}
		break;
	}
}

void getCurrentAngle(char i, servo_t* Servo, SPI_HandleTypeDef* hspi)
{
	
	switch (i)
	{
		case 0:
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
 			HAL_SPI_Receive(hspi,buff,3,100);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
		break;
		case 1:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_SPI_Receive(hspi,buff,3,100);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
		break;
		case 2:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_SPI_Receive(hspi,buff,3,100);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
		break;
		case 3:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
			HAL_SPI_Receive(hspi,buff,3,100);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		break;
	}
	encoderDate = ((uint32_t)buff[0]<<4 | (uint32_t)buff[3]<<3  | ((uint32_t)buff[2])/32);
	
	Servo->currentAngle = (float)encoderDate*3.14*2/4096-3.14;
}

void dirInit(void)
{	
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;
	GPIOB->MODER|=GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0;//PB0-PB3 в режим выходов
	GPIOB->OTYPER|=GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3;
}

void tim1Init(void)
{	
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOEEN;
	GPIOE->MODER|=GPIO_MODER_MODER9_1;//PE9,PC6,PE5,PB14 в режим альтернативной функции
	GPIOE->OTYPER|=GPIO_OTYPER_OT9;
	GPIOE->AFR[1]=(1<<4);//AF1 enable
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	TIM1->CCER = 0;
	TIM1->ARR = 150;
	TIM1->PSC = 1680/4-1;
	TIM1->BDTR |= TIM_BDTR_MOE;
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	TIM1->CCER |= TIM_CCER_CC1E;
	TIM1->CR1 |= TIM_CR1_CEN;
	//TIM1->DIER|=TIM_DIER_CC1IE;//Настройка прерываний по совпадению
	TIM1->CCR1=0xffff;
}

void tim8Init(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER|=GPIO_MODER_MODER7_1;//PE9,PC7,PE5,PB14 в режим альтернативной функции
	GPIOC->OTYPER|=GPIO_OTYPER_OT7;
	GPIOC->AFR[0]=(3<<24);//AF1 enable
	RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
	TIM8->CCER = 0;
	TIM8->ARR = 150;
	TIM8->PSC = 1680/4-1;
	TIM8->BDTR |= TIM_BDTR_MOE;
	TIM8->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
	TIM8->CCER |= TIM_CCER_CC2E;
	TIM8->CR1 |= TIM_CR1_CEN;
	//TIM8->DIER|=TIM_DIER_CC1IE;//Настройка прерываний по совпадению
	TIM8->CCR1=0xffff;
}

void tim9Init(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOEEN;
	GPIOE->MODER|=GPIO_MODER_MODER5_1;//PE5 в режим AF
	GPIOE->OTYPER|=GPIO_OTYPER_OT5;
	GPIOE->AFR[0]=(3<<20);
	RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
	TIM9->CCER = 0;
	TIM9->ARR = 150;
	TIM9->PSC = 1680/4-1;
	TIM9->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	TIM9->CCER |= TIM_CCER_CC1E;
	//TIM9->DIER|=TIM_DIER_CC1IE;
	TIM9->CR1 |= TIM_CR1_CEN;
	TIM9->CCR1=0xffff;
}

void tim2Init(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;
	GPIOB->MODER|=GPIO_MODER_MODER14_1;//PB14 в режим AF
	GPIOB->OTYPER|=GPIO_OTYPER_OT14;
	GPIOB->AFR[1]=(9<<24);
	RCC->APB1ENR |= RCC_APB1ENR_TIM12EN;
	TIM12->CCER = 0;
	TIM12->ARR = 150;
	TIM12->PSC = 1680/4-1;
	TIM12->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	TIM12->CCER |= TIM_CCER_CC1E;
	//TIM12->DIER|=TIM_DIER_CC1IE;
	TIM12->CR1 |= TIM_CR1_CEN;
	TIM12->CCR1=0xffff;
}

