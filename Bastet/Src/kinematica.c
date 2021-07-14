#include "kinematica.h"

#define pi 3.14159265
#define deg 57.29577951

float gam=0;
float R=9999;
float V=1;
float L=0.945;
float C=0.63;
uint16_t vel_period_max=502;
uint16_t vel_period_zero=385;
uint16_t dir_period_max=502;
uint16_t dir_period_zero=385;
uint16_t rx_period_zero=385;
uint16_t rx_period_max=502;
uint16_t ry_period_zero=385;
uint16_t ry_period_max=502;
float k_vel=5;
float k_dir=pi/2;
float k_gam=pi/2;
float r_wheel=0.2;
char constract;

int d_pwm_ch1=0;
int d_pwm_ch2=0;
int d_pwm_ch3=0;
int d_pwm_ch4=0;

int pwm_count_ch1=0;
int pwm_count_ch2=0;
int pwm_count_ch3=0;
int pwm_count_ch4=0;
int pwm_aim_ch1=0;
int pwm_aim_ch2=0;
int pwm_aim_ch3=0;
int pwm_aim_ch4=0;

double sign(double a){
	if (a>0) return 1;
	else if (a<0) return -1;
	else return 0;
}

int16_t intSign(double a){
	if (a>0) return 1;
	else if (a<0) return -1;
	else return 0;
}

// void TIM1_CC_IRQHandler(void)
// {
// 	if (TIM1->SR & TIM_SR_CC1IF) {
// 		if (pwm_count_ch1==pwm_aim_ch1) TIM1->CCR1 = 0xffff;
// 		else pwm_count_ch1+=d_pwm_ch1;
// 		TIM1->SR&=~TIM_SR_CC1IF;
// 	}
// }

// void TIM8_CC_IRQHandler(void)
// {
// 	if (TIM8->SR & TIM_SR_CC1IF) {
// 		if (pwm_count_ch2==pwm_aim_ch2) TIM8->CCR1 =  0xffff;
// 		else pwm_count_ch2+=d_pwm_ch2;
// 		TIM8->SR&=~TIM_SR_CC1IF;
// 	}
// }

// void TIM1_BRK_TIM9_IRQHandler(void)
// {
// 	if (TIM9->SR & TIM_SR_CC1IF) {
// 		if (pwm_count_ch3==pwm_aim_ch3) TIM9->CCR1 =  0xffff;
// 		else pwm_count_ch3+=d_pwm_ch3;
// 		TIM9->SR&=~TIM_SR_CC1IF;
// 	}
// }
// void TIM8_BRK_TIM12_IRQHandler(void)
// {
// 	if (TIM12->SR & TIM_SR_CC1IF) {
// 		if (pwm_count_ch4==pwm_aim_ch4) TIM12->CCR1 =  0xffff;
// 		else pwm_count_ch4+=d_pwm_ch4;
// 		TIM12->SR&=~TIM_SR_CC1IF;
// 	}
// }

// void tim1Init(void)
// {
// 	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN;
// 	GPIOC->MODER|=GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0;//PC0-PC3 в режим выходов
// 	GPIOC->OTYPER|=GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3;
// 	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOEEN;
// 	GPIOE->MODER|=GPIO_MODER_MODER9_1;//PE9,PC6,PE5,PB14 в режим альтернативной функции
// 	GPIOE->OTYPER|=GPIO_OTYPER_OT9;
// 	GPIOE->AFR[1]=(1<<4);//AF1 enable
// 	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
// 	TIM1->CCER = 0;
// 	TIM1->ARR = 150;
// 	TIM1->PSC = 1680/4-1;
// 	TIM1->BDTR |= TIM_BDTR_MOE;
// 	TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
// 	TIM1->CCER |= TIM_CCER_CC1E;
// 	TIM1->CR1 |= TIM_CR1_CEN;
// 	TIM1->DIER|=TIM_DIER_CC1IE;//Настройка прерываний по совпадению
// 	TIM1->CCR1=0xffff;
// }

// void tim8Init(void)
// {
// 	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN;
// 	GPIOC->MODER|=GPIO_MODER_MODER6_1;//PE9,PC6,PE5,PB14 в режим альтернативной функции
// 	GPIOC->OTYPER|=GPIO_OTYPER_OT6;
// 	GPIOC->AFR[0]=(3<<24);//AF1 enable
// 	RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
// 	TIM8->CCER = 0;
// 	TIM8->ARR = 150;
// 	TIM8->PSC = 1680/4-1;
// 	TIM8->BDTR |= TIM_BDTR_MOE;
// 	TIM8->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
// 	TIM8->CCER |= TIM_CCER_CC1E;
// 	TIM8->CR1 |= TIM_CR1_CEN;
// 	TIM8->DIER|=TIM_DIER_CC1IE;//Настройка прерываний по совпадению
// 	TIM8->CCR1=0xffff;
// }

