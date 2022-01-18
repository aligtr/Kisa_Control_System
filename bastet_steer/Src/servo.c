#include "servo.h"
#define pi 3.14159265
#define deg 57.29577951

Servo_t servo[4];


void TIM1_CC_IRQHandler(void)
{
	if (TIM1->SR & TIM_SR_CC1IF) {
		if (servo[0].aimPulse==servo[0].pulseCounter) TIM1->CCR1 = 0xffff;
		else servo[0].pulseCounter+=servo[0].dirPulse;
		TIM1->SR&=~TIM_SR_CC1IF;
	}
}

void TIM8_CC_IRQHandler(void)
{
	if (TIM8->SR & TIM_SR_CC1IF) {
		if (servo[1].aimPulse==servo[1].pulseCounter) TIM8->CCR1 =  0xffff;
		else servo[1].pulseCounter+=servo[1].dirPulse;
		TIM8->SR&=~TIM_SR_CC1IF;
	}
}

void TIM1_BRK_TIM9_IRQHandler(void)
{
	if (TIM9->SR & TIM_SR_CC1IF) {
		if (servo[2].aimPulse==servo[2].pulseCounter) TIM9->CCR1 =  0xffff;
		else servo[2].pulseCounter+=servo[2].dirPulse;
		TIM9->SR&=~TIM_SR_CC1IF;
	}
}
void TIM8_BRK_TIM12_IRQHandler(void)
{
	if (TIM12->SR & TIM_SR_CC1IF) {
		if (servo[3].aimPulse==servo[3].pulseCounter) TIM12->CCR1 =  0xffff;
		else servo[3].pulseCounter+=servo[3].dirPulse;
		TIM12->SR&=~TIM_SR_CC1IF;
	}
}

void tim1Init(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER|=GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0;//PC0-PC3 в режим выходов
	GPIOC->OTYPER|=GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3;
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOEEN;
	GPIOE->MODER|=GPIO_MODER_MODE9_1;//PE9,PC6,PE5,PB14 в режим альтернативной функции
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
	TIM1->DIER|=TIM_DIER_CC1IE;//Настройка прерываний по совпадению
	TIM1->CCR1=0xffff;
}

void tim8Init(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER|=GPIO_MODER_MODE6_1;//PE9,PC6,PE5,PB14 в режим альтернативной функции
	GPIOC->OTYPER|=GPIO_OTYPER_OT6;
	GPIOC->AFR[0]=(3<<24);//AF1 enable
	RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
	TIM8->CCER = 0;
	TIM8->ARR = 150;
	TIM8->PSC = 1680/4-1;
	TIM8->BDTR |= TIM_BDTR_MOE;
	TIM8->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	TIM8->CCER |= TIM_CCER_CC1E;
	TIM8->CR1 |= TIM_CR1_CEN;
	TIM8->DIER|=TIM_DIER_CC1IE;//Настройка прерываний по совпадению
	TIM8->CCR1=0xffff;
}

void tim9Init(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOEEN;
	GPIOE->MODER|=GPIO_MODER_MODE5_1;//PE5 в режим AF
	GPIOE->OTYPER|=GPIO_OTYPER_OT5;
	GPIOE->AFR[0]=(3<<20);
	RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
	TIM9->CCER = 0;
	TIM9->ARR = 150;
	TIM9->PSC = 1680/4-1;
	TIM9->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	TIM9->CCER |= TIM_CCER_CC1E;
	TIM9->DIER|=TIM_DIER_CC1IE;
	TIM9->CR1 |= TIM_CR1_CEN;
	TIM9->CCR1=0xffff;
}

void tim12Init(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;
	GPIOB->MODER|=GPIO_MODER_MODE14_1;//PB14 в режим AF
	GPIOB->OTYPER|=GPIO_OTYPER_OT14;
	GPIOB->AFR[1]=(9<<24);
	RCC->APB1ENR |= RCC_APB1ENR_TIM12EN;
	TIM12->CCER = 0;
	TIM12->ARR = 150;
	TIM12->PSC = 1680/4-1;
	TIM12->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	TIM12->CCER |= TIM_CCER_CC1E;
	TIM12->DIER|=TIM_DIER_CC1IE;
	TIM12->CR1 |= TIM_CR1_CEN;
	TIM12->CCR1=0xffff;
}

