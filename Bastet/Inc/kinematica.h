#ifndef KINEMATICA_H
#define KINEMATICA_H

#include "main.h"
#include "math.h"
#include "MotorControl.h"
#include "ServoControl.h"

double sign(double a);
void kinematica(uint32_t mode, Motor_t* Motors, servo_t* Servo);
void normaliz(uint32_t vel_mean, uint32_t rx_mean, uint32_t dir_mean, uint32_t ry_mean);

#define pi 3.14159265
#define deg 57.29577951

#endif