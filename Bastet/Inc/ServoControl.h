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
	int16_t dir;
	float dAngle;
	int16_t initFlag;
}servo_t;

typedef struct
{
	float targetFrontLeft; 
	float targetFrontRight;
	float targetRearLeft;
	float targetRearRight;   
}servoTarget_t;

void getCurrentAngle(char i, servo_t* Servo, SPI_HandleTypeDef* hspi);
void tim1Init(void);
void tim8Init(void);
void tim9Init(void);
void tim12Init(void);
void dirInit(void);
void setServo(servo_t Servos, char i);
#endif