// void tim9Init(void)
// {
// 	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOEEN;
// 	GPIOE->MODER|=GPIO_MODER_MODER5_1;//PE5 в режим AF
// 	GPIOE->OTYPER|=GPIO_OTYPER_OT5;
// 	GPIOE->AFR[0]=(3<<20);
// 	RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
// 	TIM9->CCER = 0;
// 	TIM9->ARR = 150;
// 	TIM9->PSC = 1680/4-1;
// 	TIM9->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
// 	TIM9->CCER |= TIM_CCER_CC1E;
// 	TIM9->DIER|=TIM_DIER_CC1IE;
// 	TIM9->CR1 |= TIM_CR1_CEN;
// 	TIM9->CCR1=0xffff;
// }

// void tim12Init(void)
// {
// 	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;
// 	GPIOB->MODER|=GPIO_MODER_MODER14_1;//PB14 в режим AF
// 	GPIOB->OTYPER|=GPIO_OTYPER_OT14;
// 	GPIOB->AFR[1]=(9<<24);
// 	RCC->APB1ENR |= RCC_APB1ENR_TIM12EN;
// 	TIM12->CCER = 0;
// 	TIM12->ARR = 150;
// 	TIM12->PSC = 1680/4-1;
// 	TIM12->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
// 	TIM12->CCER |= TIM_CCER_CC1E;
// 	TIM12->DIER|=TIM_DIER_CC1IE;
// 	TIM12->CR1 |= TIM_CR1_CEN;
// 	TIM12->CCR1=0xffff;
// }

void normaliz(uint16_t vel_mean, uint16_t rx_mean, uint16_t dir_mean, uint16_t ry_mean, uint16_t elevdr){
	float d_vel=0;
	float d_ry=0;
	float Rg0=0;
	float d_rx=0;
	float d_dir=0;
	float vel_coef=0;
	// if (elevdr==1){
	// 	gam=0;
	// 	V=vel_mean/100;
	// 	R=L/2/tan(dir_mean/100);
	// 	if (fabs(R)<0.1) R=0.00001*sign(R);
	// 	if (fabs(R)>50) R=10000;
	// 	return;
	// }
	d_dir=(dir_mean-dir_period_zero);
	d_vel=(vel_mean-vel_period_zero);
	d_rx=(rx_mean-rx_period_zero);
	d_ry=ry_mean-ry_period_zero;
	
	if (fabs(d_dir)<20) d_dir=0;
	if (fabs(d_vel)<20) d_vel=0;
	if (fabs(d_rx)<20) d_rx=0;
	if (fabs(d_ry)<20) d_ry=0;
	
	Rg0=d_dir*k_dir/(dir_period_max-dir_period_zero);
	if (fabs(Rg0)>k_dir) Rg0=sign(Rg0)*k_dir;
	R=L/2/tan(Rg0)*10;
	if (fabs(R)<0.1) R=0.1*sign(R);//минимальный радиус поворота
	if (fabs(R)>50) R=10000;
	
	V=(d_vel)/((float)(vel_period_max-vel_period_zero));
	vel_coef=(d_rx/((float)(rx_period_max-rx_period_zero)));
	V=V*vel_coef;

	gam=(d_ry)/(ry_period_max-ry_period_zero)*k_gam;
	if (fabs(gam)<0.01) gam=0;
	if (fabs(gam)>(pi/4-0.1)) gam=pi/4*sign(gam);//предельные углы поворота
	if (gam!=0) R=100000;
}

