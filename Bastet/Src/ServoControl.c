#include "ServoControl.h"
uint32_t encoderDate=0;
uint8_t buff[4]={0};
void getCurrentAngle(char i, servo_t* Servo, SPI_HandleTypeDef* hspi){
	
	switch (i){
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

	Servo->currentAngle = encoderDate/4096*3.14*2;
}

void PI_control(servo_t* Servo){	
	Servo->intergator += (Servo->targetAngle-Servo->currentAngle);
	Servo->torque = (int16_t)((Servo->targetAngle-Servo->currentAngle)*Servo->P+Servo->intergator*Servo->I);
}