void servoControl(){  
	uint16_t dgfl, dgfr, dgrl, dgrr;
	
	uint16_t gm;

	servo[0].aimPulse=servo[0].aimAngle*RESOLUTION*REDUCTOR/4096;
	servo[1].aimPulse=servo[1].aimAngle*RESOLUTION*REDUCTOR/4096;
	servo[2].aimPulse=servo[2].aimAngle*RESOLUTION*REDUCTOR/4096;
	servo[3].aimPulse=servo[3].aimAngle*RESOLUTION*REDUCTOR/4096;
	
	dgfl=abs(servo[0].aimPulse-servo[0].pulseCounter); 
	dgfr=abs(servo[1].aimPulse-servo[1].pulseCounter);
	dgrl=abs(servo[2].aimPulse-servo[2].pulseCounter);
	dgrr=abs(servo[3].aimPulse-servo[3].pulseCounter);	
	
	gm=dgfl;
	if (gm<dgfr) gm=dgfr;
	if (gm<dgrl) gm=dgrl;
	if (gm<dgrr) gm=dgrr;
	TIM1->ARR=150*gm/dgfl;
	TIM8->ARR=150*gm/dgfr;
	TIM9->ARR=150*gm/dgrl;
	TIM12->ARR=150*gm/dgrr;
							 
	if (servo[0].aimPulse>servo[0].pulseCounter) {//ch1
		GPIOC->BSRR=GPIO_BSRR_BS0;
		TIM1->CCR1 = 50;
		servo[0].dirPulse=1;
	}
	else if (servo[0].aimPulse<servo[0].pulseCounter) {
		TIM1->CCR1 = 50;
		servo[0].dirPulse=-1;
		GPIOC->BSRR=GPIO_BSRR_BR0;
	}
	else TIM1->CCR1 = 0xffff;
	if (servo[1].aimPulse>servo[1].pulseCounter) {//ch2
		GPIOC->BSRR=GPIO_BSRR_BR1;
		TIM8->CCR1 = 50;
		servo[1].dirPulse=1;
	}
	else if (servo[1].aimPulse<servo[1].pulseCounter) {
		TIM8->CCR1 = 50;
		GPIOC->BSRR=GPIO_BSRR_BS1;
		servo[1].dirPulse=-1;
	}
	else TIM8->CCR1 = 0xffff;
	if (servo[2].aimPulse>servo[2].pulseCounter) {//ch3
		GPIOC->BSRR=GPIO_BSRR_BR2;
		TIM9->CCR1 = 50;
		servo[2].dirPulse=1;
	}
	else if (servo[2].aimPulse<servo[2].pulseCounter) {
		TIM9->CCR1 = 50;
		GPIOC->BSRR=GPIO_BSRR_BS2;
		servo[2].dirPulse=-1;
	}
	else TIM9->CCR1 = 0xffff;
	if (servo[3].aimPulse>servo[3].pulseCounter) {//ch4
		GPIOC->BSRR=GPIO_BSRR_BS3;
		TIM12->CCR1 = 50;
		servo[3].dirPulse=1;
	}
	else if (servo[3].aimPulse<servo[3].pulseCounter) {
		TIM12->CCR1 = 50;
		GPIOC->BSRR=GPIO_BSRR_BR3;
		servo[3].dirPulse=-1;
	}
	else TIM12->CCR1 = 0xffff;
}

void Calibration(){
	
	if (servo[0].calibFlag==0){
		if (servo[0].aimAngle>servo[0].encoderDate) {//ch1
			GPIOC->BSRR=GPIO_BSRR_BS0;
			TIM1->CCR1 = 50;
			servo[0].dirPulse=1;
		}
		else {
			if (servo[0].aimAngle<servo[0].encoderDate) {
			TIM1->CCR1 = 50;
			servo[0].dirPulse=-1;
			GPIOC->BSRR=GPIO_BSRR_BR0;
			}
			else {
				TIM1->CCR1 = 0xffff;
				servo[0].calibFlag=1;
			}
		}
	}
	
	if (servo[1].calibFlag==0){
		if (servo[1].aimAngle>servo[1].encoderDate) {//ch2
			GPIOC->BSRR=GPIO_BSRR_BR1;
			TIM8->CCR1 = 50;
			servo[1].dirPulse=1;
		}
		else{
			if (servo[1].aimAngle<servo[1].encoderDate) {
				TIM8->CCR1 = 50;
				GPIOC->BSRR=GPIO_BSRR_BS1;
				servo[1].dirPulse=-1;
			}
			else {
				TIM8->CCR1 = 0xffff;
				servo[1].calibFlag=1;
			}
		}
	}
	
	if (servo[2].calibFlag==0){
		if (servo[2].aimAngle>servo[2].encoderDate) {//ch3
			GPIOC->BSRR=GPIO_BSRR_BR2;
			TIM9->CCR1 = 50;
			servo[2].dirPulse=1;
		}
		else {
			if (servo[2].aimAngle<servo[2].encoderDate) {
				TIM9->CCR1 = 50;
				GPIOC->BSRR=GPIO_BSRR_BS2;
				servo[2].dirPulse=-1;
			}
			else {
				TIM9->CCR1 = 0xffff;
				servo[2].calibFlag=1;
			}
		}
	}
	
	if (servo[3].calibFlag==0){
		if (servo[3].aimAngle>servo[3].encoderDate) {//ch4
			GPIOC->BSRR=GPIO_BSRR_BS3;
			TIM12->CCR1 = 50;
			servo[3].dirPulse=1;
		}
		else {
			if (servo[3].aimAngle<servo[3].encoderDate) {
				TIM12->CCR1 = 50;
				GPIOC->BSRR=GPIO_BSRR_BR3;
				servo[3].dirPulse=-1;
			}
			else {
				TIM12->CCR1 = 0xffff;
				servo[3].calibFlag=1;
			}
		}
	}
	
}