void kinematica(uint32_t mode, Motor_t* Motors, servoTarget_t* Servo){
	float x,y;
	float Rfl, Rfr, Rrl, Rrr;  
	float Rm,gm;
	float gfl, gfr, grl, grr;
	float dgfl, dgfr, dgrl, dgrr;
	float Vfl, Vfr, Vrl, Vrr;
	mode=2;
	switch(mode)
	{
		case 1:
			k_vel=(constract);// ???????? 3 ??????: 0, V, constract
			break;
		case 2:
			k_vel=5;// ???????? 3 ??????: 0, V, constract
			break;
		case 3:
			k_vel=0;
		break;
	}
	V=V*k_vel;
	x=R*cos(pi/2+gam);
	y=R*sin(pi/2+gam);
	if (R>0){
		gfl=atan2(L/2-x, y-C/2);
		gfr=atan2(L/2-x, y+C/2);
		grl=atan2(-L/2-x, y-C/2);
		grr=atan2(-L/2-x, y+C/2);
	}
	else{
		grr=atan2(L/2+x, -y-C/2);
		grl=atan2(L/2+x, -y+C/2);
		gfr=atan2(-L/2+x, -y-C/2);
		gfl=atan2(-L/2+x, -y+C/2);
	}

	Rfl=fabs(y-C/2)/fabs(cos(gfl));
	Rfr=fabs(y+C/2)/fabs(cos(gfr));
	Rrl=fabs(y-C/2)/fabs(cos(grl));
	Rrr=fabs(y+C/2)/fabs(cos(grr));

	if(Rfl>1000) Rfl=100000;
	if(Rfr>1000) Rfr=100000;
	if(Rrl>1000) Rrl=100000;
	if(Rrr>1000) Rrr=100000;
	Rm=Rfl;
	if (Rm<Rfr) Rm=Rfr;
	if (Rm<Rrl) Rm=Rrl;
	if (Rm<Rrr) Rm=Rrr;

	Vfl=-V*Rfl/Rm;
	Vfr=V*Rfr/Rm;
	Vrl=V*Rrl/Rm;
	Vrr=V*Rrr/Rm;
	if (fabs(gfl)<0.01) gfl=0;
	if (fabs(gfr)<0.01) gfr=0;
	if (fabs(grl)<0.01) grl=0;
	if (fabs(grr)<0.01) grr=0;

	// pwm_aim_ch1=gfl*deg*800*15/360;
	// pwm_aim_ch2=gfr*deg*800*15/360;
	// pwm_aim_ch3=grl*deg*800*15/360;
	// pwm_aim_ch4=grr*deg*800*15/360;
	
	// dgfl=fabs(pwm_aim_ch1-pwm_count_ch1); 
	// dgfr=fabs(pwm_aim_ch2-pwm_count_ch2);
	// dgrl=fabs(pwm_aim_ch3-pwm_count_ch3);
	// dgrr=fabs(pwm_aim_ch4-pwm_count_ch4);	
	
	// gm=dgfl;
	// if (gm<dgfr) gm=dgfr;
	// if (gm<dgrl) gm=dgrl;
	// if (gm<dgrr) gm=dgrr;
	// TIM1->ARR=150*gm/dgfl;
	// TIM8->ARR=150*gm/dgfr;
	// TIM9->ARR=150*gm/dgrl;
	// TIM12->ARR=150*gm/dgrr;
							 
	// if (pwm_aim_ch1>pwm_count_ch1) {//ch1
	// 	GPIOC->BSRR=GPIO_BSRR_BS0;
	// 	TIM1->CCR1 = 50;
	// 	d_pwm_ch1=1;
	// }
	// else if (pwm_aim_ch1<pwm_count_ch1) {
	// 	TIM1->CCR1 = 50;
	// 	d_pwm_ch1=-1;
	// 	GPIOC->BSRR=GPIO_BSRR_BR0;
	// }
	// else TIM1->CCR1 = 0xffff;
	// if (pwm_aim_ch2>pwm_count_ch2) {//ch2
	// 	GPIOC->BSRR=GPIO_BSRR_BR1;
	// 	TIM8->CCR1 = 50;
	// 	d_pwm_ch2=1;
	// }
	// else if (pwm_aim_ch2<pwm_count_ch2) {
	// 	TIM8->CCR1 = 50;
	// 	GPIOC->BSRR=GPIO_BSRR_BS1;
	// 	d_pwm_ch2=-1;
	// }
	// else TIM8->CCR1 = 0xffff;
	// if (pwm_aim_ch3>pwm_count_ch3) {//ch3
	// 	GPIOC->BSRR=GPIO_BSRR_BR2;
	// 	TIM9->CCR1 = 50;
	// 	d_pwm_ch3=1;
	// }
	// else if (pwm_aim_ch3<pwm_count_ch3) {
	// 	TIM9->CCR1 = 50;
	// 	GPIOC->BSRR=GPIO_BSRR_BS2;
	// 	d_pwm_ch3=-1;
	// }
	// else TIM9->CCR1 = 0xffff;
	// if (pwm_aim_ch4>pwm_count_ch4) {//ch4
	// 	GPIOC->BSRR=GPIO_BSRR_BS3;
	// 	TIM12->CCR1 = 50;
	// 	d_pwm_ch4=1;
	// }
	// else if (pwm_aim_ch4<pwm_count_ch4) {
	// 	TIM12->CCR1 = 50;
	// 	GPIOC->BSRR=GPIO_BSRR_BR3;
	// 	d_pwm_ch4=-1;
	// }
	// else TIM12->CCR1 = 0xffff;

	Motors[0].command=CHANGE_SPEED;
	Motors[0].prevRefImpact=Motors[0].refImpact;
	Motors[0].refImpact=(int16_t)(((Vfl*60/r_wheel/2/pi)));

	Motors[1].command=CHANGE_SPEED;
	Motors[1].prevRefImpact=Motors[1].refImpact;
	Motors[1].refImpact=(int16_t)(Vfr*60/r_wheel/2/pi);
	
	Motors[2].command=CHANGE_SPEED;
	Motors[2].prevRefImpact=Motors[2].refImpact;
	Motors[2].refImpact=(int16_t)(Vrl*60/r_wheel/2/pi);
	
	Motors[3].command=CHANGE_SPEED;
	Motors[3].prevRefImpact=Motors[3].refImpact;
	Motors[3].refImpact=(int16_t)(Vrr*60/r_wheel/2/pi);

	Servo->targetFrontLeft=gfl;
	Servo->targetFrontRight=gfr;
	Servo->targetRearLeft=grl;
	Servo->targetRearRight=grr;
}
