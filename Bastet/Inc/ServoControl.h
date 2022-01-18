#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H
#include "main.h"
#include "MotorControl.h"

typedef struct
{
	float targetFrontLeft; 
	float targetFrontRight;
	float targetRearLeft;
	float targetRearRight;   
}servoTarget_t;


typedef enum
{
	CALIBRATION=0x01,
	SET_ANGLE=0x02  
}servoFrame_t;

typedef struct
{
    servoTarget_t servoTarget; 
	servoFrame_t frameType;
	uint16_t frameID;
}Servo_t;

HAL_StatusTypeDef canServoFrameSend(Servo_t servos);

#endif