#ifndef KINEMATICA_H
#define KINEMATICA_H

#include "main.h"
#include "math.h"
#include "MotorControl.h"
#include "ServoControl.h"

double sign(double a);
int16_t intSign(double a);
void kinematica(uint32_t mode, Motor_t* Motors, servoTarget_t* Servo);
void normaliz(uint16_t vel_mean, uint16_t rx_mean, uint16_t dir_mean, uint16_t ry_mean, uint16_t elevdr);
// void tim1Init(void);
// void tim8Init(void);
// void tim9Init(void);
// void tim12Init(void);

#define pi 3.14159265
#define deg 57.29577951

#endif