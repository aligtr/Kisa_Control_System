#ifndef SERVO_H
#define SERVO_H

#include "stm32f407xx.h"

typedef struct{
	uint16_t pulseCounter;
	uint16_t aimPulse;
	uint8_t dirPulse;
	uint16_t encoderDate;
	uint16_t aimAngle; //from 0 to 4096
	uint8_t calibFlag;
}Servo_t;

#define RESOLUTION 3600
#define REDUCTOR 30

void tim1Init(void);
void tim8Init(void);
void tim9Init(void);
void tim12Init(void);
void Calibration(void);
void servoControl();
#endif