#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H
#include "main.h"
#include "MotorControl.h"

#define SERVO_FL 0
#define SERVO_FR 1
#define SERVO_RL 2
#define SERVO_RR 3

typedef struct
{
	motorID_t servoId;
	float currentAngle;
	float targetAngle;
	int16_t P;
	int16_t I;
	int16_t torque;	
	int16_t intergator;
}servo_t;

typedef struct
{
	float targetFrontLeft; 
	float targetFrontRight;
	float targetRearLeft;
	float targetRearRight;   
}servoTarget_t;

void PI_control(servo_t* Servo);
void getCurrentAngle(char i, servo_t* Servo, SPI_HandleTypeDef* hspi);
#endif