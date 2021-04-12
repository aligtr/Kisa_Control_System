#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H
#include "main.h"

typedef struct
{
	float currentAngle;
	float targetAngle;
	int16_t P;
	int16_t I;
	int16_t speed;	
	int16_t intergator;
}servo_t;

void PI_control(servo_t* Servo);
void getCurrentAngle(char i, servo_t* Servo, SPI_HandleTypeDef* hspi);
#endif