#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H
#include "main.h"

typedef struct
{
	uint16_t currentAngle;
	uint16_t targetAngle;
	uint16_t P;
	uint16_t I;
	uint16_t speed;	
	uint16_t intergator;
}servo;

void PI_control(servo * Servo);
void getCurrentAngle(char i, servo * Servo, SPI_HandleTypeDef * hspi);
#endif