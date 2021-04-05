#ifndef KINEMATICA_H
#define KINEMATICA_H

#include "main.h"

double sign(double a);
void kinematica(uint32_t mode);
void normaliz(uint32_t vel_mean, uint32_t rx_mean, uint32_t dir_mean, uint32_t ry_mean);

#define pi 3.14159265
#define deg 57.29577951
static float gam=0;
static float R=9999;
static float V=1;
static float L=0.945;
static float C=0.63;
static uint16_t vel_period_max=320;
static uint16_t vel_period_zero=596;
static uint16_t dir_period_max=320;
static uint16_t dir_period_zero=597;
static uint16_t rx_period_zero=439;
static uint16_t rx_period_max=320;
static uint16_t ry_period_zero=596;
static uint16_t ry_period_max=320;
static float k_vel=5;
static float k_dir=pi;
static float k_gam=pi;

static float gfl, gfr, grl, grr;
static float Vfl, Vfr, Vrl, Vrr;

static char constract;

#endif