#ifndef PDU_H
#define PDU_H    
#include "main.h"
#include "SBusReceiver.h"

typedef struct 
{
    uint16_t vel_mean;
    uint16_t dir_mean;
    uint16_t rx_mean;
	uint16_t elevdr;
	uint16_t aildr;
	uint16_t gear;
	uint16_t rudr;
    uint16_t ry_mean;
    uint16_t mode;
} PduData_t;


char checkDate(uint32_t date);
#endif